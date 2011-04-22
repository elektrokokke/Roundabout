#include "roundaboutsequencer.h"

RoundaboutSequencer::RoundaboutSequencer(QObject *parent) :
    QObject(parent),
    steps(16),
    stepsPerBeat(4),
    currentStep(-1),
    beatsPerMinute(120),
    sampleRate(44100),
    framesPerStep(sampleRate * 60.0 / (beatsPerMinute * (double)stepsPerBeat)),
    nextStepFrame(0)
{
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

