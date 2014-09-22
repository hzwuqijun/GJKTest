#ifndef __MESH_H
#define __MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Vertex{
	glm::vec3 _coord;
	glm::vec3 _normal;
	glm::vec2 _texCoord;
	
	Vertex(glm::vec3 coord, glm::vec3 normal, glm::vec2 texCoord = glm::vec2(0.0, 0.0)):
		_coord(coord),
		_normal(normal),
		_texCoord(texCoord)
	{}
};

class CMesh{
public:
	CMesh(std::vector<Vertex> vertices_);
	~CMesh(void);
	
	void draw(void)const;
    bool intersect(const glm::mat4& modelMatrix, glm::vec3 ray_start, glm::vec3 ray_dir, float &t);
	
	std::vector<Vertex> vertices_;
	
private:
    bool intersect_triangle(const glm::vec3* vertices, glm::vec3 ray_start, glm::vec3 ray_dir, float &t);
	
	GLuint vaoID;
	GLuint vboID;
};
#endif
