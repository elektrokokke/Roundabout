#include "roundaboutsequencer.h"

RoundaboutSequencer::RoundaboutSequencer(QObject *parent) :
    QObject(parent)
{
}

void RoundaboutSequencer::toggleStep(int step)
{
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_STEP;
    event.step = step;
    writeInboundEvent(event);
}

void RoundaboutSequencer::toggleNote(int step, int noteNumber)
{
    RoundaboutSequencerInboundEvent event;
    event.eventType = RoundaboutSequencerInboundEvent::TOGGLE_NOTE;
    event.step = step;
    event.noteNumber = noteNumber;
    writeInboundEvent(event);
}

void RoundaboutSequencer::processInboundEvent(RoundaboutSequencerInboundEvent &event)
{
    if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_STEP) {
    } else if (event.eventType == RoundaboutSequencerInboundEvent::TOGGLE_NOTE) {
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

