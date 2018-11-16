#ifndef BSPFILE_H__
#define BSPFILE_H__
#include "cmdlib.h" //--vluzacn
#include "mathlib.h"

#include <pvector.h>

#if _MSC_VER >= 1000
#pragma once
#endif

#define DEFAULT_TEXCONTENTS_FILE "D:\\OTHER\\lachb\\Documents\\cio\\cio-panda3d\\built_x64\\bin\\texturecontents.txt"

#define PBSP_MAGIC ( ( 'P' << 24 ) + ( 'S' << 16 ) + ( 'B' << 8 ) + 'P' )

#define DEFAULT_LIGHTMAP_SIZE 16.0

// upper design bounds

#define MAX_MAP_HULLS            4
// hard limit

#define MAX_MAP_MODELS         512 //400 //vluzacn
// variable, but 400 brush entities is very stressful on the engine and network code as it is

#define MAX_MAP_BRUSHES       32768
#define MAX_MAP_BRUSHSIDES    65536
// arbitrary, but large numbers of brushes generally require more lightmap's than the compiler can handle

#define MAX_ENGINE_ENTITIES   16384 //1024 //vluzacn
#define MAX_MAP_ENTITIES      16384 //2048 //vluzacn
// hard limit, in actuallity it is too much, as temporary entities in the game plus static map entities can overflow

#define MAX_MAP_ENTSTRING   (2048*1024) //(512*1024) //vluzacn
// abitrary, 512Kb of string data should be plenty even with TFC FGD's

#define MAX_MAP_PLANES      32768 // TODO: This can be larger, because although faces can only use plane 0~32767
#define MAX_INTERNAL_MAP_PLANES (256*1024)
// (from email): I have been building a rather complicated map, and using your latest 
// tools (1.61) it seemed to compile fine.  However, in game, the engine was dropping
// a lot of faces from almost every FUNC_WALL, and also caused a strange texture 
// phenomenon in software mode (see attached screen shot).  When I compiled with v1.41,
// I noticed that it hit the MAX_MAP_PLANES limit of 32k.  After deleting some brushes
// I was able to bring the map under the limit, and all of the previous errors went away.

#define MAX_MAP_NODES        32767
// hard limit (negative short's are used as contents values)

#define MAX_MAP_LEAFS        32760
#define MAX_MAP_LEAFS_ENGINE 8192
#define MAX_MAP_LEAFFACES    65536
#define MAX_MAP_LEAFBRUSHES  65536
// No problem has been observed in testmap or reported, except when viewing the map from outside (some leafs missing, no crash).
// This problem indicates that engine's MAX_MAP_LEAFS is 8192 (for reason, see: Quake - gl_model.c - Mod_Init).
// I don't know if visleafs > 8192 will cause Mod_DecompressVis overflow.

#define MAX_MAP_VERTS        65535
#define MAX_MAP_FACES        65535 // This ought to be 32768, otherwise faces(in world) can become invisible. --vluzacn
#define MAX_MAP_WORLDFACES   32768
#define MAX_MAP_MARKSURFACES 65535
// hard limit (data structures store them as unsigned shorts)

#define MAX_MAP_TEXTURES      4096 //512 //vluzacn
// hard limit (halflife limitation) // I used 2048 different textures in a test map and everything looks fine in both opengl and d3d mode.

#define MAX_MAP_TEXINFO      32767
// hard limit (face.texinfo is signed short)
#define MAX_INTERNAL_MAP_TEXINFO 262144

#define MAX_MAP_EDGES       256000
#define MAX_MAP_SURFEDGES   512000
// arbtirary

#define DEFAULT_MAX_MAP_TEXREF      0x2000000 //0x400000 //vluzacn
// 4Mb of textures is enough especially considering the number of people playing the game
// still with voodoo1 and 2 class cards with limited local memory.

#define DEFAULT_MAX_MAP_LIGHTDATA	0x3000000 //0x600000 //vluzacn
// arbitrary

#define MAX_MAP_VISIBILITY  0x800000 //0x200000 //vluzacn
// arbitrary

// these are for entity key:value pairs
#define MAX_KEY                 128 //32 //vluzacn
#define MAX_VAL               4096 // the name used to be MAX_VALUE //vluzacn
// quote from yahn: 'probably can raise these values if needed'

// texture size limit

#define MAX_TEXTURE_SIZE     348972 //((256 * 256 * sizeof(short) * 3) / 2) //stop compiler from warning 512*512 texture. --vluzacn
// this is arbitrary, and needs space for the largest realistic texture plus
// room for its mipmaps.'  This value is primarily used to catch damanged or invalid textures
// in a wad file

#define MAX_TEXTURE_NAME 256 // number of characters

#define TEXTURE_STEP 16 // Default is 16
#define MAX_LIGHTMAP_DIM 128 // Default is 16

#define ENGINE_ENTITY_RANGE 4096.0

#define MAX_LIGHTSTYLES 64
//=============================================================================

#define BSPVERSION  31
#define TOOLVERSION 3

// One hammer unit is 1/16th of a foot.
// One panda unit is one foot.
// Scale factors to convert between interfaces.
#define PANDA_TO_HAMMER 16.0
#define HAMMER_TO_PANDA 1.0 / PANDA_TO_HAMMER

//
// BSP File Structures
//


typedef struct
{
        int             fileofs, filelen;
} lump_t;

#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTEXES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_LEAFS        9
#define LUMP_MARKSURFACES 10
#define LUMP_EDGES        11
#define LUMP_SURFEDGES    12
#define LUMP_MODELS       13
#define LUMP_BRUSHES      14
#define LUMP_BRUSHSIDES   15
#define LUMP_LEAFAMBIENTLIGHTING 16
#define LUMP_LEAFAMBIENTINDEX    17
#define LUMP_LEAFBRUSHES         18
#define LUMP_STATICPROPS         19
#define LUMP_STATICPROPVERTEXDATA 20
#define LUMP_STATICPROPLIGHTING   21

//#define LUMP_ORIGFACES    15
#define HEADER_LUMPS      22

//#define LUMP_MISCPAD      -1
//#define LUMP_ZEROPAD      -2

typedef struct
{
        float           mins[3], maxs[3];
        float           origin[3];
        int             headnode[MAX_MAP_HULLS];
        int             visleafs;                              // not including the solid leaf 0
        int             firstface, numfaces;
} dmodel_t;

typedef struct
{
        int             ident;
        int             version;
        lump_t          lumps[HEADER_LUMPS];
} dheader_t;

typedef struct texref_s
{
        char            name[MAX_TEXTURE_NAME];
} texref_t;

typedef struct dvertex_s
{
        float           point[3];
} dvertex_t;

typedef struct
{
        float           normal[3];
        float           dist;
        planetypes      type;                                  // PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;

// Up to 32 unique contents
typedef enum
{
        CONTENTS_EMPTY          = 1 << 0,
        CONTENTS_SOLID          = 1 << 1,
        CONTENTS_WATER          = 1 << 2,
        CONTENTS_SLIME          = 1 << 3,
        CONTENTS_LAVA           = 1 << 4,
        CONTENTS_SKY            = 1 << 5,
        CONTENTS_ORIGIN         = 1 << 6,                                  // removed at csg time

        CONTENTS_CURRENT_0      = 1 << 7,
        CONTENTS_CURRENT_90     = 1 << 8,
        CONTENTS_CURRENT_180    = 1 << 9,
        CONTENTS_CURRENT_270    = 1 << 10,
        CONTENTS_CURRENT_UP     = 1 << 11,
        CONTENTS_CURRENT_DOWN   = 1 << 12,

        CONTENTS_TRANSLUCENT    = 1 << 13,
        CONTENTS_HINT           = 1 << 14,     // Filters down to CONTENTS_EMPTY by bsp, ENGINE SHOULD NEVER SEE THIS

        CONTENTS_NULL           = 1 << 15,     // AJM  // removed in csg and bsp, VIS or RAD shouldnt have to deal with this, only clip planes!


        CONTENTS_BOUNDINGBOX    = 1 << 16, // similar to CONTENTS_ORIGIN

        CONTENTS_TOEMPTY        = 1 << 17,
        CONTENTS_PROP           = 1 << 18,
} contents_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
        int             planenum;
        short           children[2];                           // negative numbers are -(leafs+1), not nodes
        short           mins[3];                               // for sphere culling
        short           maxs[3];
        unsigned short  firstface;
        unsigned short  numfaces;                              // counting both sides
} dnode_t;

typedef struct texinfo_s
{
        float           vecs[2][4]; // [s/t][xyz offset]
        float           lightmap_vecs[2][4]; // [s/t][xyz offset]
        int             lightmap_scale;
        int             texref;
        int             flags;
} texinfo_t;

#define TEX_SPECIAL     1                                  // sky or slime or null, no lightmap or 256 subdivision
#define TEX_BUMPLIGHT   2

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct dedge_s
{
        unsigned short  v[2];                                  // vertex numbers
} dedge_t;

#define MAXLIGHTMAPS    4
typedef struct dface_s
{
        unsigned short	planenum;
        byte   side;
        byte   on_node;
        unsigned int bumped_lightmap;                          // it's an int for padding

        int             firstedge;                             // we must support > 64k edges
        short           numedges;
        short           texinfo;

        // lighting info
        byte            styles[MAXLIGHTMAPS];
        int             lightofs;                              // start of [numstyles*surfsize] samples
        int             lightmap_mins[2];
        int             lightmap_size[2];

        // index into the g_dorigfaces array,
        // which original non-split face this face comes from.
        // it's needed for collision geometry, as the splitting
        // done is only useful for lightmaps and very inefficient
        // for collisions.
        //int		    origface;				   
} dface_t;

struct compressedlightcube_t
{
        colorrgbexp32_t color[6];
};

struct dleafambientlighting_t
{
        compressedlightcube_t cube;
        // fixed point fraction of leaf bounds
        unsigned char x, y, z, pad; // pad is unused
};

struct dleafambientindex_t
{
        unsigned short num_ambient_samples;
        unsigned short first_ambient_sample;
};

struct dbrush_t
{
        int firstside;
        int numsides;
        int contents;
};

struct dbrushside_t
{
        unsigned short planenum;
        short texinfo;
        //short dispinfo;
        short bevel;
};

enum
{
        DLF_in_ambient_cube = 0x1,
};

#define AMBIENT_WATER   0
#define AMBIENT_SKY     1
#define AMBIENT_SLIME   2
#define AMBIENT_LAVA    3

#define NUM_AMBIENTS            4                  // automatic ambient sounds

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info
typedef struct
{
        int             contents;
        int             visofs;                                // -1 = no visibility info

        short           mins[3];                               // for frustum culling
        short           maxs[3];

        unsigned short firstleafbrush;
        unsigned short numleafbrushes;

        unsigned short  firstmarksurface;
        unsigned short  nummarksurfaces;

        byte            ambient_level[NUM_AMBIENTS];
} dleaf_t;

enum
{
        STATICPROPFLAGS_NOLIGHTING              = 1 << 0,

        // use the vertex lighting baked in by p3rad
        STATICPROPFLAGS_STATICLIGHTING          = 1 << 1,

        // treat as a dynamic node, updated with ambient cube
        STATICPROPFLAGS_DYNAMICLIGHTING         = 1 << 2,

        // clear_model_nodes() and flatten_strong()
        STATICPROPFLAGS_HARDFLATTEN             = 1 << 3,

        // flatten together with other props in the same leaf
        STATICPROPFLAGS_GROUPFLATTEN            = 1 << 4,

        // cast real-time depth shadows
        STATICPROPFLAGS_REALSHADOWS             = 1 << 5,

        // prop casted shadows onto the lightmaps in p3rad
        STATICPROPFLAGS_LIGHTMAPSHADOWS         = 1 << 6,

        // set_two_sided()
        STATICPROPFLAGS_DOUBLESIDE              = 1 << 7,
};

struct dstaticprop_t
{
        float pos[3];
        float hpr[3];
        float scale[3];

        char name[128]; // path to model
        unsigned short flags;

        // hopefully the prop doesn't have >256 GeomVertexDatas...
        short first_vertex_data; // index into LUMP_STATICPROPVERTEXDATA
        short num_vertex_datas;

        short lightsrc; // index of light that this prop casts, -1 for none
};

struct dstaticpropvertexdata_t
{
        // assuming the order of vertex datas is the same each time the prop is loaded
        unsigned short first_lighting_sample; // index into LUMP_STATICPROPLIGHTING
        unsigned short num_lighting_samples;
};

typedef struct epair_s
{
        struct epair_s* next;
        char*           key;
        char*           value;
}
epair_t;

typedef struct
{
        vec3_t          origin;
        int             firstbrush;
        int             numbrushes;
        epair_t*        epairs;
}
entity_t;

//============================================================================

#define ANGLE_UP    -1.0 //#define ANGLE_UP    -1 //--vluzacn
#define ANGLE_DOWN  -2.0 //#define ANGLE_DOWN  -2 //--vluzacn

//
// BSP File Data
//

struct bspdata_t
{
        int      nummodels;
        dmodel_t dmodels[MAX_MAP_MODELS];
        int      dmodels_checksum;

        int      visdatasize;
        byte     dvisdata[MAX_MAP_VISIBILITY];
        int      dvisdata_checksum;

        pvector<colorrgbexp32_t>    dlightdata;
        int      dlightdata_checksum;

        int      numtexrefs;
        texref_t dtexrefs[MAX_MAP_TEXTURES];                                  // (dtexlump_t)
        int      dtexrefs_checksum;

        int      entdatasize;
        char     dentdata[MAX_MAP_ENTSTRING];
        int      dentdata_checksum;

        int      numleafs;
        dleaf_t  dleafs[MAX_MAP_LEAFS];
        int      dleafs_checksum;

        int      numplanes;
        dplane_t dplanes[MAX_INTERNAL_MAP_PLANES];
        int      dplanes_checksum;

        int      numvertexes;
        dvertex_t dvertexes[MAX_MAP_VERTS];
        int      dvertexes_checksum;

        int      numnodes;
        dnode_t  dnodes[MAX_MAP_NODES];
        int      dnodes_checksum;

        int      numtexinfo;
        texinfo_t texinfo[MAX_INTERNAL_MAP_TEXINFO];
        int      texinfo_checksum;

        int      numfaces;
        dface_t  dfaces[MAX_MAP_FACES];
        int      dfaces_checksum;

        int	numorigfaces;
        dface_t	dorigfaces[MAX_MAP_FACES];
        int	dorigfaces_checksum;

        int      numedges;
        dedge_t  dedges[MAX_MAP_EDGES];
        int      dedges_checksum;

        int      nummarksurfaces;
        unsigned short dmarksurfaces[MAX_MAP_MARKSURFACES];
        int      dmarksurfaces_checksum;

        int      numsurfedges;
        int      dsurfedges[MAX_MAP_SURFEDGES];
        int      dsurfedges_checksum;

        pvector<dleafambientlighting_t> leafambientlighting;
        pvector<dleafambientindex_t> leafambientindex;
        pvector<dbrush_t> dbrushes;
        pvector<dbrushside_t> dbrushsides;
        pvector<unsigned short> dleafbrushes;
        pvector<dstaticprop_t> dstaticprops;
        pvector<dstaticpropvertexdata_t> dstaticpropvertexdatas;
        pvector<colorrgbexp32_t> staticproplighting;

        int      numentities;
        entity_t entities[MAX_MAP_ENTITIES];
};

extern bspdata_t *g_bspdata;

extern void     DecompressVis( bspdata_t *data, const byte* src, byte* const dest, const unsigned int dest_length );
extern int      CompressVis( const byte* const src, const unsigned int src_length,
                             byte* dest, unsigned int dest_length );

extern bspdata_t     *LoadBSPImage( dheader_t* header );
extern bspdata_t     *LoadBSPFile( const char* const filename );
extern void     WriteBSPFile( bspdata_t *data, const char* const filename );
extern void     PrintBSPFileSizes( bspdata_t *data );
#ifdef PLATFORM_CAN_CALC_EXTENT
extern void		WriteExtentFile( bspdata_t *data, const char *const filename );
extern bool		CalcFaceExtents_test();
#else
extern void		LoadExtentFile( const char *const filename );
#endif
extern void		GetFaceExtents( bspdata_t *data, int facenum, int mins_out[2], int maxs_out[2] );

//
// Entity Related Stuff
//

extern void            ParseEntities( bspdata_t *data );
extern void            UnparseEntities( bspdata_t *data );

extern void            DeleteKey( entity_t* ent, const char* const key );
extern void            SetKeyValue( entity_t* ent, const char* const key, const char* const value );
extern const char*     ValueForKey( const entity_t* const ent, const char* const key );
extern int             IntForKey( const entity_t* const ent, const char* const key );
extern vec_t           FloatForKey( const entity_t* const ent, const char* const key );
extern void            GetVectorForKey( const entity_t* const ent, const char* const key, vec3_t vec );

extern entity_t* FindTargetEntity( bspdata_t *data, const char* const target );
extern epair_t* ParseEpair();
extern entity_t* EntityForModel( bspdata_t *data, int modnum );

extern int FastChecksum( const void* const buffer, int bytes );

//
// Texture Related Stuff
//

extern int      g_max_map_texref;
extern int		g_max_map_lightdata;
extern void     dtexdata_init();
extern void CDECL dtexdata_free();

extern char*    GetTextureByNumber( bspdata_t *data, int texturenumber );

extern map<string, contents_t> g_tex_contents;
extern std::string g_tex_contents_file;
extern void SetTextureContentsFile( const char *path );
extern void LoadTextureContents();
extern contents_t GetTextureContents( const char *texname );

extern LRGBColor dface_AvgLightColor( bspdata_t *data, dface_t *face, int style );
INLINE extern colorrgbexp32_t *SampleLightmap( bspdata_t *data, const dface_t *face, int luxel, int style, int bump = 0 );

#endif //BSPFILE_H__
