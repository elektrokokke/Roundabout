#ifndef ROUNDABOUTSEQUENCER_H
#define ROUNDABOUTSEQUENCER_H

#include <QObject>
#include "roundaboutthread.h"

struct RoundaboutSequencerInboundEvent {
    enum EventType {
        TOGGLE_STEP,
        TOGGLE_NOTE
    } eventType;
    int step, noteNumber;
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
    RoundaboutSequencer(QObject *parent = 0);

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
};

#endif // ROUNDABOUTSEQUENCER_H
