#ifndef BASIC_EVENT_H
#define BASIC_EVENT_H

#include "EventMgr/event.h"

class BasicEvent: public Event{
public:
    BasicEvent(const EventType& type):
        type_(type)
    {}

	const EventType& getEventType(void)const{
        return type_;
    }

private:
    const EventType type_;
};

#endif
