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

#include "roundaboutsequencer.h"

RoundaboutSequencer::RoundaboutSequencer(QObject *parent) :
    QObject(parent),
    inputChannel(0),
    outputChannel(0),
    baseNoteNumber(48),
    activeBaseNoteNumber(48),
    activeNotes(13),
    stepsPerBeat(4),
    currentStep(-1),
    nextStep(0),
    beatsPerMinute(120),
    sampleRate(44100),
    framesPerStep(qRound(sampleRate * 60.0 / (beatsPerMinute * (double)stepsPerBeat))),
    framesTillNextStep(0),
    steps(16)
{
    for (int i = 0; i < steps.size(); i++) {
        steps[i].active = true;
        steps[i].activeNotes = activeNotes;
        steps[i].connection = 0;
    }
}

void RoundaboutSequencer::setNextStep(int step)
{
    nextStep = step;
    framesTillNextStep = 0;
}

RoundaboutSequencer * RoundaboutSequencer::move(jack_nframes_t nframes, jack_nframes_t time, const QVector<MidiEvent> &midiEventsInput, QVector<MidiEvent> &midiEventsOutput)
{
    jack_nframes_t timeFrom = time;
    for (; framesTillNextStep < (int)nframes; ) {
        time += framesTillNextStep;
        // process all input midi events between timeFrom and time:
        processMidiEvents(timeFrom, time, midiEventsInput);
        timeFrom = time;
        nframes -= framesTillNextStep;
        RoundaboutSequencerOutboundEvent event;
        if (currentStep >= 0) {
            event.eventType = RoundaboutSequencerOutboundEvent::LEFT_STEP;
            event.step = currentStep;
            writeOutboundEvent(event);
            // create midi note off event:
            for (int note = 0; note < activeNotes.size(); note++) {
                if (activeNotes[note]) {
                    midiEventsOutput.append(MidiNoteOffEvent(time, outputChannel, qBound(0, activeBaseNoteNumber + note, 127), 127));
                }
            }
            if (steps[currentStep].connection) {
                Step &step = steps[currentStep];
                currentStep = -1;
                step.connection->setNextStep(step.connectedStep);
                return step.connection->move(nframes, time, midiEventsInput, midiEventsOutput);
            }
        }
        currentStep = nextStep;
        nextStep = (nextStep + 1) % steps.size();
        event.eventType = RoundaboutSequencerOutboundEvent::ENTERED_STEP;
        event.step = currentStep;
        writeOutboundEvent(event);
        // create midi note on event:
        activeNotes = steps[currentStep].activeNotes;
        activeBaseNoteNumber = baseNoteNumber;
        for (int note = 0; note < activeNotes.size(); note++) {
            if (activeNotes[note]) {
                midiEventsOutput.append(MidiNoteOnEvent(time, outputChannel, qBound(0, activeBaseNoteNumber + note, 127), 127));
            }
        }
        framesTillNextStep += framesPerStep;
    }
    time += nframes;
    // process all input midi events between timeFrom and time:
    processMidiEvents(timeFrom, time, midiEventsInput);
    framesTillNextStep -= nframes;
    return this;
}

void RoundaboutSequencer::processMidiEvents(jack_nframes_t start, jack_nframes_t end, const QVector<MidiEvent> &midiEventsInput)
{
    for (int i = 0; (i < midiEventsInput.size()) && (midiEventsInput[i].time < end); i++) {
        if (midiEventsInput[i].time >= start) {
            // interpret midi note on events to set the base note number:
            const MidiEvent &event = midiEventsInput[i];
            if (((event.buffer[0] & 0x0F) == inputChannel) && ((event.buffer[0] & 0xF0) == 0x90)) {
                baseNoteNumber = event.buffer[1];
            }
        }
    }
}

void RoundaboutSequencer::toggleStep(int step)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_STEP;
    event.step = step;
    writeInboundEvent(event);
}

void RoundaboutSequencer::toggleNote(int step, int note)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_NOTE;
    event.step = step;
    event.note = note;
    writeInboundEvent(event);
}

void RoundaboutSequencer::connect(int step, RoundaboutSequencer *sequencer, int connectedStep)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    Q_ASSERT(!sequencer || ((connectedStep >= 0) && (connectedStep < sequencer->steps.size())));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::CONNECT_STEP;
    event.step = step;
    event.sequencer = sequencer;
    event.connectedStep = connectedStep;
    writeInboundEvent(event);
}

void RoundaboutSequencer::disconnect(int step)
{
    connect(step, 0, 0);
}

void RoundaboutSequencer::processInboundEvent(RoundaboutSequencerInboundEvent &event)
{
    Q_ASSERT((event.step >= 0) && (event.step < steps.size()));
    if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_STEP) {
        steps[event.step].active = !steps[event.step].active;
    } else if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_NOTE) {
        Q_ASSERT((event.note >= 0) && (event.note < activeNotes.size()));
        steps[event.step].activeNotes.toggleBit(event.note);
    } else if (event.eventType == RoundaboutSequencerInboundEvent::CONNECT_STEP) {
        steps[event.step].connection = event.sequencer;
        steps[event.step].connectedStep = event.connectedStep;
    }
}

void RoundaboutSequencer::processOutboundEvent(RoundaboutSequencerOutboundEvent &event)
{
    if (event.eventType == RoundaboutSequencerOutboundEvent::ENTERED_STEP) {
        enteredStep(event.step);
    } else if (event.eventType == RoundaboutSequencerOutboundEvent::LEFT_STEP) {
        leftStep(event.step);
    }
}

