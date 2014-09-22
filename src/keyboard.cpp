#include "include/keyboard.h"
#include "include/basic_event.h"
#include <GL/freeglut.h>

CKeyboard::CKeyboard(EventManager *event_manager):
    keyMap{false}, keySpMap{false},
    evt_manager_(event_manager)
{
	keyRepeat = false;
}

CKeyboard::~CKeyboard(void){
}

void CKeyboard::keyIsDown(int key){
	keyMap[key]=true;
}

void CKeyboard::keyIsUp(int key){
	keyMap[key]=false;
}

void CKeyboard::keySpIsDown(int key){
	keySpMap[key]=true;
}

void CKeyboard::keySpIsUp(int key){
	keySpMap[key]=false;
}

void CKeyboard::bind_key(unsigned char key, const EventType& event_type){
    auto found = keybinds_.find(key);
    if(found != keybinds_.end()){
        found->second = event_type;
    }
    else keybinds_.insert(decltype(keybinds_)::value_type(key, event_type));
}

void CKeyboard::keyOps(void){
    for(auto pair: keybinds_){
        if(keyMap[pair.first]) evt_manager_->queueEvent(new BasicEvent(pair.second));
    }
	//if(keyMap[13]){ //Enter
	//	int mod = glutGetModifiers();
	//	if(mod==GLUT_ACTIVE_ALT){
	//		if(fullscreen){
	//			glutReshapeWindow(600,600);
	//			glutSetWindowTitle("Project");
	//		}
	//		else{
	//			glutFullScreen();
	//			glutSetWindowTitle("Project Fullscreen");
	//		}
	//		fullscreen=!fullscreen;
	//	}
	//}
}

void CKeyboard::keySpOps(void){
	if(keySpMap[GLUT_KEY_F1]){
		if(!keyRepeat) printf("Key Repeat Enabled\n");
		else printf("Key Repeat Disabled\n");
		keyRepeat=!keyRepeat;
		glutSetKeyRepeat(keyRepeat);
	}
}
