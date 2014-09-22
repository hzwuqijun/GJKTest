#include "include/mouse.h"
#include <GL/freeglut.h>
#include <AntTweakBar.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

EventType CMouse::M_MDRAG_EVENT("M_MDRAG_EVENT");
EventType CMouse::M_LDRAG_EVENT("M_LDRAG_EVENT");
EventType CMouse::M_RDRAG_EVENT("M_RDRAG_EVENT");
EventType CMouse::M_ROLLER_EVENT("M_ROLLER_EVENT");
EventType CMouse::M_RCLICK_EVENT("M_RCLICK_EVENT");
EventType CMouse::M_LCLICK_EVENT("M_LCLICK_EVENT");
EventType CMouse::M_MCLICK_EVENT("M_MCLICK_EVENT");

CMouse::CMouse(EventManager *evt_manager):
    evt_manager_(evt_manager),
	last_mx(0), last_my(0), cur_mx(0), cur_my(0),
	dragging(false)
{}

CMouse::~CMouse(void){
}

void CMouse::onMouse(int button, int state, int x, int y){
	if(!TwEventMouseButtonGLUT(button, state, x, y)){
		if(button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON || button == GLUT_MIDDLE_BUTTON){
			if(state==GLUT_DOWN){
                mbtn_clicked = button;
				dragging = true;
				cur_mx  = x;
				last_mx = x;
				cur_my  = y;
				last_my = y;
			}
			else if(dragging){
				dragging = false;
				if(last_mx == x && last_my == y){
				//Regular Clicking goes here//
                    evt_manager_->queueEvent(new MouseClickEvent(button, x, y));
				}
				last_mx = cur_mx;
				last_my = cur_my;
			}
		}
		if(button == 3){
            evt_manager_->queueEvent(new MouseRollerEvent( M_ROLLER_DOWN ));
		}
        else if(button == 4){
            evt_manager_->queueEvent(new MouseRollerEvent( M_ROLLER_UP ));
        }
	}
}

void CMouse::onMotion(int x, int y){
	if(!TwEventMouseMotionGLUT(x, y)){
		if(dragging){
            if(last_mx != x || last_my != y){
				last_mx = cur_mx;
				cur_mx  = x;
				last_my = cur_my;
				cur_my  = y;
                evt_manager_->queueEvent(new MouseDragEvent(mbtn_clicked, glm::ivec2(last_mx, last_my), glm::ivec2(cur_mx, cur_my) ));
            }
		}
	}
}

void CMouse::onPassiveMotion(int x, int y){
	if(!TwEventMouseMotionGLUT(x, y)){
	}
}
