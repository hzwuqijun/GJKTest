#ifndef __OPENGL_3_H
#define __OPENGL_3_H

#include <AntTweakBar.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "mesh.h"
#include "light.h"
#include "camera.h"
#include "scene.h"

class Event;

class OpenGLContext{
public:
	OpenGLContext(Scene* scene);
	~OpenGLContext(void);
	bool create30Context(void);
	void init(int argc, char *argv[]);
	void reshapeWindow(int w, int h);
	void renderScene(void);
	void processScene(void);

    void hOnMiddleMouseDrag(const Event& event);
    void hOnRightMouseDrag(const Event& event);
    void hOnLeftMouseDrag(const Event& event);
    void hOnMouseRoll(const Event& event);
    void hOnMouseLeftClick(const Event& event);

	bool redisplay;
    Camera camera;
    Camera arrow_camera;

private:
	void drawPass(void)const;
	void drawConfiguration(void)const;
	void createGui(void);

	int windowWidth;
	int windowHeight;
	float fov, znear, zfar;

	bool rotating;
    bool check_collisions_;

	glm::vec3 m_bgColor;

	glm::mat4 projectionMatrix;
	glm::mat4 lightViewMatrix;
	glm::mat4 lightProjectionMatrix;

	CLight light;
    Scene* scene_;
    Scene* arrows_;

	Shader *sh_gbuffer; // GLSL Shader

	TwBar *bar;
};


#endif
