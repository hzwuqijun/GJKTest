#include "include/opengl_3.h"
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "include/obj_parser.h"
#include "include/main.h"
#include "include/mouse.h"
#include "include/collision.h"

OpenGLContext::OpenGLContext(Scene* scene):
    redisplay(false),
    camera(1.0f, glm::vec3(0.0f, 8.0f, 25.0f), glm::vec3(0.0)),
    arrow_camera(1.0f, glm::vec3(0.0f, 8.0f, 25.0f), glm::vec3(0.0)),
    fov(45.0f), znear(1.0f), zfar (100.0f),
    rotating(false),
    m_bgColor(0.4, 0.6, 0.9),
    light(glm::vec3(-10.0, 10.0, 10.0), glm::vec3(1.0, -1.0, -1.0)),
    scene_(scene)
{}

OpenGLContext::~OpenGLContext(void) {
    delete sh_gbuffer;
    delete arrows_;
}

bool OpenGLContext::create30Context(void){
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(600, 600);
    windowWidth = windowHeight = 600;
    glutCreateWindow("partViewer GLSL");

    glewExperimental = GL_TRUE;
    GLenum error = glewInit(); //Enable GLEW
    if(error != GLEW_OK) return false; //Failure!
    glError(__FILE__, __LINE__);

    int glVersion[2] = {-1,1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    printf("Using OpenGL: %u.%u\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s.\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s.\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return true;
}

void OpenGLContext::createGui(void){
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(windowWidth, windowHeight);
    bar = TwNewBar("Parameters");

    TwDefine("Parameters position='0 0' size='200 250' iconified=true");

    TwAddVarRW(bar, "Rotation", TW_TYPE_BOOLCPP, &rotating, "");

    TwAddVarCB(bar, "Specular", TW_TYPE_FLOAT, CLight::SetSpecIntCallback, CLight::GetSpecIntCallback, &light,"\
        min=0.0 max=2.0 step=0.01 group=Light");

    TwAddVarCB(bar, "Diffuse", TW_TYPE_FLOAT, CLight::SetDiffIntCallback, CLight::GetDiffIntCallback, &light,"\
        min=0.0 max=2.0 step=0.01 group=Light");

    TwAddVarCB(bar, "Ambient", TW_TYPE_FLOAT, CLight::SetAmbIntCallback, CLight::GetAmbIntCallback, &light,"\
        min=0.0 max=2.0 step=0.01 group=Light");

    TwAddVarCB(bar, "Intensity", TW_TYPE_FLOAT, CLight::SetIntCallback, CLight::GetIntCallback, &light,"\
        min=0.0 max=2.0 step=0.01 group=Light");

    // float color[3] = {1.0, 1.0, 1.0};
    //TwAddVarRW(bar, "Particle Color", TW_TYPE_COLOR3F, &diffcolor," colormode=hls ");
    //TwAddVarRW(bar, "Background Color", TW_TYPE_COLOR3F, &m_bgColor," colormode=hls ");
}

void OpenGLContext::init(int argc, char *argv[]){

    createGui();

    glClearColor(m_bgColor[0], m_bgColor[1], m_bgColor[2], 1.0);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    sh_gbuffer = new Shader("shaders/gbuffer.vert", "shaders/gbuffer.frag");

    if(!light.Init(sh_gbuffer->id())) printf("Cannot bind light uniform.\n");

	camera.setProjectionParams((float) windowWidth / windowHeight, fov, znear, zfar);
	arrow_camera.setProjectionParams(1.0f, fov, znear, zfar);

    arrows_ = new Scene();
    CMesh arrow_mesh = parse_mesh("obj/arrow.obj", "flat");

    auto arrow1 = new SceneObject(arrow_mesh);
    auto arrow2 = new SceneObject(arrow_mesh);
    auto arrow3 = new SceneObject(arrow_mesh);

    arrows_->addChild(arrow1);
    arrows_->addChild(arrow2);
    arrows_->addChild(arrow3);

    arrow1->scale(3.0f);
    arrow2->scale(3.0f);
    arrow3->scale(3.0f);

    arrow2->rotate(-90.0f, glm::vec3(1.0, 0.0, 0.0));
    arrow3->rotate(-90.0f, glm::vec3(0.0, 0.0, 1.0));

    arrow1->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    arrow2->setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    arrow3->setColor(glm::vec3(0.0f, 0.0f, 1.0f));
}

void OpenGLContext::reshapeWindow(int w, int h){
    windowWidth = w;
    windowHeight = h;
    TwWindowSize(w, h);
    glViewport(0, 0, windowWidth, windowHeight);
	camera.setProjectionParams((float) windowWidth / windowHeight, fov, znear, zfar);
}

void OpenGLContext::processScene(void){
    static float last_time = 0.0;
    float this_time = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
    // if(this_time-last_time > 1.0f/61.0f){
    if(rotating){
        scene_->pre_rotate((this_time-last_time) * 30.0f, glm::vec3(0.0, 1.0, 0.0));
    }
    camera.update();
    redisplay = true;
    last_time = this_time;
    // }
}

void OpenGLContext::drawConfiguration(void)const{

    glm::mat4 pMatrix = camera.getProjectionMatrix();
    glm::mat4 ModelViewMatrix = camera.getViewMatrix() * scene_->getModelMatrix();

    sh_gbuffer->setUniform("scale", 1.0f);

    glDisable(GL_CULL_FACE);
    for(auto object: scene_->children_){
        sh_gbuffer->setUniform("diffColor", 1, object->getColor());
        if(object->is_selected_) sh_gbuffer->setUniform("diffColor", 1, glm::vec3(1.0f, 1.0f, 0.0f));
        glm::mat4 tempModelViewMatrix = ModelViewMatrix * object->getModelMatrix();
        glm::mat3 tempNormalMatrix    = glm::mat3(glm::transpose(glm::inverse(tempModelViewMatrix)));
        glm::mat4 tempMVPMatrix       = pMatrix * tempModelViewMatrix;

        sh_gbuffer->setUniform("NormalMatrix", 1, tempNormalMatrix);
        sh_gbuffer->setUniform("ModelViewMatrix", 1, tempModelViewMatrix);
        sh_gbuffer->setUniform("MVPMatrix", 1, tempMVPMatrix);

        object->getMesh().draw();
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    ModelViewMatrix = arrow_camera.getViewMatrix() * scene_->getModelMatrix();
    pMatrix = arrow_camera.getProjectionMatrix();
    auto min_size = std::min(windowWidth, windowHeight);
    glViewport(-min_size / 30, -min_size / 30, min_size / 5, min_size / 5);
    for(auto object: arrows_->children_){
        sh_gbuffer->setUniform("diffColor", 1, object->getColor());
        if(object->is_selected_) sh_gbuffer->setUniform("diffColor", 1, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 tempModelViewMatrix = ModelViewMatrix * object->getModelMatrix();
        glm::mat3 tempNormalMatrix    = glm::mat3(glm::transpose(glm::inverse(tempModelViewMatrix)));
        glm::mat4 tempMVPMatrix       = pMatrix * tempModelViewMatrix;

        sh_gbuffer->setUniform("NormalMatrix", 1, tempNormalMatrix);
        sh_gbuffer->setUniform("ModelViewMatrix", 1, tempModelViewMatrix);
        sh_gbuffer->setUniform("MVPMatrix", 1, tempMVPMatrix);

        object->getMesh().draw();
    }
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_CULL_FACE);
}


void OpenGLContext::drawPass(void)const{

    glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sh_gbuffer->bind();
    {
        light.uploadDirection(camera.getViewMatrix());
        drawConfiguration();
    }
    sh_gbuffer->unbind();

    glEnable(GL_BLEND);
}

void OpenGLContext::renderScene(void){
    drawPass();
    TwDraw();
    glutSwapBuffers();
}

void OpenGLContext::hOnMiddleMouseDrag(const Event& event){
    auto drag_event = static_cast<const MouseDragEvent&>(event);
    int dx = drag_event.end_.x - drag_event.start_.x;
    int dy = drag_event.end_.y - drag_event.start_.y;
    auto y_axis = glm::vec3(0.0, 1.0, 0.0);
    if(glm::dot(glm::mat3(scene_->getModelMatrix()) * y_axis, y_axis) < 0.0) y_axis = -y_axis;
    scene_->pre_rotate(0.5f * dx, y_axis);
    scene_->rotate(0.5f * dy, glm::vec3(1.0, 0.0, 0.0));
}

void OpenGLContext::hOnMouseRoll(const Event& event){
    auto roll_event = static_cast<const MouseRollerEvent&>(event);
    auto dir_vec = glm::normalize(camera.getPosition());
    camera.translate((1.0f - 2.0f * roll_event.dir_) * dir_vec);
}

void OpenGLContext::hOnMouseLeftClick(const Event& event){
    auto click_event = static_cast<const MouseClickEvent&>(event);
    auto start = glm::unProject(glm::vec3(click_event.x_, windowHeight - click_event.y_, 0.0f),
                                camera.getViewMatrix(), camera.getProjectionMatrix(),
                                glm::vec4(0.0, 0.0, windowWidth, windowHeight));
    auto end   = glm::unProject(glm::vec3(click_event.x_, windowHeight - click_event.y_, 1.0f),
                                camera.getViewMatrix(), camera.getProjectionMatrix(),
                                glm::vec4(0.0, 0.0, windowWidth, windowHeight));

    auto ray_dir = glm::normalize(end - start);

    float t = 100000.0f;
    SceneObject* selected_object = nullptr;
    for(auto object: scene_->children_){
        auto modelMatrix = scene_->getModelMatrix() * object->getModelMatrix();
        if(object->is_selectable_){
            if(object->is_selected_) object->is_selected_ = false;
            else if(object->getMesh().intersect(modelMatrix, start, ray_dir, t)){
                selected_object = object;
            }
        }
    }
    if(selected_object){
        selected_object->is_selected_ = true;
    }
    else{
        for(auto object: scene_->children_){
            if(object->is_selectable_) object->is_selected_ = false;
        }
    }
}

namespace{
    glm::vec3 getArcballVec3f(const glm::vec2& r){
        auto P = glm::vec3(r, 0.0);
        float temp_square = glm::dot(P, P);
        if(temp_square <= 0.5f) P.z = sqrt(1.0f - temp_square);
        else{
            P.z = 0.5f / sqrt(temp_square);
            P = glm::normalize(P);
        }
        return P;
    }
};

void OpenGLContext::hOnLeftMouseDrag(const Event& event){
    for(auto object: scene_->children_){
        if(object->is_selected_){
            auto drag_event = static_cast<const MouseDragEvent&>(event);
            glm::vec2 pos = glm::vec2(glm::project(object->getPosition(),
                                      camera.getViewMatrix() * scene_->getModelMatrix(),
                                      camera.getProjectionMatrix(),
                                      glm::vec4(0.0, 0.0, windowWidth, windowHeight)));
            pos.y = windowHeight - pos.y;

            int screen_min = windowHeight;
            if(windowWidth < windowHeight) screen_min = windowWidth;

            glm::vec2 start = 2.0f * (glm::vec2(drag_event.start_) - pos) / float(screen_min);
            glm::vec2 end   = 2.0f * (glm::vec2(drag_event.end_) - pos) / float(screen_min);
            start.y = -start.y;
            end.y   = -end.y;

            auto a = getArcballVec3f(start);
            auto b = getArcballVec3f(end);
            auto c = glm::cross(a, b);
            if(glm::length(c) < 1e-5f) break;
            c = glm::normalize(glm::inverse(glm::mat3(scene_->getModelMatrix())) * c);

            float angle = 2.0f * glm::degrees(std::acos(std::min(1.0f, glm::dot(a, b))));

            object->rotate(angle, c);

            break;
        }
    }
}

void OpenGLContext::hOnRightMouseDrag(const Event& event){
    auto drag_event = static_cast<const MouseDragEvent&>(event);
    int dx = drag_event.end_.x - drag_event.start_.x;
    int dy = drag_event.end_.y - drag_event.start_.y;

    for(auto object: scene_->children_){
        if(object->is_selected_){
            auto modelViewMatrix = camera.getViewMatrix() * scene_->getModelMatrix();
            auto transform       = glm::mat3(glm::inverse(modelViewMatrix));

            auto right = glm::column(transform, 0); //transform * (1 0 0)
            auto up    = glm::column(transform, 1); //transform * (0 1 0)

            auto dist = -(modelViewMatrix * glm::vec4(object->getPosition(), 1.0f)).z;

            float factor = 2.0f * tanf(0.5f * fov * M_PI / 180.0f);

            object->translate( ((factor * dx) / windowHeight) * (dist / znear) * right); //also divide by height due to aspect
            object->translate(-((factor * dy) / windowHeight) * (dist / znear) * up);

            if(find_collisions(*scene_)){
                printf("Found a collision\n");
            }

            break;
        }
    }
}
