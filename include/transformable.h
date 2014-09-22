#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transformable{
public:
    Transformable(void);
    const glm::mat4& getModelMatrix(void);
    const glm::vec3& getPosition(void)const;
    void translate(glm::vec3 dr);
    void scale(float scale_factor);
    void rotate(float angle, glm::vec3 axis);
    void pre_rotate(float angle, glm::vec3 axis);
private:
    bool       is_changed_;
    float      scale_;
    glm::vec3  pos_;
    glm::fquat rot_;
    glm::mat4  modelMatrix_;
};

#endif
