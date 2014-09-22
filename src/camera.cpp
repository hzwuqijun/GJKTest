#include "include/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Camera::Camera(float aspect, const glm::vec3& pos, glm::vec3 look_at, eProjection mode):
	projection_mode_(mode),
	isProjectionChanged_(false),
	isViewChanged_(false),
	pos_(pos),
	zn_(1.0f),
	zf_(100.0f),
	fov_(45.0f),
    aspect_(aspect),
	ortho_height_(10.0f)
{
    auto forward = glm::normalize(look_at - pos_);
    auto up      = glm::vec3(0.0, 1.0, 0.0);
    auto left    = glm::normalize(glm::cross(up, forward));
    auto up_new  = glm::normalize(glm::cross(forward, left));
    float angle  = acos(glm::dot(up, up_new)) * 180.0f / M_PI;

    rotation_ = glm::angleAxis(angle, left);

	view_matrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rotation_);
	view_matrix_ = glm::inverse(view_matrix_);

	if(mode == Perspective){
		projection_matrix_ = glm::perspective(fov_, aspect_, zn_, zf_);
	}
	else{
		projection_matrix_ = glm::ortho(-aspect_ * ortho_height_ * 0.5f, aspect_ * ortho_height_ * 0.5f,
                                        -ortho_height_ * 0.5f, ortho_height_ * 0.5f, zn_, zf_);
	}
}

Camera::~Camera(void){
}

void Camera::update(void){
    if(isViewChanged_){
        view_matrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rotation_);
        view_matrix_ = glm::inverse(view_matrix_);
        isViewChanged_ = false;
    }

	if(isProjectionChanged_){
		if(projection_mode_ == Perspective){
			projection_matrix_ = glm::perspective(fov_, aspect_, zn_, zf_);
		}
		else{
			projection_matrix_ = glm::ortho(-aspect_ * ortho_height_ * 0.5f,
                                             aspect_ * ortho_height_ * 0.5f,
                                             -ortho_height_ * 0.5f, ortho_height_ * 0.5f, zn_, zf_);
		}
		isProjectionChanged_ = false;
	}
}
	
void Camera::rotate(float angle, const glm::vec3& axis){
	rotation_ = glm::rotate(rotation_, angle, axis);
    isViewChanged_ = true;
}

void Camera::setRotation(float angle, const glm::vec3& axis){
	rotation_ = glm::rotate(rotation_, angle, axis);
    isViewChanged_ = true;
}

void Camera::setPosition(const glm::vec3& pos){
	this->pos_ = pos;
    isViewChanged_ = true;
}

void Camera::translate(const glm::vec3& trans){
	pos_ += trans;
    isViewChanged_ = true;
}

const glm::mat4& Camera::getViewMatrix(void)const{
	return view_matrix_;
}

const glm::vec3& Camera::getPosition(void)const{
    return pos_;
}

void Camera::setProjectionParams(float aspect, float fov_ortho_h, float zn, float zf){
	aspect_ = aspect;
	if(zn > 0.0f) zn_ = zn;
	if(zf > 0.0f) zf_ = zf;
	if(fov_ortho_h > 0.0f){
		if(projection_mode_ == Perspective) fov_ = fov_ortho_h;
		else ortho_height_ = fov_ortho_h;
	}
	isProjectionChanged_ = true;
}

void Camera::setProjectionMode(eProjection mode){
	projection_mode_ = mode;
	isProjectionChanged_ = true;
}

const glm::mat4& Camera::getProjectionMatrix(void)const{
	return projection_matrix_;
}

