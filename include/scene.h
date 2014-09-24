#ifndef SCENE_H
#define SCENE_H

#include "transformable.h"
#include "mesh.h"
#include "convex_body.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class SceneObject: public Transformable{
public:
    SceneObject(CMesh mesh, ConvexBody* body = nullptr);
    CMesh& getMesh(void);
    void setColor(const glm::vec3& color);
    const glm::vec3& getColor(void)const;
    void make_selectable(void);
    void make_unselectable(void);

    bool is_selectable_;
    bool is_selected_;
private:
    glm::vec3 color_;
    CMesh mesh_;
    ConvexBody* body_;
};

class Scene: public Transformable{
public:
    ~Scene(void);
    void addChild(SceneObject* object);
    std::vector<SceneObject*> children_;
};

#endif
