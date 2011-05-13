#ifndef ROUNDABOUTSEQUENCER_H
#define ROUNDABOUTSEQUENCER_H

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

#include <QObject>
#include <QBitArray>
#include "roundaboutthread.h"

class RoundaboutSequencer;

struct RoundaboutSequencerInboundEvent {
    enum EventType {
        TOGGLE_STEP,
        TOGGLE_NOTE,
        CONNECT_STEP,
        CHANGE_STEP_BRANCH_FREQUENCY
    } eventType;
    int step, note, connectedStep, branchFrequency, continueFrequency;
    RoundaboutSequencer *sequencer;
};
struct RoundaboutSequencerOutboundEvent {
    enum EventType {
        ENTERED_STEP,
        LEFT_STEP,
        CHANGED_BRANCH_COUNTER
    } eventType;
    int step, branchCounter;
};

class RoundaboutSequencer : public QObject, public InboundEventsHelper<RoundaboutSequencerInboundEvent>, public OutboundEventsHelper<RoundaboutSequencerOutboundEvent>
{
    Q_OBJECT
public:
    class Step {
    public:
        Step() :
            active(true),
            activeNotes(13),
            connection(0),
            connectedStep(0),
            branchFrequency(1),
            continueFrequency(0),
            branchCounter(0)
        {}
        bool active;
        QBitArray activeNotes;
        RoundaboutSequencer *connection;
        int connectedStep;
        int branchFrequency, continueFrequency, branchCounter;
    };
    RoundaboutSequencer(QObject *parent = 0);

    void processChangeInputChannel(unsigned char channel);
    void processChangeOutputChannel(unsigned char channel);
    void setNextStep(int step);

    virtual RoundaboutSequencer * processStepBegin(QVector<MidiEvent> &output);
    virtual void processStepEnd(QVector<MidiEvent> &output);
    void processStop(QVector<MidiEvent> &output);
    virtual void processMidiEvents(const QVector<MidiEvent> &input);
protected:
    // Reimplemented from InboundEventsHelper:
    virtual void processInboundEvent(RoundaboutSequencerInboundEvent &event);
    // Reimplemented from OutboundEventsHelper:
    virtual void processOutboundEvent(RoundaboutSequencerOutboundEvent &event);
signals:
    void enteredStep(int step);
    void leftStep(int step);
    void changedBranchCounter(int step, int branchCounter);
public slots:
    void toggleStep(int step);
    void toggleNote(int step, int note);
    void connect(int step, RoundaboutSequencer *sequencer, int connectedStep);
    void disconnect(int step);
    void setStepBranchFrequency(int step, int branchFrequency, int continueFrequency);
private:
    unsigned char inputChannel, outputChannel, baseNoteNumber, activeBaseNoteNumber;
    QBitArray activeNotes;
    int stepsPerBeat, nextStep, activeStep;
    QVector<Step> steps;
};

#endif // ROUNDABOUTSEQUENCER_H
