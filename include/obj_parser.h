#ifndef __OBJ_PARSER_H
#define __OBJ_PARSER_H

#include "mesh.h"
#include <vector>
#include <string>

CMesh parse_mesh(const char *filename, std::string shading_model);

#endif
