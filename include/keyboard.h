#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "EventMgr/event_manager.h"

class CKeyboard{
public:
	CKeyboard(EventManager *event_manager);
	~CKeyboard(void);
	void keyOps(void);
	void keySpOps(void);
	void keyIsDown(int key);
	void keyIsUp(int key);
	void keySpIsDown(int key);
	void keySpIsUp(int key);

    void bind_key(unsigned char key, const EventType& event_type);

private:
	bool keyMap[256];
	bool keySpMap[256];
	bool keyRepeat;

    std::map<unsigned char, EventType> keybinds_;

	EventManager* evt_manager_;
};

void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
#endif
