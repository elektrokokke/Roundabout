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
#include "ringbuffer.h"

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

class RoundaboutThread : public QThread, public OutboundEventsInterface, public InboundEventsInterface
{
    Q_OBJECT
public:
    struct InboundEvent {
        enum EventType {
            CREATE_SEQUENCER
        } eventType;
    };
    struct OutboundEvent {
        enum EventType {
            CREATED_SEQUENCER,
            SHUTDOWN
        } eventType;
    };
    RoundaboutThread(QObject *parent = 0);
    virtual ~RoundaboutThread();
    bool isValid() const;
    // Reimplemented from EventsAcrossThreadBoundariesInterface:
    virtual void processOutboundEvents();
    virtual void processInboundEvents();
signals:
    void createdSequencer();
public slots:
    void createSequencer();
protected:
    // Reimplemented from QThread:
    virtual void run();
private:
    Ringbuffer<InboundEvent> inboundRingbuffer;
    Ringbuffer<OutboundEvent> outboundRingbuffer;
    bool shutdown;
    QMutex outboundMutex;
    QWaitCondition outboundCondition;
    jack_client_t *client;
    QVector<OutboundEventsInterface*> outboundEventsInterfaces;
    QVector<InboundEventsInterface*> inboundEventsInterfaces;

    // Will be called in the jack process thread:
    void process(jack_nframes_t nframes);
    static void process(jack_nframes_t nframes, void *arg);
};

#endif // ROUNDABOUTTHREAD_H
