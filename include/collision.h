#ifndef __COLLISION_H
#define __COLLISION_H

#include "scene.h"
#include <utility>

std::vector<std::pair<SceneObject*, SceneObject*>> find_collisions(const Scene& scene);

#endif
