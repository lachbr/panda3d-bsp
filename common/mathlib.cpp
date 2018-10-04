#include "cmdlib.h"
#include "messages.h"
#include "log.h"
#include "hlassert.h"
#include "mathtypes.h"
#include "mathlib.h"
#include "win32fix.h"

const vec3_t    vec3_origin = { 0, 0, 0 };

void ColorRGBExp32ToVector( const colorrgbexp32_t& in, LVector3& out )
{
        // FIXME: Why is there a factor of 255 built into this?
        out[0] = 255.0f * TexLightToLinear( in.r, in.exponent );
        out[1] = 255.0f * TexLightToLinear( in.g, in.exponent );
        out[2] = 255.0f * TexLightToLinear( in.b, in.exponent );
}

// given a floating point number  f, return an exponent e such that
// for f' = f * 2^e,  f is on [128..255].
// Uses IEEE 754 representation to directly extract this information
// from the float.
inline static int VectorToColorRGBExp32_CalcExponent( const float *pin )
{
        // The thing we will take advantage of here is that the exponent component
        // is stored in the float itself, and because we want to map to 128..255, we
        // want an "ideal" exponent of 2^7. So, we compute the difference between the
        // input exponent and 7 to work out the normalizing exponent. Thus if you pass in 
        // 32 (represented in IEEE 754 as 2^5), this function will return 2
        // (because 32 * 2^2 = 128)
        if ( *pin == 0.0f )
                return 0;

        unsigned int fbits = *reinterpret_cast<const unsigned int *>( pin );

        // the exponent component is bits 23..30, and biased by +127
        const unsigned int biasedSeven = 7 + 127;

        signed int expComponent = ( fbits & 0x7F800000 ) >> 23;
        expComponent -= biasedSeven; // now the difference from seven (positive if was less than, etc)
        return expComponent;
}

/// Slightly faster version of the function to turn a float-vector color into 
/// a compressed-exponent notation 32bit color. However, still not SIMD optimized.
/// PS3 developer: note there is a movement of a float onto an int here, which is
/// bad on the base registers -- consider doing this as Altivec code, or better yet
/// moving it onto the cell.
/// \warning: Assumes an IEEE 754 single-precision float representation! Those of you
/// porting to an 8080 are out of luck.
void VectorToColorRGBExp32( const LVector3 &vin, colorrgbexp32_t &c )
{
        hlassert( vin[0] >= 0.0f && vin[1] >= 0.0f && vin[1] >= 0.0f );

        // work out which of the channels is the largest ( we will use that to map the exponent )
        // this is a sluggish branch-based decision tree -- most architectures will offer a [max]
        // assembly opcode to do this faster.
        float pMax;
        if ( vin[0] > vin[1] )
        {
                if ( vin[0] > vin[2] )
                {
                        pMax = vin[0];
                } else
                {
                        pMax = vin[2];
                }
        } else
        {
                if ( vin[1] > vin[2] )
                {
                        pMax = vin[1];
                } else
                {
                        pMax = vin[2];
                }
        }

        // now work out the exponent for this luxel. 
        signed int exponent = VectorToColorRGBExp32_CalcExponent( &pMax );

        // make sure the exponent fits into a signed byte.
        // (in single precision format this is assured because it was a signed byte to begin with)
        hlassert( exponent > -128 && exponent <= 127 );

        // promote the exponent back onto a scalar that we'll use to normalize all the numbers
        float scalar;
        {
                unsigned int fbits = ( 127 - exponent ) << 23;
                scalar = *reinterpret_cast<float *>( &fbits );
        }

        // we should never need to clamp:
        hlassert( vin[0] * scalar <= 255.0f &&
                vin[1] * scalar <= 255.0f &&
                vin[2] * scalar <= 255.0f );

        // This awful construction is necessary to prevent VC2005 from using the 
        // fldcw/fnstcw control words around every float-to-unsigned-char operation.
        {
                int red = ( vin[0] * scalar );
                int green = ( vin[1] * scalar );
                int blue = ( vin[2] * scalar );

                c.r = red;
                c.g = green;
                c.b = blue;
        }
        /*
        c.r = ( unsigned char )(vin.x * scalar);
        c.g = ( unsigned char )(vin.y * scalar);
        c.b = ( unsigned char )(vin.z * scalar);
        */

        c.exponent = (signed char)exponent;
}

void GetBumpNormals( const LVector3 &svec, const LVector3 &tvec, const LVector3 &face_normal,
                     const LVector3 &phong_normal, LVector3 *bump_vecs )
{
        LVector3 tmp_normal;
        bool left_handed = false;
        int i;

        hlassert( NUM_BUMP_VECTS == 3 );

        // left handed or right handed?
        tmp_normal = svec.cross( tvec );
        if ( face_normal.dot( tmp_normal ) < 0.0 )
        {
                left_handed = true;
        }

        // build a basis for the face around the phong normal
        LMatrix3 smooth_basis;
        smooth_basis.set_row( 1, phong_normal.cross( svec ).normalized() );
        smooth_basis.set_row( 0, smooth_basis.get_row( 1 ).cross( phong_normal ).normalized() );
        smooth_basis.set_row( 2, phong_normal );

        if ( left_handed )
        {
                VectorInverse( smooth_basis[1] );
        }

        // move the g_localbumpbasis into world space to create bump_vecs
        for ( i = 0; i < 3; i++ )
        {
                VectorIRotate( g_localbumpbasis[i], smooth_basis, bump_vecs[i] );
        }
}