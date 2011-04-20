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

#include "roundaboutthread.h"
#include "roundaboutsequencer.h"

RoundaboutThread::RoundaboutThread(QObject *parent) :
    QThread(parent),
    shutdown(false),
    client(0)
{
    // connect to the jack server:
    client = jack_client_open("Roundabout", JackNullOption, 0);
    if (client) {
        // start the jack client:
        if (jack_activate(client)) {
            jack_client_close(client);
            client = 0;
        }
    }
    // start the QThread:
    if (isValid()) {
        start();
    }
}

RoundaboutThread::~RoundaboutThread()
{
    if (isValid()) {
        // close the jack client:
        jack_client_close(client);
        // send a shutdown event to the QThread:
        RoundaboutThreadOutboundEvent event;
        event.eventType = RoundaboutThreadOutboundEvent::SHUTDOWN;
        writeOutboundEvent(event);
        outboundCondition.wakeAll();
        // wait for the thread to finish:
        wait();
    }
}

bool RoundaboutThread::isValid() const
{
    return client;
}

void RoundaboutThread::processOutboundEvents()
{
    // process our events:
    OutboundEventsHelper<RoundaboutThreadOutboundEvent>::processOutboundEvents();
    // process other events:
    for (int i = 0; i < outboundEventsInterfaces.size(); i++) {
        outboundEventsInterfaces[i]->processOutboundEvents();
    }
}

void RoundaboutThread::processInboundEvents()
{
    // process our events:
    InboundEventsHelper<RoundaboutThreadInboundEvent>::processInboundEvents();
    // process other events:
    for (int i = 0; i < inboundEventsInterfaces.size(); i++) {
        inboundEventsInterfaces[i]->processInboundEvents();
    }
}

void RoundaboutThread::createSequencer()
{
    RoundaboutSequencer *sequencer = new RoundaboutSequencer(this);
    RoundaboutThreadInboundEvent event;
    event.eventType = RoundaboutThreadInboundEvent::CREATE_SEQUENCER;
    event.sequencer = sequencer;
    writeInboundEvent(event);
}

void RoundaboutThread::run()
{
    for (; !shutdown; ) {
        // wait for outbound events:
        outboundMutex.lock();
        outboundCondition.wait(&outboundMutex);
        processOutboundEvents();
    }
}

void RoundaboutThread::processInboundEvent(RoundaboutThreadInboundEvent &event)
{
    if (event.eventType == RoundaboutThreadInboundEvent::CREATE_SEQUENCER) {
        inboundEventsInterfaces.append(event.sequencer);
    }
}

void RoundaboutThread::processOutboundEvent(RoundaboutThreadOutboundEvent &event)
{
    if (event.eventType == RoundaboutThreadOutboundEvent::SHUTDOWN) {
        shutdown = true;
    }
}

void RoundaboutThread::process(jack_nframes_t nframes)
{
    processInboundEvents();
    outboundCondition.wakeAll();
}

void RoundaboutThread::process(jack_nframes_t nframes, void *arg)
{
    ((RoundaboutThread*)arg)->process(nframes);
}
