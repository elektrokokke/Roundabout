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
#include <QDebug>

RoundaboutSequencer::RoundaboutSequencer(QObject *parent) :
    QObject(parent),
    inputChannel(0),
    outputChannel(0),
    baseNoteNumber(48),
    activeBaseNoteNumber(48),
    activeNotes(13),
    stepsPerBeat(4),
    nextStep(0),
    activeStep(0),
    steps(16)
{
    for (int i = 0; i < steps.size(); i++) {
        steps[i].active = true;
        steps[i].activeNotes = activeNotes;
        steps[i].connection = 0;
    }
}

void RoundaboutSequencer::processChangeInputChannel(unsigned char channel)
{
    qDebug() << "input channel" << channel;
    inputChannel = channel;
}

void RoundaboutSequencer::processChangeOutputChannel(unsigned char channel)
{
    qDebug() << "output channel" << channel;
    outputChannel = channel;
}

void RoundaboutSequencer::setNextStep(int step)
{
    nextStep = step;
}

RoundaboutSequencer * RoundaboutSequencer::processStepBegin(QVector<MidiEvent> &output)
{
    // determine current step:
    activeStep = nextStep;
    activeNotes = steps[activeStep].activeNotes;
    activeBaseNoteNumber = baseNoteNumber;
    // send step entered event:
    RoundaboutSequencerOutboundEvent event;
    event.eventType = RoundaboutSequencerOutboundEvent::ENTERED_STEP;
    event.step = nextStep;
    writeOutboundEvent(event);
    // create midi note on events:
    for (int note = 0; note < activeNotes.size(); note++) {
        if (activeNotes[note]) {
            output.append(MidiNoteOnEvent(outputChannel, qBound(0, activeBaseNoteNumber + note, 127), 127));
        }
    }
    // determine next step (maybe in another roundabout):
    RoundaboutSequencer *nextSequencer = this;
    if (steps[nextStep].connection) {
        Step &step = steps[nextStep];
        step.connection->setNextStep(step.connectedStep);
        nextSequencer = step.connection;
    } else {
        nextStep = (nextStep + 1) % steps.size();
    }
    // return next roundabout:
    return nextSequencer;
}

void RoundaboutSequencer::processStepEnd(QVector<MidiEvent> &output)
{
    // send step left event:
    RoundaboutSequencerOutboundEvent event;
    event.eventType = RoundaboutSequencerOutboundEvent::LEFT_STEP;
    event.step = activeStep;
    writeOutboundEvent(event);
    // create midi note off events:
    for (int note = 0; note < activeNotes.size(); note++) {
        if (activeNotes[note]) {
            output.append(MidiNoteOffEvent(outputChannel, qBound(0, activeBaseNoteNumber + note, 127), 127));
        }
    }
}

void RoundaboutSequencer::stop(QVector<MidiEvent> &output)
{
    processStepEnd(output);
    setNextStep(0);
}

void RoundaboutSequencer::processMidiEvents(const QVector<MidiEvent> &input)
{
    for (int i = 0; i < input.size(); i++) {
        // interpret midi note on events to set the base note number:
        const MidiEvent &event = input[i];
        if (((event.buffer[0] & 0x0F) == inputChannel) && ((event.buffer[0] & 0xF0) == 0x90)) {
            baseNoteNumber = event.buffer[1];
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
        Q_ASSERT((event.note >= 0) && (event.note < steps[event.step].activeNotes.size()));
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

