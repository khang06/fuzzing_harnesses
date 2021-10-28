//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef VTF_H
#define VTF_H

#ifdef _WIN32
#pragma once
#endif

//#include "bitmap/imageformat.h"
//#include "tier0/platform.h"

// manually defining what's required...
typedef unsigned char uint8;
typedef signed char int8;

#if defined( _WIN32 )

typedef __int16					int16;
typedef unsigned __int16		uint16;
typedef __int32					int32;
typedef unsigned __int32		uint32;
typedef __int64					int64;
typedef unsigned __int64		uint64;

#ifdef PLATFORM_64BITS
typedef __int64 intp;				// intp is an integer that can accommodate a pointer
typedef unsigned __int64 uintp;		// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
#else
typedef __int32 intp;
typedef unsigned __int32 uintp;
#endif

// Use this to specify that a function is an override of a virtual function.
// This lets the compiler catch cases where you meant to override a virtual
// function but you accidentally changed the function signature and created
// an overloaded function. Usage in function declarations is like this:
// int GetData() const OVERRIDE;
#define OVERRIDE override

#else // _WIN32

typedef short					int16;
typedef unsigned short			uint16;
typedef int						int32;
typedef unsigned int			uint32;
typedef long long				int64;
typedef unsigned long long		uint64;
#ifdef PLATFORM_64BITS
typedef long long			intp;
typedef unsigned long long	uintp;
#else
typedef int					intp;
typedef unsigned int		uintp;
#endif
typedef void* HWND;

// Avoid redefinition warnings if a previous header defines this.
#undef OVERRIDE
#if __cplusplus >= 201103L
#define OVERRIDE override
#if defined(__clang__)
	// warning: 'override' keyword is a C++11 extension [-Wc++11-extensions]
	// Disabling this warning is less intrusive than enabling C++11 extensions
#pragma GCC diagnostic ignored "-Wc++11-extensions"
#endif
#else
#define OVERRIDE
#endif

#endif // else _WIN32

enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

#if defined( _X360 )
	// Depth-stencil texture formats
	IMAGE_FORMAT_X360_DST16,
	IMAGE_FORMAT_X360_DST24,
	IMAGE_FORMAT_X360_DST24F,
	// supporting these specific formats as non-tiled for procedural cpu access
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,
#endif

	IMAGE_FORMAT_DXT1_RUNTIME,
	IMAGE_FORMAT_DXT5_RUNTIME,

	NUM_IMAGE_FORMATS
};

class CUtlBuffer
{
public:
	char dummy[1024]; // too lazy to actually have the proper stuff

	CUtlBuffer(int growSize = 0, int initSize = 0, int nFlags = 0);
	CUtlBuffer(const void* pBuffer, int size, int nFlags = 0);
	void			Put(const void* pMem, int size);
};

// #define VTF_FILE_FORMAT_ONLY to just include the vtf header and none of the code declaration
#ifndef VTF_FILE_FORMAT_ONLY

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CUtlBuffer;
class Vector;
struct Rect_t;
class IFileSystem;

//-----------------------------------------------------------------------------
// Texture flags
//-----------------------------------------------------------------------------
enum CompiledVtfFlags
{
	// flags from the *.txt config file
	TEXTUREFLAGS_POINTSAMPLE	               = 0x00000001,
	TEXTUREFLAGS_TRILINEAR		               = 0x00000002,
	TEXTUREFLAGS_CLAMPS			               = 0x00000004,
	TEXTUREFLAGS_CLAMPT			               = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC	               = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5		               = 0x00000020,
	TEXTUREFLAGS_SRGB						   = 0x00000040,
	TEXTUREFLAGS_NORMAL			               = 0x00000080,
	TEXTUREFLAGS_NOMIP			               = 0x00000100,
	TEXTUREFLAGS_NOLOD			               = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS			           = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL		               = 0x00000800,
	
	// These are automatically generated by vtex from the texture data.
	TEXTUREFLAGS_ONEBITALPHA	               = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA	               = 0x00002000,

	// newer flags from the *.txt config file
	TEXTUREFLAGS_ENVMAP			               = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET	               = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET	           = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE               = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY		               = 0x00040000,
	
	TEXTUREFLAGS_STAGING_MEMORY                = 0x00080000,
	TEXTUREFLAGS_IMMEDIATE_CLEANUP			   = 0x00100000,
	TEXTUREFLAGS_IGNORE_PICMIP				   = 0x00200000,
		TEXTUREFLAGS_UNUSED_00400000           = 0x00400000,

	TEXTUREFLAGS_NODEPTHBUFFER                 = 0x00800000,

		TEXTUREFLAGS_UNUSED_01000000           = 0x01000000,

	TEXTUREFLAGS_CLAMPU                        = 0x02000000,

	TEXTUREFLAGS_VERTEXTEXTURE                 = 0x04000000,					// Usable as a vertex texture

	TEXTUREFLAGS_SSBUMP                        = 0x08000000,
					
		TEXTUREFLAGS_UNUSED_10000000           = 0x10000000,

	// Clamp to border color on all texture coordinates
	TEXTUREFLAGS_BORDER						   = 0x20000000,

		TEXTUREFLAGS_UNUSED_40000000		   = 0x40000000,
		TEXTUREFLAGS_UNUSED_80000000		   = 0x80000000,
};

enum VersionedVtfFlags
{
	VERSIONED_VTF_FLAGS_MASK_7_3			=	~0xD1780400,	// For a ver 7.3 or earlier only these flags are valid
};


struct VtfProcessingOptions
{
	uint32 cbSize;					// Set to sizeof( VtfProcessingOptions )

	//
	// Flags0
	//
	enum Flags0
	{
		// Have a channel decaying to a given decay goal for the given last number of mips
		OPT_DECAY_R				= 0x00000001,	// Red decays
		OPT_DECAY_G				= 0x00000002,	// Green decays
		OPT_DECAY_B				= 0x00000004,	// Blue decays
		OPT_DECAY_A				= 0x00000008,	// Alpha decays
		
		OPT_DECAY_EXP_R			= 0x00000010,	// Channel R decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_G			= 0x00000020,	// Channel G decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_B			= 0x00000040,	// Channel B decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_A			= 0x00000080,	// Channel A decays exponentially (otherwise linearly)

		OPT_NOCOMPRESS			= 0x00000100,	// Use uncompressed image format
		OPT_NORMAL_DUDV			= 0x00000200,	// dU dV normal map
		OPT_FILTER_NICE			= 0x00000400,	// Use nice filtering

		OPT_SET_ALPHA_ONEOVERMIP		= 0x00001000,	// Alpha  = 1/miplevel
		OPT_PREMULT_COLOR_ONEOVERMIP	= 0x00002000,	// Color *= 1/miplevel
		OPT_MIP_ALPHATEST		= 0x00004000,	// Alpha-tested mip generation
	};

	uint32 flags0;					// A combination of "Flags0"

	//
	// Decay settings
	//
	uint8 clrDecayGoal[4];			// Goal colors for R G B A
	uint8 numNotDecayMips[4];		// Number of first mips unaffected by decay (0 means all below mip0)
	float fDecayExponentBase[4];	// For exponential decay the base number (e.g. 0.75)
};


//-----------------------------------------------------------------------------
// Cubemap face indices
//-----------------------------------------------------------------------------
enum CubeMapFaceIndex_t
{
	CUBEMAP_FACE_RIGHT = 0,
	CUBEMAP_FACE_LEFT,
	CUBEMAP_FACE_BACK,	// NOTE: This face is in the +y direction?!?!?
	CUBEMAP_FACE_FRONT,	// NOTE: This face is in the -y direction!?!?
	CUBEMAP_FACE_UP,
	CUBEMAP_FACE_DOWN,

	// This is the fallback for low-end
	CUBEMAP_FACE_SPHEREMAP,

	// NOTE: Cubemaps have *7* faces; the 7th is the fallback spheremap
	CUBEMAP_FACE_COUNT
};


//-----------------------------------------------------------------------------
// Enumeration used for spheremap generation
//-----------------------------------------------------------------------------
enum LookDir_t
{
	LOOK_DOWN_X = 0,
	LOOK_DOWN_NEGX,
	LOOK_DOWN_Y,
	LOOK_DOWN_NEGY,
	LOOK_DOWN_Z,
	LOOK_DOWN_NEGZ,
};


//-----------------------------------------------------------------------------
// Use this image format if you want to perform tool operations on the texture
//-----------------------------------------------------------------------------
#define IMAGE_FORMAT_DEFAULT	((ImageFormat)-2)

//-----------------------------------------------------------------------------
// Interface to get at various bits of a VTF texture
//-----------------------------------------------------------------------------
class IVTFTexture
{
public:
	virtual ~IVTFTexture() {}

	// Initializes the texture and allocates space for the bits
	// In most cases, you shouldn't force the mip count.
	virtual bool Init( int nWidth, int nHeight, int nDepth, ImageFormat fmt, int nFlags, int iFrameCount, int nForceMipCount = -1 ) = 0;

	// Methods to set other texture fields
	virtual void SetBumpScale( float flScale ) = 0;
	virtual void SetReflectivity( const Vector &vecReflectivity ) = 0;

	// Methods to initialize the low-res image
	virtual void InitLowResImage( int nWidth, int nHeight, ImageFormat fmt ) = 0;

	// set the resource data (for writers). pass size=0 to delete data. if pdata is not null,
	// the resource data will be copied from *pData
	virtual void *SetResourceData( uint32 eType, void const *pData, size_t nDataSize ) = 0;

	// find the resource data and return a pointer to it. The data pointed to by this pointer will
	// go away when the ivtftexture does. returns null if resource not present
	virtual void *GetResourceData( uint32 eType, size_t *pDataSize ) const = 0;

	// Locates the resource entry info if it's present, easier than crawling array types
	virtual bool HasResourceEntry( uint32 eType ) const = 0;

	// Retrieve available resource types of this IVTFTextures
	//		arrTypesBuffer			buffer to be filled with resource types available.
	//		numTypesBufferElems		how many resource types the buffer can accommodate.
	// Returns:
	//		number of resource types available (can be greater than "numTypesBufferElems"
	//		in which case only first "numTypesBufferElems" are copied to "arrTypesBuffer")
	virtual unsigned int GetResourceTypes( uint32 *arrTypesBuffer, int numTypesBufferElems ) const = 0;

	// When unserializing, we can skip a certain number of mip levels,
	// and we also can just load everything but the image data
	// NOTE: If you load only the buffer header, you'll need to use the
	// VTFBufferHeaderSize() method below to only read that much from the file
	// NOTE: If you skip mip levels, the height + width of the texture will
	// change to reflect the size of the largest read in mip level
	virtual bool Unserialize( CUtlBuffer &buf, bool bHeaderOnly = false, int nSkipMipLevels = 0 ) = 0;
	virtual bool Serialize( CUtlBuffer &buf ) = 0;

	// These are methods to help with optimization:
	// Once the header is read in, they indicate where to start reading
	// other data (measured from file start), and how many bytes to read....
	virtual void LowResFileInfo( int *pStartLocation, int *pSizeInBytes) const = 0;
	virtual void ImageFileInfo( int nFrame, int nFace, int nMip, int *pStartLocation, int *pSizeInBytes) const = 0;
	virtual int FileSize( int nMipSkipCount = 0 ) const = 0;

	// Attributes...
	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual int Depth() const = 0;
	virtual int MipCount() const = 0;

	// returns the size of one row of a particular mip level
	virtual int RowSizeInBytes( int nMipLevel ) const = 0;

	// returns the size of one face of a particular mip level
	virtual int FaceSizeInBytes( int nMipLevel ) const = 0;

	virtual ImageFormat Format() const = 0;
	virtual int FaceCount() const = 0;
	virtual int FrameCount() const = 0;
	virtual int Flags() const = 0;

	virtual float BumpScale() const = 0;

	virtual int LowResWidth() const = 0;
	virtual int LowResHeight() const = 0;
	virtual ImageFormat LowResFormat() const = 0;

	// NOTE: reflectivity[0] = blue, [1] = green, [2] = red
	virtual const Vector &Reflectivity() const = 0;

	virtual bool IsCubeMap() const = 0;
	virtual bool IsNormalMap() const = 0;
	virtual bool IsVolumeTexture() const = 0;

	// Computes the dimensions of a particular mip level
	virtual void ComputeMipLevelDimensions( int iMipLevel, int *pMipWidth, int *pMipHeight, int *pMipDepth ) const = 0;

	// Computes the size (in bytes) of a single mipmap of a single face of a single frame 
	virtual int ComputeMipSize( int iMipLevel ) const = 0;

	// Computes the size of a subrect (specified at the top mip level) at a particular lower mip level
	virtual void ComputeMipLevelSubRect( Rect_t* pSrcRect, int nMipLevel, Rect_t *pSubRect ) const = 0;

	// Computes the size (in bytes) of a single face of a single frame
	// All mip levels starting at the specified mip level are included
	virtual int ComputeFaceSize( int iStartingMipLevel = 0 ) const = 0;

	// Computes the total size (in bytes) of all faces, all frames
	virtual int ComputeTotalSize() const = 0;

	// Returns the base address of the image data
	virtual unsigned char *ImageData() = 0;

	// Returns a pointer to the data associated with a particular frame, face, and mip level
	virtual unsigned char *ImageData( int iFrame, int iFace, int iMipLevel ) = 0;

	// Returns a pointer to the data associated with a particular frame, face, mip level, and offset
	virtual unsigned char *ImageData( int iFrame, int iFace, int iMipLevel, int x, int y, int z = 0 ) = 0;

	// Returns the base address of the low-res image data
	virtual unsigned char *LowResImageData() = 0;

	// Converts the textures image format. Use IMAGE_FORMAT_DEFAULT
	// if you want to be able to use various tool functions below
	virtual	void ConvertImageFormat( ImageFormat fmt, bool bNormalToDUDV ) = 0;

	// NOTE: The following methods only work on textures using the
	// IMAGE_FORMAT_DEFAULT!

	// Generate spheremap based on the current cube faces (only works for cubemaps)
	// The look dir indicates the direction of the center of the sphere
	// NOTE: Only call this *after* cube faces have been correctly
	// oriented (using FixCubemapFaceOrientation)
	virtual void GenerateSpheremap( LookDir_t lookDir = LOOK_DOWN_Z ) = 0;

	// Generate spheremap based on the current cube faces (only works for cubemaps)
	// The look dir indicates the direction of the center of the sphere
	// NOTE: Only call this *after* cube faces have been correctly
	// oriented (using FixCubemapFaceOrientation)
	virtual void GenerateHemisphereMap( unsigned char *pSphereMapBitsRGBA, int targetWidth, 
		int targetHeight, LookDir_t lookDir, int iFrame ) = 0;

	// Fixes the cubemap faces orientation from our standard to the
	// standard the material system needs.
	virtual void FixCubemapFaceOrientation( ) = 0;

	// Generates mipmaps from the base mip levels
	virtual void GenerateMipmaps() = 0;

	// Put 1/miplevel (1..n) into alpha.
	virtual void PutOneOverMipLevelInAlpha() = 0;
	
	// Computes the reflectivity
	virtual void ComputeReflectivity( ) = 0;

	// Computes the alpha flags
	virtual void ComputeAlphaFlags() = 0;

	// Generate the low-res image bits
	virtual bool ConstructLowResImage() = 0;

	// Gets the texture all internally consistent assuming you've loaded
	// mip 0 of all faces of all frames
	virtual void PostProcess(bool bGenerateSpheremap, LookDir_t lookDir = LOOK_DOWN_Z, bool bAllowFixCubemapOrientation = true) = 0;

	// Blends adjacent pixels on cubemap borders, since the card doesn't do it. If the texture
	// is S3TC compressed, then it has to do it AFTER the texture has been compressed to prevent
	// artifacts along the edges.
	//
	// If bSkybox is true, it assumes the faces are oriented in the way the engine draws the skybox
	// (which happens to be different from the way cubemaps have their faces).
	virtual void MatchCubeMapBorders( int iStage, ImageFormat finalFormat, bool bSkybox ) = 0;

	// Sets threshold values for alphatest mipmapping
	virtual void SetAlphaTestThreshholds( float flBase, float flHighFreq ) = 0;

	// Sets post-processing flags (settings are copied, pointer passed to distinguish between structure versions)
	virtual void SetPostProcessingSettings( VtfProcessingOptions const *pOptions ) = 0;
};

//-----------------------------------------------------------------------------
// Class factory
//-----------------------------------------------------------------------------
IVTFTexture *CreateVTFTexture();
void DestroyVTFTexture( IVTFTexture *pTexture );

//-----------------------------------------------------------------------------
// Allows us to only load in the first little bit of the VTF file to get info
// Clients should read this much into a UtlBuffer and then pass it in to
// Unserialize
//-----------------------------------------------------------------------------
int VTFFileHeaderSize( int nMajorVersion = -1, int nMinorVersion = -1 );

//-----------------------------------------------------------------------------
// 360 Conversion
//-----------------------------------------------------------------------------
typedef bool (*CompressFunc_t)( CUtlBuffer &inputBuffer, CUtlBuffer &outputBuffer );
bool ConvertVTFTo360Format( const char *pDebugName, CUtlBuffer &sourceBuf, CUtlBuffer &targetBuf, CompressFunc_t pCompressFunc );

//-----------------------------------------------------------------------------
// 360 Preload
//-----------------------------------------------------------------------------
bool GetVTFPreload360Data( const char *pDebugName, CUtlBuffer &fileBufferIn, CUtlBuffer &preloadBufferOut );

//#include "mathlib/vector.h"

#endif // VTF_FILE_FORMAT_ONLY

//-----------------------------------------------------------------------------
// Disk format for VTF files ver. 7.2 and earlier
//
// NOTE: After the header is the low-res image data
// Then follows image data, which is sorted in the following manner
//
//	for each mip level (starting with 1x1, and getting larger)
//		for each animation frame
//			for each face
//				store the image data for the face
//
// NOTE: In memory, we store the data in the following manner:
//	for each animation frame
//		for each face
//			for each mip level (starting with the largest, and getting smaller)
//				store the image data for the face
//
// This is done because the various image manipulation function we have
// expect this format
//-----------------------------------------------------------------------------
// Disk format for VTF files ver. 7.3
//
// NOTE: After the header is the array of ResourceEntryInfo structures,
// number of elements in the array is defined by "numResources".
// there are entries for:
//		eRsrcLowResImage	=	low-res image data
//		eRsrcSheet			=	sheet data
//		eRsrcImage			=	image data
// {
//	for each mip level (starting with 1x1, and getting larger)
//		for each animation frame
//			for each face
//				store the image data for the face
//
// NOTE: In memory, we store the data in the following manner:
//	for each animation frame
//		for each face
//			for each mip level (starting with the largest, and getting smaller)
//				store the image data for the face
// }
//
//-----------------------------------------------------------------------------


//#include "datamap.h"

// more manual definition
struct inputdata_t;
typedef void (*inputfunc_t)(inputdata_t& data);
enum
{
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,

	// Must be last
	TD_OFFSET_COUNT,
};
typedef enum _fieldtypes
{
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions

	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t *

	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)

	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)

	// NOTE: Use float arrays for local transformations that don't need to be fixed up.
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

	FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)

	FIELD_VECTOR2D,			// 2 floats

	FIELD_TYPECOUNT,		// MUST BE LAST
} fieldtype_t;
struct datamap_t;
struct typedescription_t
{
	fieldtype_t			fieldType;
	const char* fieldName;
	int					fieldOffset[TD_OFFSET_COUNT]; // 0 == normal, 1 == packed offset
	unsigned short		fieldSize;
	short				flags;
	// the name of the variable in the map/fgd data, or the name of the action
	const char* externalName;
	// pointer to the function set for save/restoring of custom data types
	void* pSaveRestoreOps;
	// for associating function with string names
	inputfunc_t			inputFunc;
	// For embedding additional datatables inside this one
	datamap_t* td;

	// Stores the actual member variable size in bytes
	int					fieldSizeInBytes;

	// FTYPEDESC_OVERRIDE point to first baseclass instance if chains_validated has occurred
	struct typedescription_t* override_field;

	// Used to track exclusion of baseclass fields
	int					override_count;

	// Tolerance for field errors for float fields
	float				fieldTolerance;
};
struct datamap_t
{
	typedescription_t* dataDesc;
	int					dataNumFields;
	char const* dataClassName;
	datamap_t* baseMap;

	bool				chains_validated;
	// Have the "packed" offsets been computed
	bool				packed_offsets_computed;
	int					packed_size;

#if defined( _DEBUG )
	bool				bValidityChecked;
#endif // _DEBUG
};

#define DECLARE_SIMPLE_DATADESC() \
	static datamap_t m_DataMap; \
	static datamap_t *GetBaseMap(); \
	template <typename T> friend void DataMapAccess(T *, datamap_t **p); \
	template <typename T> friend datamap_t *DataMapInit(T *);
#define DECLARE_BYTESWAP_DATADESC() DECLARE_SIMPLE_DATADESC()

// lazy lol
class __declspec(align(16)) VectorAligned {
	float x, y, z, w;
};

#pragma pack(1)

// version number for the disk texture cache
#define VTF_MAJOR_VERSION 7
#define VTF_MINOR_VERSION 4

//-----------------------------------------------------------------------------
// !!!!CRITICAL!!!! BEFORE YOU CHANGE THE FORMAT
//
// The structure sizes ARE NOT what they appear, regardless of Pack(1).
// The "VectorAligned" causes invisible padding in the FINAL derived structure.
// 
// Each VTF format has been silently plagued by this.
//
// LOOK AT A 7.3 FILE. The 7.3 structure ends at 0x48 as you would expect by
// counting structure bytes. But, the "Infos" start at 0x50! because the PC
// compiler pads, the 360 compiler does NOT.
//-----------------------------------------------------------------------------

struct VTFFileBaseHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	char fileTypeString[4]; // "VTF" Valve texture file
	int version[2]; 		// version[0].version[1]
	int headerSize;
};

struct VTFFileHeaderV7_1_t : public VTFFileBaseHeader_t 
{
	DECLARE_BYTESWAP_DATADESC();
	unsigned short	width;
	unsigned short	height;
	unsigned int	flags;
	unsigned short	numFrames;
	unsigned short	startFrame;
#if !defined( POSIX )
	VectorAligned	reflectivity;
#else
	// must manually align in order to maintain pack(1) expected layout with existing binaries
	char			pad1[4];
	Vector			reflectivity;
	char			pad2[4];
#endif
	float			bumpScale;
	ImageFormat		imageFormat;
	unsigned char	numMipLevels;
	ImageFormat		lowResImageFormat;
	unsigned char	lowResImageWidth;
	unsigned char	lowResImageHeight;	
};

struct VTFFileHeaderV7_2_t : public VTFFileHeaderV7_1_t
{
	DECLARE_BYTESWAP_DATADESC();

	unsigned short depth;
};

#define BYTE_POS( byteVal, shft )	uint32( uint32(uint8(byteVal)) << uint8(shft * 8) )
#define MK_VTF_RSRC_ID(a, b, c)		uint32( BYTE_POS(a, 0) | BYTE_POS(b, 1) | BYTE_POS(c, 2) )
#define MK_VTF_RSRCF(d)				BYTE_POS(d, 3)

// Special section for stock resources types
enum ResourceEntryType
{
	// Legacy stock resources, reading/writing are handled differently (i.e. they do not have the length tag word!)
	VTF_LEGACY_RSRC_LOW_RES_IMAGE	= MK_VTF_RSRC_ID( 0x01, 0, 0 ),	// Low-res image data
	VTF_LEGACY_RSRC_IMAGE			= MK_VTF_RSRC_ID( 0x30, 0, 0 ),	// Image data

	// New extended resource
	VTF_RSRC_SHEET = MK_VTF_RSRC_ID( 0x10, 0, 0 ),			// Sheet data
};

// Bytes with special meaning when set in a resource type
enum ResourceEntryTypeFlag
{
	RSRCF_HAS_NO_DATA_CHUNK	= MK_VTF_RSRCF( 0x02 ),	// Resource doesn't have a corresponding data chunk
	RSRCF_MASK				= MK_VTF_RSRCF( 0xFF )	// Mask for all the flags
};

// Header details constants
enum HeaderDetails
{
	MAX_RSRC_DICTIONARY_ENTRIES = 32,		// Max number of resources in dictionary
	MAX_X360_RSRC_DICTIONARY_ENTRIES = 4,	// 360 needs this to be slim, otherwise preload size suffers
};

struct ResourceEntryInfo
{
	union
	{ 
		unsigned int	eType;		// Use MK_VTF_??? macros to be endian compliant with the type
		unsigned char	chTypeBytes[4];
	};
	unsigned int		resData;	// Resource data or offset from the beginning of the file
};

struct VTFFileHeaderV7_3_t : public VTFFileHeaderV7_2_t
{
	DECLARE_BYTESWAP_DATADESC();

	char			pad4[3];
	unsigned int	numResources;

#if defined( POSIX )
	// must manually align in order to maintain pack(1) expected layout with existing binaries
	char			pad5[8];
#endif
	
	// AFTER THE IMPLICIT PADDING CAUSED BY THE COMPILER....
	// *** followed by *** ResourceEntryInfo resources[0];
	// Array of resource entry infos sorted ascending by type
};

struct VTFFileHeader_t : public VTFFileHeaderV7_3_t
{
	DECLARE_BYTESWAP_DATADESC();
};

///////////////////////////
//  Resource Extensions  //
///////////////////////////

// extended texture lod control:
#define VTF_RSRC_TEXTURE_LOD_SETTINGS ( MK_VTF_RSRC_ID( 'L','O','D' ) )
struct TextureLODControlSettings_t
{
	// What to clamp the dimensions to, mip-map wise, when at picmip 0. keeps texture from
	// exceeding (1<<m_ResolutionClamp) at picmip 0.  at picmip 1, it won't exceed
	// (1<<(m_ResolutionClamp-1)), etc.
	uint8 m_ResolutionClampX;
	uint8 m_ResolutionClampY;

	uint8 m_ResolutionClampX_360;
	uint8 m_ResolutionClampY_360;
};

// Extended flags and settings:
#define VTF_RSRC_TEXTURE_SETTINGS_EX ( MK_VTF_RSRC_ID( 'T','S','0' ) )
struct TextureSettingsEx_t
{
	enum Flags0			// flags0 byte mask
	{
		UNUSED = 0x01,
	};

	uint8 m_flags0;		// a bitwise combination of Flags0
	uint8 m_flags1;		// set to zero. for future expansion.
	uint8 m_flags2;		// set to zero. for future expansion.
	uint8 m_flags3;		// set to zero. for future expansion.
};

#define VTF_RSRC_TEXTURE_CRC ( MK_VTF_RSRC_ID( 'C','R','C' ) )

#pragma pack()

#endif // VTF_H
