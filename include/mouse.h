#ifndef __CMOUSE_H
#define __CMOUSE_H

#include <glm/glm.hpp>
#include "EventMgr/event_manager.h"

#define M_LEFT_BUTTON   0
#define M_MIDDLE_BUTTON 1
#define M_RIGHT_BUTTON  2

class CMouse{
public:
	CMouse(EventManager* evt_manager);
	~CMouse(void);
	
	void onMouse(int button, int state, int x, int y);
	void onMotion(int x, int y);
	void onPassiveMotion(int x, int y);

    static EventType M_MDRAG_EVENT;
    static EventType M_LDRAG_EVENT;
    static EventType M_RDRAG_EVENT;
    static EventType M_ROLLER_EVENT;
    static EventType M_LCLICK_EVENT;
    static EventType M_RCLICK_EVENT;
    static EventType M_MCLICK_EVENT;
private:
	EventManager *evt_manager_;
	int last_mx,last_my,cur_mx,cur_my;
	bool dragging;
    int mbtn_clicked;
	int windowWidth;
	int windowHeight;
};

struct MouseDragEvent: public Event{
    explicit MouseDragEvent(int button, glm::ivec2 start, glm::ivec2 end):
        button_(button),
        start_(start), end_(end)
    {}

    virtual const EventType& getEventType(void)const{
        return (button_ == M_LEFT_BUTTON)?  CMouse::M_LDRAG_EVENT:
               (button_ == M_RIGHT_BUTTON)? CMouse::M_RDRAG_EVENT: CMouse::M_MDRAG_EVENT;
    }

    int button_;
    glm::ivec2 start_;
    glm::ivec2 end_;
};

struct MouseClickEvent: public Event{
    explicit MouseClickEvent(int button, int x, int y):
        button_(button),
        x_(x), y_(y)
    {}

    virtual const EventType& getEventType(void)const{
        return (button_ == M_LEFT_BUTTON)?  CMouse::M_LCLICK_EVENT:
               (button_ == M_RIGHT_BUTTON)? CMouse::M_RCLICK_EVENT: CMouse::M_MCLICK_EVENT;
    }

    int button_;
    int x_, y_;
};

enum eMouseRoller{
    M_ROLLER_UP,
    M_ROLLER_DOWN
};

struct MouseRollerEvent: public Event{
    explicit MouseRollerEvent(eMouseRoller dir):
        dir_(dir)
    {}

    virtual const EventType& getEventType(void)const{
        return CMouse::M_ROLLER_EVENT;
    }

    eMouseRoller dir_;
};

#endif
