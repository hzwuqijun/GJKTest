#include "include/scene.h"

/********* Scene Object *********/

SceneObject::SceneObject(CMesh mesh, ConvexBody* body):
    Transformable(),
    is_selectable_(true), is_selected_(false), 
    color_(1.0f),
    mesh_(mesh), body_(body)
{}

void SceneObject::make_selectable(void){
    is_selectable_ = true;
}

void SceneObject::make_unselectable(void){
    is_selectable_ = false;
}

CMesh& SceneObject::getMesh(void){
    return mesh_;
}

void SceneObject::setColor(const glm::vec3& color){
    color_ = color;
}

const glm::vec3& SceneObject::getColor(void)const {
    return color_;
}

/********* Scene *********/

Scene::~Scene(void){
    for(auto child: children_){
        delete child;
    }
}

void Scene::addChild(SceneObject* object){
    children_.push_back(object);
}


//void Model::calc_bounding_box(void){
//    for(auto child: children_) child->calc_bounding_box()
//
//    glm::mat3 model_matrix = glm::mat3(modelMatrix_);
//    bb_min_ = model_matrix * mesh_.vertices_[0]._coord;
//    bb_max_ = model_matrix * mesh_.vertices_[0]._coord;
//    for(auto vertex: mesh_.vertices_){
//        auto coords = model_matrix * vertex._coord;
//        for(int i = 0; i < 3; ++i){
//            if(coords[i] > bb_max_[i]) bb_max_[i] = coords[i];
//            if(coords[i] < bb_min_[i]) bb_min_[i] = coords[i];
//        }
//    }
//    printf("%f, %f, %f\n", bb_max_[0], bb_max_[1], bb_max_[2]);
//}
