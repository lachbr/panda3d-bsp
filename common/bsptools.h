#ifndef BSPTOOLS_H
#define BSPTOOLS_H

#include <aa_luse.h>
#include "bspfile.h"

#define TEST_EPSILON 0.03125
#define DIST_EPSILON 0.03125

struct Ray
{
        Ray( const LPoint3 &s, const LPoint3 &e,
             const LPoint3 &mi, const LPoint3 &ma )
        {
                start = s;
                end = e;
                mins = mi;
                maxs = ma;
                delta = end - start;

                is_swept = delta.length_squared() != 0;
                extents = maxs - mins;
                extents *= 0.5;
                is_ray = extents.length_squared() < 1e-6;
                start_offset = ( mins + maxs ) / 2;
                start = ( start + start_offset );
                start_offset /= -1;
        }

        LPoint3 start;
        LPoint3 end;
        LPoint3 mins;
        LPoint3 maxs;
        LVector3 extents;
        bool is_swept;
        bool is_ray;
        LVector3 start_offset;
        LVector3 delta;
};

struct Trace
{
        LPoint3 start_pos;
        LPoint3 end_pos;
        dplane_t plane;
        PN_stdfloat fraction;
        PN_stdfloat fraction_left_solid;
        int contents;
        bool all_solid;
        bool start_solid;
        bool is_point;
        LPoint3 mins;
        LPoint3 maxs;
        LVector3 extents;
        texinfo_t *surface;

        bspdata_t *bspdata;

        INLINE bool has_hit() const
        {
                return fraction != 1.0;
        }

        Trace() :
                fraction( 1.0 ),
                fraction_left_solid( 0.0 ),
                contents( 0 ),
                all_solid( false ),
                start_solid( false ),
                is_point( false ),
                surface( nullptr ),
                bspdata( nullptr )
        {
        }

};

class BaseBSPEnumerator
{
public:
        BaseBSPEnumerator( bspdata_t *data );
        virtual bool enumerate_node( int node_id, const Ray &ray,
                                     float f, int context ) = 0;

        virtual bool enumerate_leaf( int leaf_id, const Ray &ray, float start,
                                     float end, int context ) = 0;

        virtual bool find_intersection( const Ray &ray ) = 0;

        bspdata_t *data;
};

extern bool r_enumerate_nodes_along_ray( int node_id, const Ray &ray, float start,
                                         float end, BaseBSPEnumerator *surf, int context, float scale = 1.0 );

extern bool enumerate_nodes_along_ray( const Ray &ray, BaseBSPEnumerator *surf, int context, float scale = 1.0 );

#endif // BSPTOOLS_H