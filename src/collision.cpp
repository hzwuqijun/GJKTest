#include "include/collision.h"
#include <glm/gtc/type_ptr.hpp>

namespace{
using uchar = unsigned char;
using uint  = unsigned int;
// Lookup table which tells us at which positions in the simplex array
// to get our points a, b, c, d. I.e. if our bits are 0111 -> 7 -> {0, 1, 2}
const uchar p_pos[16][3] =
{
    {0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 1, 0},
    {2, 0, 0}, {0, 2, 0}, {1, 2, 0}, {0, 1, 2},
    {3, 0, 0}, {0, 3, 0}, {1, 3, 0}, {0, 1, 3},
    {2, 3, 0}, {0, 2, 3}, {1, 2, 3}, {0, 0, 0}
};

const uchar s_pos[] = {0, 0, 1, 0, 2, 0, 0, 0, 3}; //Lookup table for single enabled bit position
//_______________________________^__^_____^___________^

struct Simplex{
    float p[12];//up to 4 points / 3-Simplex
    uchar bits;
    uchar last_sb;
    uchar size;
};

inline uchar addPoint(Simplex* s){
    uchar b = ~s->bits; //Flip bits
    b &= -b; //Last set (available) bit
    uchar pos = s_pos[b]; //Get the bit position from the lookup table
    s->last_sb = pos;
    s->bits |= b; //Insert the new bit
    s->size++;
    return pos;
}

inline void removePoint(Simplex *s, int p){
    s->bits ^= (1 << p); //Erase the bit at position p
    s->size--;
}

inline float dot_p(const float* a, const float* b){
    //////////////////////
    //   Dot Product    //
    //////////////////////
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void cross_p(const float* a, const float* b, float* c){
    //////////////////////
    //  Cross Product   //
    //////////////////////
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

inline void triple_p(const float* a, const float* b, const float* c, float* result){
    //////////////////////////////
    // Triple product (axb)xc   //
    //////////////////////////////
    float cross_temp[3];
    cross_p(a, b, cross_temp);
    cross_p(cross_temp, c, result);
}

inline void support(
    Simplex* s,
    const ConvexBody& body_a,
    const ConvexBody& body_b,
    const glm::mat4& model_matrix_a,
    const glm::mat4& model_matrix_b,
    const glm::vec3& dir)
{
    //////////////////////////////////////////////////////
    //  The support function. This provides us with     //
    //  the next point for our simplex.                 //
    //////////////////////////////////////////////////////
    uint pa, pb;
    pa = body_a.poly_max(model_matrix_a, dir);
    pb = body_b.poly_min(model_matrix_b, dir);

    auto vertex_a = model_matrix_a * glm::vec4(body_a.vertices_[pa], 1.0);
    auto vertex_b = model_matrix_b * glm::vec4(body_b.vertices_[pb], 1.0);

    auto dist_vec = glm::vec3(vertex_a) / vertex_a.w - glm::vec3(vertex_b) / vertex_b.w;

    int pos = addPoint(s);

    s->p[3 * pos + 0] = dist_vec[0];
    s->p[3 * pos + 1] = dist_vec[1];
    s->p[3 * pos + 2] = dist_vec[2];
}

bool containsOrigin(Simplex* s, float* dir){
    ///////////////////////////////////////////////
    //  Check if the origin is contained in the  //
    //  Minkowski sum.                           //
    ///////////////////////////////////////////////
    switch( s->size ){
    case 4:
    {
        /////////////////////////////////////////////////////
        ///////////////////* Tetrahedron *///////////////////
        /////////////////////////////////////////////////////

        const uchar* pos = p_pos[(s->bits ^ (1 << s->last_sb))];

        float *a = (s->p + 3*s->last_sb);
        float *b = (s->p + 3*pos[0]);
        float *c = (s->p + 3*pos[1]);
        float *d = (s->p + 3*pos[2]);
        float ab[3], ac[3], ad[3];
        float abcPerp[3], acdPerp[3], abdPerp[3];
        float abPerp1[3], abPerp2[3];
        float acPerp1[3], acPerp2[3];
        float adPerp1[3], adPerp2[3];
        float abxac[3], abxad[3], acxad[3];

        for(uint i = 0; i < 3; i++){
            ab[i] = b[i] - a[i];
            ac[i] = c[i] - a[i];
            ad[i] = d[i] - a[i];
        }

        ////////////////////* Face Cases *///////////////////


        /* Find the triangle face normals with the correct sign (pointing outward) */
        cross_p(ab, ac, abxac);
        cross_p(ab, ad, abxad);
        cross_p(ac, ad, acxad);

        int sign_abc = (dot_p(abxac, ad) > 0.0)? -1: 1;
        int sign_acd = (dot_p(acxad, ab) > 0.0)? -1: 1;
        int sign_abd = (dot_p(abxad, ac) > 0.0)? -1: 1;
        for(uint i = 0; i < 3; i++){
            abcPerp[i] = sign_abc * abxac[i];
            acdPerp[i] = sign_acd * acxad[i];
            abdPerp[i] = sign_abd * abxad[i];
        }

        cross_p(acxad, ac, acPerp1);
        cross_p(ad, acxad, adPerp2);
        bool acPerp1Pos = (dot_p(acPerp1, a) > 0.0);
        bool adPerp2Pos = (dot_p(adPerp2, a) > 0.0);
        /* On acd side */
        // The origin should be on acd's side and between the half-spaces defined by ac and ad (normal to acd)
        if((dot_p(acdPerp, a) < 0.0) && !acPerp1Pos && !adPerp2Pos){
            /* Remove point b */
            removePoint(s, pos[0]);
            memcpy(dir, acdPerp, 3*sizeof(*dir));
            break;
        }

        cross_p(abxad, ab, abPerp2);
        cross_p(ad, abxad, adPerp1);
        bool abPerp2Pos = (dot_p(abPerp2, a) > 0.0);
        bool adPerp1Pos = (dot_p(adPerp1, a) > 0.0);
        /* On abd side */
        // The origin should be on abd's side and between the half-spaces defined by ab and ad (normal to abd)
        if((dot_p(abdPerp, a) < 0.0) && !abPerp2Pos && !adPerp1Pos){
            /* Remove point c */
            removePoint(s, pos[1]);
            memcpy(dir, abdPerp, 3*sizeof(*dir));
            break;
        }

        cross_p(abxac, ab, abPerp1);
        cross_p(ac, abxac, acPerp2);
        bool abPerp1Pos = (dot_p(abPerp1, a) > 0.0);
        bool acPerp2Pos = (dot_p(acPerp2, a) > 0.0);
        /* On abc side */
        // The origin should be on abc's side and between the half-spaces defined by ac and ab (normal to abc)
        if((dot_p(abcPerp,a) < 0.0) && !abPerp1Pos && !acPerp2Pos){
            /* Remove point d */
            removePoint(s, pos[2]);
            memcpy(dir, abcPerp, 3*sizeof(*dir));
            break;
        }

        ////////////////////* Edge Cases *///////////////////

        /* ab Edge case */
        // The origin must be inside the space defined by the intersection
        // of two half-space normal to the adjacent faces abc, abd
        if(abPerp1Pos && abPerp2Pos){
            triple_p(a, ab, ab, dir);
            removePoint(s, pos[1]);
            removePoint(s, pos[2]);
            break;
        }


        /* ac Edge case */
        // The origin must be inside the space defined by the intersection
        // of two half-space normal to the adjacent faces abc, acd
        if(acPerp1Pos && acPerp2Pos){
            triple_p(a, ac, ac, dir);
            removePoint(s, pos[0]);
            removePoint(s, pos[2]);
            break;
        }

        /* ad Edge case */
        // The origin must be inside the space defined by the intersection
        // of two half-space normal to the adjacent faces acd, abd
        if(adPerp1Pos && adPerp2Pos){
            triple_p(a, ad, ad, dir);
            removePoint(s, pos[0]);
            removePoint(s, pos[1]);
            break;
        }
        /* 'else' should only be when the origin is inside the tetrahedron */
        return true;
    }
    case 3:
    {
        /////////////////////////////////////////////////////
        /////////////////////* Triangle *////////////////////
        /////////////////////////////////////////////////////

        const uchar* pos = p_pos[(s->bits ^ (1 << s->last_sb))];

        float *a = (s->p + 3*s->last_sb);
        float *b = (s->p + 3*pos[0]);
        float *c = (s->p + 3*pos[1]);
        float ab[3], ac[3];
        float abPerp[3], acPerp[3];

        for(uint i = 0; i < 3; i++){
            ab[i] = b[i] - a[i];
            ac[i] = c[i] - a[i];
        }

        ////////////////////* Edge Cases *///////////////////

        float abxac[3];
        cross_p(ab, ac, abxac);

        /* Origin on the outside of triangle and close to ab */
        cross_p(ab, abxac, abPerp);
        if(dot_p(abPerp, a) < 0.0){
            triple_p(a, ab, ab, dir);
            /* Remove Point c */
            removePoint(s, pos[1]);
            break;
        }

        /* Origin on the outside of triangle and close to ac */
        cross_p(abxac, ac, acPerp);
        if(dot_p(acPerp, a) < 0.0){
            triple_p(a, ac, ac, dir);
            /* Remove Point b */
            removePoint(s, pos[0]);
            break;
        }

        /////////////////////* Face Case *///////////////////

        int sign = (dot_p(abxac, a) > 0.0)? -1: 1;
        for(uint i = 0; i < 3; i++) dir[i] = sign * abxac[i];
        break;
    }
    case 2:
    {
        /////////////////////////////////////////////////////
        ///////////////////////* Line *//////////////////////
        /////////////////////////////////////////////////////

        const uchar* pos = p_pos[(s->bits ^ (1 << s->last_sb))];

        float *a = (s->p + 3*s->last_sb);
        float *b = (s->p + 3*pos[0]);
        float ab[3];
        for(uint i = 0; i < 3; i++) ab[i] = b[i] - a[i];
        triple_p(a, ab, ab, dir);
        break;
    }
    case 1:
    {
        /////////////////////////////////////////////////////
        ///////////////////////* Point */////////////////////
        /////////////////////////////////////////////////////
        float *a = (s->p + 3*s->last_sb);
        for(uint i = 0; i < 3; i++) dir[i] = -a[i]; //Take direction passing through origin
        break;
    }
    default: break;
    }
    return false;
}

bool gjk_overlap(
    const ConvexBody& a, const ConvexBody& b,
    const glm::mat4& model_matrix_a, const glm::mat4& model_matrix_b
)
{
    float dir[3] = {1.0, 0.0, 0.0};
    Simplex S;
    S.size = 0;
    S.bits = 0;

    uint fail_safe = 0;

    while(fail_safe < 20){

        fail_safe++;

        support(&S, a, b, model_matrix_a, model_matrix_b, glm::make_vec3(dir));

        if(dot_p((S.p+3*(S.last_sb)), dir) < 0.0) return false;
        else if(containsOrigin(&S, dir)) return true;
    }
    printf("Encountered error in GJK: Infinite Loop.\n Direction (%f, %f, %f)\n", dir[0], dir[1], dir[2]);
    return false;
}

};

bool find_collisions(const Scene& scene){
    for(auto object_a = scene.children_.begin(); object_a < scene.children_.end(); ++object_a){
        if((*object_a)->getBody()){
            for(auto object_b = object_a + 1; object_b < scene.children_.end(); ++object_b){
                if((*object_b)->getBody()){
                    return gjk_overlap(*(*object_a)->getBody(),       *(*object_b)->getBody(),
                                        (*object_a)->getModelMatrix(), (*object_b)->getModelMatrix());
                }
            }
        }
    }

    return false;
}

