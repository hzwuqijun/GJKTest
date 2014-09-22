#include "include/mesh.h"
#include "include/main.h"

CMesh::CMesh(std::vector<Vertex> vertices):
    vertices_(vertices),
    vaoID(0), vboID(0)
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray((GLuint)0);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray((GLuint)1);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));

	glEnableVertexAttribArray((GLuint)2);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(glm::vec3)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CMesh::~CMesh(void){
	if(vaoID != 0) glDeleteBuffers(1, &vaoID);
	if(vboID != 0) glDeleteBuffers(1, &vboID);
}

void CMesh::draw(void)const{
	glBindVertexArray(vaoID); 
	glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
	glBindVertexArray(0);
}

bool CMesh::intersect_triangle(const glm::vec3* vertices, glm::vec3 ray_start, glm::vec3 ray_dir, float &t){
	auto AC = vertices[2] - vertices[0];
	auto AB = vertices[1] - vertices[0];

	auto normal = glm::cross(AB, AC);
    if(glm::dot(normal, ray_dir) >= 0.0f) return false;
    normal = glm::normalize(normal);

	auto P = glm::cross(ray_dir, AC);

	float det = glm::dot(AB, P);
	if(det <= 0.0f) return false;

	float invDet = 1.0f / det;
	auto T   = ray_start - vertices[0];
	auto Q   = glm::cross(T, AB);
	float t1 = glm::dot(AC, Q) * invDet;
	if(t1 > t || t1 < 0.0f) return false;

	float u = glm::dot(T, P) * invDet;
	if(u < 0.0f || u > 1.0f) return false;

	float v = glm::dot(ray_dir, Q) * invDet;
	if(v < 0.0f || u + v > 1.0f) return false;

	t = t1;
	return true;
}

bool CMesh::intersect(const glm::mat4& modelMatrix, glm::vec3 ray_start, glm::vec3 ray_dir, float &t){
    bool is_intersect = false;
    for(unsigned int i = 0; i < vertices_.size() / 3; ++i){
        glm::vec3 vertices[3];
        for(int j = 0; j < 3; ++j){
            auto vec = modelMatrix * glm::vec4(vertices_[3 * i + j]._coord, 1.0f);
            vertices[j] = glm::vec3(vec) / vec.w;
        }
        if(intersect_triangle(vertices, ray_start, ray_dir, t)){
            is_intersect = true;
            break;
        }
    }
    return is_intersect;
}
