#include "include/opengl_3.h"
#include "include/keyboard.h"
#include "include/mouse.h"
#include "include/app_events.h"
#include "include/obj_parser.h"
#include <AntTweakBar.h>
#include <GL/freeglut.h>

OpenGLContext* openglContext;
CKeyboard*     keyboard;
EventManager*  event_manager;
CMouse*        mouse;
Scene*         scene;
////////////GLUT Keyboard Function Wrappers/////////////
void keyDown(unsigned char key, int x, int y){
	if(!TwEventKeyboardGLUT(key, x, y)){
		keyboard->keyIsDown(key);
		keyboard->keyOps();
	}
}

void keyUp(unsigned char key, int x, int y){
	keyboard->keyIsUp(key);
}

void specialDown(int key, int x, int y){
	if(!TwEventSpecialGLUT(key, x, y)){
		keyboard->keySpIsDown(key);
		keyboard->keySpOps();
	}
}

void specialUp(int key, int x, int y){
	keyboard->keySpIsUp(key);
}

//////////////GLUT Mouse Function Wrappers////////////////

void onMouse(int button, int state, int x, int y){
	mouse->onMouse(button,state,x,y);
}

void onMotion(int x, int y){
	mouse->onMotion(x,y);
}
void onPassiveMotion(int x, int y){
	mouse->onPassiveMotion(x, y);
}
//////////////////////////////////////////////////////////

void display(void){
	openglContext->renderScene();
}

void idle(void){
    event_manager->processQueue();
	openglContext->processScene();
	if(openglContext->redisplay){
		openglContext->redisplay = false;
		glutPostRedisplay();
	}
}

void reshape(int width, int height){
	openglContext->reshapeWindow(width,height);
}

void init(int argc, char *argv[]){
	openglContext->create30Context();

    //Setup scene (Need to run before context->init because anttweakbar gets corrupted)
    {
        CMesh tetrahedron_mesh = parse_mesh("obj/Tetrahedron.obj", "flat");
        CMesh cube_mesh        = parse_mesh("obj/cube.obj", "flat");
        CMesh full_quad_mesh   = parse_mesh("obj/full_quad.obj", "flat");

        auto full_quad   = new SceneObject(full_quad_mesh);
        auto tetrahedron = new SceneObject(tetrahedron_mesh);
        auto cube        = new SceneObject(cube_mesh);

        scene->addChild(tetrahedron);
        scene->addChild(cube);
        scene->addChild(full_quad);


        tetrahedron->translate(glm::vec3(-1.5, 3.0, -1.5));
        tetrahedron->setColor(glm::vec3(0.282, 0.239, 0.545));

        cube->translate(glm::vec3(1.5, 3.0, 1.5));
        cube->setColor(glm::vec3(0.282, 0.239, 0.545));

        full_quad->scale(8.0f);
        full_quad->rotate(90.0f, glm::vec3(1.0, 0.0, 0.0));
        full_quad->make_unselectable();
    }

	openglContext->init(argc,argv);

    event_manager->addHandler([](const Event& event){
        openglContext->hOnMiddleMouseDrag(event);
    }, CMouse::M_MDRAG_EVENT);

    event_manager->addHandler([](const Event& event){
        openglContext->hOnRightMouseDrag(event);
    }, CMouse::M_RDRAG_EVENT);

    event_manager->addHandler([](const Event& event){
        openglContext->hOnLeftMouseDrag(event);
    }, CMouse::M_LDRAG_EVENT);

    event_manager->addHandler([](const Event& event){
        openglContext->hOnMouseRoll(event);
    }, CMouse::M_ROLLER_EVENT);

    event_manager->addHandler([](const Event& event){
        openglContext->hOnMouseLeftClick(event);
    }, CMouse::M_LCLICK_EVENT);

    event_manager->addHandler([](const Event& event){
       glutLeaveMainLoop();
    }, APP_EXIT_EVENT);

    keyboard->bind_key(27, APP_EXIT_EVENT);
}

void close_func(void){
    TwTerminate();
    delete keyboard;
    delete mouse;
    delete event_manager;
    delete openglContext;
    delete scene;
}

int main(int argc,char *argv[] ){

    scene         = new Scene();
    openglContext = new OpenGLContext(scene);
    event_manager = new EventManager();
    keyboard      = new CKeyboard(event_manager);
    mouse         = new CMouse(event_manager);

	glutInit(&argc, argv);
	init(argc, argv);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	
	
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutPassiveMotionFunc(onPassiveMotion);

    glutCloseFunc(&close_func);
	
	glutMainLoop();

	return 1;
}
