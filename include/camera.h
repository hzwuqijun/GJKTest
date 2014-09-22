#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera{
public:
	enum eProjection{Orthographic, Perspective};
	
	Camera(float aspect, const glm::vec3& pos = glm::vec3(0.0), glm::vec3 look_at = glm::vec3(0.0, 0.0, 0.0),
            eProjection mode = Perspective);
	~Camera(void);
	
	void update(void);
	void rotate(float angle, const glm::vec3& axis);
	void translate(const glm::vec3&);
	void setPosition(const glm::vec3&);
	void setRotation(float angle, const glm::vec3& axis);
	
	const glm::mat4& getViewMatrix(void)const;
	const glm::mat4& getProjectionMatrix(void)const;
    const glm::vec3& getPosition(void)const;

	void setProjectionParams(float aspect, float fov_ortho_h, float zn, float zf);
	void setProjectionMode(eProjection);
	
private:
	eProjection projection_mode_;
	bool isProjectionChanged_;
	bool isViewChanged_;
	glm::vec3 pos_;
	glm::fquat rotation_;
	float zn_, zf_;
	float fov_, aspect_, ortho_height_;
	glm::mat4 view_matrix_;
	glm::mat4 projection_matrix_;
};

#endif
