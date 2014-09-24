#include "include/convex_body.h"

ConvexBody::ConvexBody(std::vector<glm::vec3> vertices, std::vector<glm::uvec3> faces):
    vertices_(vertices)
{
    std::vector< glm::uvec2 > edges;
    std::vector< std::vector<unsigned int> > vertNeigh;
    /* Calculate Properties */
    //Find Edges

    //Iterate over each face and for each next face in the list, check if they
    //share two vertices, this defines an edge.
    std::vector<glm::uvec3>::const_iterator faceItr1, faceItr2;
    for(faceItr1 = faces.begin(); faceItr1 < faces.end(); ++faceItr1){
        for(faceItr2 = faceItr1 + 1; faceItr2 < faces.end(); ++faceItr2){
            uint fcount = 0;
            glm::uvec2 edge;
            for(int i = 0; i < 3; ++i){
                for(int j = 0; j < 3; ++j){
                    if((*faceItr1)[i] == (*faceItr2)[j]){
                        edge[fcount] = (*faceItr1)[i];
                        ++fcount;
                    }
                }
                if(fcount == 2){
                    edges.push_back(edge);
                    fcount = 0;
                }
            }
        }
    }

    //Find Vertex Neighbours
    //For all vertices, check if two edges share this vertex. If they do and it
    //isn't vertex 0, append the other vertices of these edge to the neighbor list
    for(unsigned int vi = 0; vi < vertices.size(); vi++){
        std::vector<unsigned int> neighbors;
        for(unsigned int ei = 0; ei < edges.size(); ei++){
            for(unsigned int i = 0; i < 2; i++){
                if(edges[ei][i] == vi){
                    if(edges[ei][(i + 1) % 2] != 0){
                        neighbors.push_back(edges[ei][(i + 1) % 2]);
                    }
                }
            }
        }
        if(!neighbors.empty()) vertex_neighbours_.push_back(neighbors);
    }

    //Find the inRadius
    //For each face, calculate its distance from the particle's center and find the min
    std::vector< glm::uvec3 >::const_iterator faceItr;
    double minDistance = 100.0;
    for(faceItr = faces.begin(); faceItr < faces.end(); ++faceItr){
        glm::vec3 p(vertices[(*faceItr)[0]]);

        glm::vec3 a(
            vertices[(*faceItr)[1]][0] - vertices[(*faceItr)[0]][0],
            vertices[(*faceItr)[1]][1] - vertices[(*faceItr)[0]][1],
            vertices[(*faceItr)[1]][2] - vertices[(*faceItr)[0]][2]
        );

        glm::vec3 b(
            vertices[(*faceItr)[2]][0] - vertices[(*faceItr)[0]][0],
            vertices[(*faceItr)[2]][1] - vertices[(*faceItr)[0]][1],
            vertices[(*faceItr)[2]][2] - vertices[(*faceItr)[0]][2]
        );

        glm::vec3 normal = glm::cross(a, b);
        double length = glm::length(normal);
        for(int i = 0; i < 3; ++i) normal[i] /= length;
        double faceDistance = fabs(glm::dot(normal, p));

        if(faceDistance < minDistance) minDistance = faceDistance;
    }
    in_radius = minDistance;

    //Find the circumRadius
    //It's just the farthest vertex from the particle's center
    double maxDistance = 0.0;
    std::vector<glm::vec3>::const_iterator vItr;
    for(vItr = vertices.begin(); vItr < vertices.end(); ++vItr){
        double vertexLength = glm::length(*vItr);
        if(vertexLength > maxDistance) maxDistance = vertexLength;
    }
    out_radius = maxDistance;
}

