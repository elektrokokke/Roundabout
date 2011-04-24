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
    stepsPerBeat(4),
    currentStep(-1),
    beatsPerMinute(120),
    sampleRate(44100),
    framesPerStep(sampleRate * 60.0 / (beatsPerMinute * (double)stepsPerBeat)),
    nextStepFrame(0),
    steps(16)
{
    for (int i = 0; i < steps.size(); i++) {
        steps[i].active = true;
        steps[i].connection = 0;
    }
}

void RoundaboutSequencer::process(jack_nframes_t nframes, QVector<MidiEvent> &midiEventsOutput)
{
    for (; qRound(nextStepFrame) < (int)nframes; ) {
        RoundaboutSequencerOutboundEvent event;
        if (currentStep >= 0) {
            event.eventType = RoundaboutSequencerOutboundEvent::LEFT_STEP;
            event.step = currentStep;
            writeOutboundEvent(event);
            currentStep = (currentStep + 1) % steps;
        } else {
            currentStep = 0;
        }
        event.eventType = RoundaboutSequencerOutboundEvent::ENTERED_STEP;
        event.step = currentStep;
        writeOutboundEvent(event);
        nextStepFrame += framesPerStep;
    }
    nextStepFrame -= (double)nframes;
}

void RoundaboutSequencer::toggleStep(int step)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_STEP;
    event.step = step;
    writeInboundEvent(event);
}

void RoundaboutSequencer::toggleNote(int step, int noteNumber)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_NOTE;
    event.step = step;
    event.noteNumber = noteNumber;
    writeInboundEvent(event);
}

void RoundaboutSequencer::connect(int step, RoundaboutSequencer *sequencer)
{
    Q_ASSERT((step >= 0) && (step < steps.size()));
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::CONNECT_STEP;
    event.step = step;
    event.sequencer = sequencer;
    writeInboundEvent(event);
}

void RoundaboutSequencer::disconnect(int step)
{
    connect(step, 0);
}

void RoundaboutSequencer::processInboundEvent(RoundaboutSequencerInboundEvent &event)
{
    Q_ASSERT((event.step >= 0) && (event.step < steps.size()));
    if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_STEP) {
        steps[event.step].active = !steps[event.step].active;
    } else if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_NOTE) {
    } else if (event.eventType == RoundaboutSequencerInboundEvent::CONNECT_STEP) {
        steps[event.step].connection = event.sequencer;
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

