#ifndef __CONVEX_BODY_H
#define __CONVEX_BODY_H

#include <vector>
#include <glm/glm.hpp>

struct ConvexBody{
    ConvexBody(std::vector<glm::vec3> vertices_, std::vector<glm::uvec3> faces);
    unsigned int poly_max(const glm::mat4& model_matrix, glm::vec3 axis)const;
    unsigned int poly_min(const glm::mat4& model_matrix, glm::vec3 axis)const;

    std::vector<glm::vec3>                 vertices_;
    std::vector<std::vector<unsigned int>> vertex_neighbours_;
    double in_radius, out_radius;
};

inline unsigned int ConvexBody::poly_max(const glm::mat4& model_matrix, glm::vec3 axis)const{
    //////////////////////////////////////////////////////
    //  Find the maximum projection on the axis using a //
    //  hill-climbing algorithm                         //
    //////////////////////////////////////////////////////
    axis = glm::transpose(glm::mat3(model_matrix)) * axis;
    unsigned int next = 0, last = 0, curr = 0;
    double p = 0.0;
    double max = glm::dot(vertices_[0], axis);
    std::vector<unsigned int>::const_iterator it;
    while(1){
        for(it = vertex_neighbours_[curr].begin(); it < vertex_neighbours_[curr].end(); it++){
            next = *it;
            if(next != last){
                p = glm::dot(vertices_[next], axis);
                if(p > max){
                    max = p;
                    last = curr;
                    curr = next;
                    break;
                }
            }
            if(it == vertex_neighbours_[curr].end() - 1) return curr;
        }
    }
}


inline unsigned int ConvexBody::poly_min(const glm::mat4& model_matrix, glm::vec3 axis)const{
    //////////////////////////////////////////////////////
    //  Find the maximum projection on the axis using a //
    //  hill-climbing algorithm                         //
    //////////////////////////////////////////////////////
    axis = glm::transpose(glm::mat3(model_matrix)) * axis;
    unsigned int next = 0, last = 0, curr = 0;
    double p = 0.0;
    double min = glm::dot(vertices_[0], axis);
    std::vector<unsigned int>::const_iterator it;
    while(1){
        for(it = vertex_neighbours_[curr].begin(); it < vertex_neighbours_[curr].end(); it++){
            next = *it;
            if(next != last){
                p = glm::dot(vertices_[next], axis);
                if(p < min){
                    min = p;
                    last = curr;
                    curr = next;
                    break;
                }
            }
            if(it == vertex_neighbours_[curr].end() - 1) return curr;
        }
    }
}


#endif
