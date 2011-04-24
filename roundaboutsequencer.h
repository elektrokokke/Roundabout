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
#include "roundaboutthread.h"

class RoundaboutSequencer;

struct RoundaboutSequencerInboundEvent {
    enum EventType {
        TOGGLE_STEP,
        TOGGLE_NOTE,
        CONNECT_STEP,
    } eventType;
    int step, noteNumber, connectedStep;
    RoundaboutSequencer *sequencer;
};
struct RoundaboutSequencerOutboundEvent {
    enum EventType {
        ENTERED_STEP,
        LEFT_STEP
    } eventType;
    int step;
};

class RoundaboutSequencer : public QObject, public InboundEventsHelper<RoundaboutSequencerInboundEvent>, public OutboundEventsHelper<RoundaboutSequencerOutboundEvent>
{
    Q_OBJECT
public:
    struct Step {
        bool active;
        RoundaboutSequencer *connection;
        int connectedStep;
    };
    RoundaboutSequencer(QObject *parent = 0);

    void setNextStep(int step);
    virtual RoundaboutSequencer * move(jack_nframes_t nframes, jack_nframes_t time, QVector<MidiEvent> &midiEventsOutput);
protected:
    // Reimplemented from InboundEventsHelper:
    virtual void processInboundEvent(RoundaboutSequencerInboundEvent &event);
    // Reimplemented from OutboundEventsHelper:
    virtual void processOutboundEvent(RoundaboutSequencerOutboundEvent &event);
signals:
    void enteredStep(int step);
    void leftStep(int step);
public slots:
    void toggleStep(int step);
    void toggleNote(int step, int noteNumber);
    void connect(int step, RoundaboutSequencer *sequencer, int connectedStep);
    void disconnect(int step);
private:
    int stepsPerBeat, currentStep, nextStep;
    double beatsPerMinute, sampleRate;
    jack_nframes_t framesPerStep, framesTillNextStep;
    QVector<Step> steps;
};

#endif // ROUNDABOUTSEQUENCER_H
