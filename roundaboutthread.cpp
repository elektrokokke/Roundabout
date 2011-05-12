/*
    Copyright 2011 Arne Jacobs <jarne@jarne.de>

    This file is part of Roundabout.

    Roundabout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Roundabout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Roundabout.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "roundaboutthread.h"
#include "roundaboutsequencer.h"

RoundaboutThread::RoundaboutThread(QObject *parent) :
    QThread(parent),
    shutdown(false),
    client(0),
    sequencer(0),
    activeSequencer(0),
    stepsPerBeat(4),
    stepExpectedAtNextBufferBegin(false)
{
    midiInput.reserve(4096);
    midiOutput.reserve(4096);
    inboundEventsInterfaces.reserve(1024);
    // connect to the jack server:
    client = jack_client_open("Roundabout", JackNullOption, 0);
    if (client) {
        bool success = true;
        sampleRate = jack_get_sample_rate(client);
        // register process callback:
        success = success && (jack_set_process_callback(client, process, this) == 0);
        // register ports:
        midiInputPort = jack_port_register(client, "midi in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
        midiOutputPort = jack_port_register(client, "midi out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
        audioOutputPort = jack_port_register(client, "audio out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        success = success && midiInputPort && midiOutputPort && audioOutputPort;
        // start the jack client:
        success = success && (jack_activate(client) == 0);
        if (!success) {
            jack_client_close(client);
            client = 0;
        }
    }
    // start the QThread:
    if (isValid()) {
        start();
    }
}

RoundaboutThread::~RoundaboutThread()
{
    if (isValid()) {
        // close the jack client:
        jack_client_close(client);
        // send a shutdown event to the QThread:
        RoundaboutThreadOutboundEvent event;
        event.eventType = RoundaboutThreadOutboundEvent::SHUTDOWN;
        writeOutboundEvent(event);
        outboundCondition.wakeAll();
        // wait for the thread to finish:
        wait();
    }
}

bool RoundaboutThread::isValid() const
{
    return client;
}

void RoundaboutThread::processOutboundEvents()
{
    // process our events:
    OutboundEventsHelper<RoundaboutThreadOutboundEvent>::processOutboundEvents();
    // process other events:
    for (int i = 0; i < outboundEventsInterfaces.size(); i++) {
        outboundEventsInterfaces[i]->processOutboundEvents();
    }
}

void RoundaboutThread::processInboundEvents()
{
    // process our events:
    InboundEventsHelper<RoundaboutThreadInboundEvent>::processInboundEvents();
    // process other events:
    for (int i = 0; i < inboundEventsInterfaces.size(); i++) {
        inboundEventsInterfaces[i]->processInboundEvents();
    }
}

void RoundaboutThread::createSequencer()
{
    RoundaboutSequencer *sequencer = new RoundaboutSequencer(this);
    RoundaboutThreadInboundEvent inboundEvent;
    inboundEvent.eventType = RoundaboutThreadInboundEvent::CREATE_SEQUENCER;
    inboundEvent.sequencer = sequencer;
    writeInboundEvent(inboundEvent);
}

void RoundaboutThread::run()
{
    for (; !shutdown; ) {
        // wait for outbound events:
        QMutexLocker locker(&outboundMutex);
        outboundCondition.wait(&outboundMutex);
        processOutboundEvents();
    }
}

void RoundaboutThread::processInboundEvent(RoundaboutThreadInboundEvent &inboundEvent)
{
    if (inboundEvent.eventType == RoundaboutThreadInboundEvent::CREATE_SEQUENCER) {
        inboundEventsInterfaces.append(inboundEvent.sequencer);
        if (sequencer == 0) {
            sequencer = inboundEvent.sequencer;
        }
        sequencers.append(inboundEvent.sequencer);
        RoundaboutThreadOutboundEvent outboundEvent;
        outboundEvent.eventType = RoundaboutThreadOutboundEvent::CREATED_SEQUENCER;
        outboundEvent.sequencer = inboundEvent.sequencer;
        writeOutboundEvent(outboundEvent);
    }
}

void RoundaboutThread::processOutboundEvent(RoundaboutThreadOutboundEvent &event)
{
    if (event.eventType == RoundaboutThreadOutboundEvent::CREATED_SEQUENCER) {
        outboundEventsInterfaces.append(event.sequencer);
        createdSequencer(event.sequencer);
    } else if (event.eventType == RoundaboutThreadOutboundEvent::SHUTDOWN) {
        shutdown = true;
    }
}

int RoundaboutThread::process(jack_nframes_t nframes)
{
    // get transport state:
    jack_position_t currentPos;
    jack_transport_state_t currentState = jack_transport_query(client, &currentPos);

    // get midi input buffer:
    void *midiInputBuffer = jack_port_get_buffer(midiInputPort, nframes);
    // copy input midi events:
    midiInput.resize(0);
    jack_nframes_t midiInputEventCount = jack_midi_get_event_count(midiInputBuffer);
    jack_nframes_t midiInputEventIndex = 0;
    // get midi output buffer:
    void *midiOutputBuffer = jack_port_get_buffer(midiOutputPort, nframes);
    jack_midi_clear_buffer(midiOutputBuffer);
    processInboundEvents();

    if (sequencer) {
        if ((currentPos.valid & JackPositionBBT) && (currentState == JackTransportRolling)) {
            jack_nframes_t bbt_offset = (currentPos.valid & JackBBTFrameOffset ? currentPos.bbt_offset : 0);
            double framesPerMinute = 60.0 * currentPos.frame_rate;
            double ticksPerMinute = (double)currentPos.ticks_per_beat * (double)currentPos.beats_per_minute;
            double ticksPerFrame = ticksPerMinute / framesPerMinute;
            // current tick is bbt_offset frames before the first frame
            double currentTick = (double)bbt_offset * ticksPerFrame + (double)currentPos.tick;
            // beat position is current tick / ticks per beat:
            double currentBeat = currentTick / (double)currentPos.ticks_per_beat + (double)(currentPos.beat - 1);
            double beatPosition = currentBeat - (int)currentBeat;
            // current step is position in beat * steps per beat:
            double currentStep = beatPosition * (double)stepsPerBeat;
            double stepPosition = currentStep - (int)currentStep;
            double framesPerStep = framesPerMinute / ((double)currentPos.beats_per_minute * (double)stepsPerBeat);
            jack_nframes_t nextStep = (jack_nframes_t)(framesPerStep - stepPosition * framesPerStep);
            if (stepExpectedAtNextBufferBegin && (nextStep > nframes / 2)) {
                nextStep = 0;
                stepPosition += 1.0;
            }
            for (; nextStep < nframes; ) {
                // get all midi input events up to nextStep:
                midiInput.resize(0);
                for (; midiInputEventIndex < midiInputEventCount; ) {
                    jack_midi_event_t jackMidiEvent;
                    jack_midi_event_get(&jackMidiEvent, midiInputBuffer, midiInputEventIndex);
                    if (jackMidiEvent.time <= nextStep) {
                        MidiEvent midiEvent(jackMidiEvent.size);
                        memcpy(midiEvent.buffer, jackMidiEvent.buffer, jackMidiEvent.size * sizeof(jack_midi_data_t));
                        midiInput.append(midiEvent);
                        midiInputEventIndex++;
                    } else {
                        break;
                    }
                }
                if (activeSequencer) {
                    // leave the current step and output the corresponding midi (note off) events:
                    midiOutput.resize(0);
                    activeSequencer->processStepEnd(midiOutput);
                    for (int i = 0; i < midiOutput.size(); i++) {
                        const MidiEvent &event = midiOutput[i];
                        jack_midi_event_write(midiOutputBuffer, nextStep, event.buffer, event.size);
                    }
                }
                activeSequencer = sequencer;
                // enter the next step and output the corresponding midi (note on) events:
                midiOutput.resize(0);
                sequencer = sequencer->processStepBegin(midiInput, midiOutput);
                for (int i = 0; i < midiOutput.size(); i++) {
                    const MidiEvent &event = midiOutput[i];
                    jack_midi_event_write(midiOutputBuffer, nextStep, event.buffer, event.size);
                }
                stepPosition -= 1.0;
                nextStep = (jack_nframes_t)(framesPerStep - stepPosition * framesPerStep);
            }
            stepExpectedAtNextBufferBegin = (nextStep == nframes);
        } else if (activeSequencer) {
            // leave the current step and output the corresponding midi (note off) events:
            midiOutput.resize(0);
            activeSequencer->stop(midiOutput);
            for (int i = 0; i < midiOutput.size(); i++) {
                const MidiEvent &event = midiOutput[i];
                jack_midi_event_write(midiOutputBuffer, 0, event.buffer, event.size);
            }
            activeSequencer = 0;
        }
    }
    outboundCondition.wakeAll();
    return 0;
}

int RoundaboutThread::process(jack_nframes_t nframes, void *arg)
{
    return ((RoundaboutThread*)arg)->process(nframes);
}
