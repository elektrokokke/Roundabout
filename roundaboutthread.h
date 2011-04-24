#ifndef ROUNDABOUTTHREAD_H
#define ROUNDABOUTTHREAD_H

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

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>
#include <jack/jack.h>
#include <jack/types.h>
#include <jack/midiport.h>
#include "ringbuffer.h"

class MidiEvent {
public:
    MidiEvent() {}
    MidiEvent(jack_nframes_t time_, size_t size_) :
            time(time_),
            size(size_)
    {}
    jack_nframes_t time;
    size_t size;
    jack_midi_data_t buffer[3];
};

class MidiNoteOnEvent : public MidiEvent {
public:
    MidiNoteOnEvent(jack_nframes_t time, unsigned char channel, unsigned char noteNumber, unsigned char velocity) :
            MidiEvent(time, 3)
    {
        buffer[0] = 0x90 + channel;
        buffer[1] = noteNumber;
        buffer[2] = velocity;
    }
};

class MidiNoteOffEvent : public MidiEvent {
public:
    MidiNoteOffEvent(jack_nframes_t time, unsigned char channel, unsigned char noteNumber, unsigned char velocity) :
            MidiEvent(time, 3)
    {
        buffer[0] = 0x80 + channel;
        buffer[1] = noteNumber;
        buffer[2] = velocity;
    }
};

class InboundEventsInterface
{
public:
    /**
      "Inbound" events are events that are sent
      from other threads to the process thread.

      This function will be called from within the
      process thread.
      */
    virtual void processInboundEvents() = 0;
};

template<class T> class InboundEventsHelper : public InboundEventsInterface
{
public:
    InboundEventsHelper() : ringbuffer(4096) {}
    bool hasInboundEvents() const { return ringbuffer.readSpace(); }
    T readInboundEvent() { return ringbuffer.read(); }
    void writeInboundEvent(T &event) { ringbuffer.write(event); }
    virtual void processInboundEvents() {
        for (; hasInboundEvents(); ) {
            T event = readInboundEvent();
            processInboundEvent(event);
        }
    }
protected:
    virtual void processInboundEvent(T &event) = 0;
private:
    Ringbuffer<T> ringbuffer;
};

class OutboundEventsInterface
{
public:
    /**
      "Outbound" events are events that are sent
      from the process thread to other threads.

      This function will be called from outside the
      process thread.
      */
    virtual void processOutboundEvents() = 0;
};

template<class T> class OutboundEventsHelper : public OutboundEventsInterface
{
public:
    OutboundEventsHelper() : ringbuffer(4096) {}
    bool hasOutboundEvents() const { return ringbuffer.readSpace(); }
    T readOutboundEvent() { return ringbuffer.read(); }
    void writeOutboundEvent(T &event) { ringbuffer.write(event); }
    virtual void processOutboundEvents() {
        for (; hasOutboundEvents(); ) {
            T event = readOutboundEvent();
            processOutboundEvent(event);
        }
    }
protected:
    virtual void processOutboundEvent(T &event) = 0;
private:
    Ringbuffer<T> ringbuffer;
};

class RoundaboutSequencer;

struct RoundaboutThreadInboundEvent {
    enum EventType {
        CREATE_SEQUENCER
    } eventType;
    RoundaboutSequencer *sequencer;
};
struct RoundaboutThreadOutboundEvent {
    enum EventType {
        CREATED_SEQUENCER,
        SHUTDOWN
    } eventType;
    RoundaboutSequencer *sequencer;
};

class RoundaboutThread : public QThread, public InboundEventsHelper<RoundaboutThreadInboundEvent>, public OutboundEventsHelper<RoundaboutThreadOutboundEvent>
{
    Q_OBJECT
public:
    RoundaboutThread(QObject *parent = 0);
    virtual ~RoundaboutThread();
    bool isValid() const;
    virtual void processInboundEvents();
    virtual void processOutboundEvents();
signals:
    void createdSequencer(RoundaboutSequencer *sequencer);
public slots:
    void createSequencer();
protected:
    // Reimplemented from QThread:
    virtual void run();
    // Reimplemented from InboundEventsHelper:
    virtual void processInboundEvent(RoundaboutThreadInboundEvent &event);
    // Reimplemented from OutboundEventsHelper:
    virtual void processOutboundEvent(RoundaboutThreadOutboundEvent &event);
private:
    bool shutdown;
    QMutex outboundMutex;
    QWaitCondition outboundCondition;
    jack_client_t *client;
    jack_port_t *midiInputPort, *midiOutputPort, *audioOutputPort;
    jack_nframes_t sampleRate;
    QVector<OutboundEventsInterface*> outboundEventsInterfaces;
    QVector<InboundEventsInterface*> inboundEventsInterfaces;
    RoundaboutSequencer *sequencer;
    QVector<MidiEvent> midiEventsInput, midiEventsOutput;

    // Will be called in the jack process thread:
    int process(jack_nframes_t nframes);
    static int process(jack_nframes_t nframes, void *arg);
};

#endif // ROUNDABOUTTHREAD_H
