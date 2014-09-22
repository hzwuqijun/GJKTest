#include "include/transformable.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Transformable::Transformable(void):
    is_changed_(true),
    scale_(1.0f),
    pos_(glm::vec3(0.0f)), rot_(1.0f, glm::vec3(0.0f))
{}

const glm::mat4& Transformable::getModelMatrix(void){
    if(is_changed_){
        modelMatrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rot_) * glm::scale(glm::mat4(1.0f), glm::vec3(scale_));
        is_changed_  = false;
    }
    return modelMatrix_;
}

const glm::vec3& Transformable::getPosition(void)const{
    return pos_;
}

void Transformable::translate(glm::vec3 dr){
    pos_ += dr;
    is_changed_ = true;
}

void Transformable::scale(float scale_factor){
    scale_ *= scale_factor;
    is_changed_ = true;
}

void Transformable::rotate(float angle, glm::vec3 axis){
    rot_ = glm::angleAxis(angle, axis) * rot_;
    is_changed_ = true;
}

void Transformable::pre_rotate(float angle, glm::vec3 axis){
    rot_ = glm::rotate(rot_, angle, axis);
    is_changed_ = true;
}

