/*
 * SOLID - Software Library for Interference Detection
 * 
 * Copyright (C) 2001-2003  Dtecta.  All rights reserved.
 *
 * This library may be distributed under the terms of the Q Public License
 * (QPL) as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * This library may be distributed and/or modified under the terms of the
 * GNU General Public License (GPL) version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * This library is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Commercial use or any other use of this library not covered by either 
 * the QPL or the GPL requires an additional license from Dtecta. 
 * Please contact info@dtecta.com for enquiries about the terms of commercial
 * use of this library.
 */

#include "include/DT_Convex.h"

//#define DEBUG

#include "include/DT_GJK.h"
#include <glm/gtc/type_ptr.hpp>

//#include <algorithm>
//#include <new>


MT_Scalar closest_points(const ConvexBody& a, const ConvexBody& b,
    const glm::mat4& model_matrix_a, const glm::mat4& model_matrix_b)
{
	MT_Vector3 v(MT_Scalar(0.0), MT_Scalar(0.0), MT_Scalar(0.0));
	
    DT_GJK gjk;

	MT_Scalar dist2 = MT_INFINITY;

    do
	{
		unsigned int idxa = a.poly_min(model_matrix_a, glm::make_vec3(v.m_co));
		unsigned int idxb = b.poly_max(model_matrix_b, glm::make_vec3(v.m_co));

        auto vertex_a = model_matrix_a * glm::vec4(a.vertices_[idxa], 1.0);
        auto vertex_b = model_matrix_b * glm::vec4(b.vertices_[idxb], 1.0);

        auto pv = glm::vec3(vertex_a) / vertex_a.w;
        MT_Vector3 p = MT_Vector3(pv.x, pv.y, pv.z);

        auto qv = glm::vec3(vertex_b) / vertex_b.w;
        MT_Vector3 q = MT_Vector3(qv.x, qv.y, qv.z);

		MT_Vector3 w = p - q;

		MT_Scalar delta = v.dot(w);
		//if (delta > MT_Scalar(0.0) && delta * delta > dist2 * 100.0) 
		//{
		//	return MT_INFINITY;
		//}

		if (gjk.inSimplex(w) || dist2 - delta <= dist2 * 1e-10) 
		{
            break;
		}

		gjk.addVertex(w, p, q);
        if (gjk.isAffinelyDependent())
        {
            break;
        }

        if (!gjk.closest(v)) 
		{
            break;
        }

		dist2 = v.length2();
    }
    while (!gjk.fullSimplex() && dist2 > 1e-5 * gjk.maxVertex()); 

	assert(!gjk.emptySimplex());
	
	return dist2;
}
