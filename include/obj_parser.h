#ifndef __OBJ_PARSER_H
#define __OBJ_PARSER_H

#include "mesh.h"
#include "convex_body.h"
#include <vector>
#include <string>

CMesh      parse_mesh(const char *filename, const std::string& shading_model);
ConvexBody parse_convex_body(const char *filename);

#endif
