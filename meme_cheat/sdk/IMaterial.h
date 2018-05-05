#pragma once

namespace sdk
{

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
	//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

	typedef unsigned short MaterialHandle_t;

	class KeyValues 
	{
	public:
		void *operator new(size_t iAllocSize)
		{
			static PVOID pKeyValuesSystem;
			if (!pKeyValuesSystem)
			{
				typedef PVOID(__cdecl* oKeyValuesSystem)();
				oKeyValuesSystem KeyValuesSystem = (oKeyValuesSystem)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "KeyValuesSystem");

				pKeyValuesSystem = KeyValuesSystem();
			}

			typedef PVOID(__thiscall* oAllocKeyValuesMemory)(PVOID, size_t);
			return VMT::VMTHookManager::GetFunction<oAllocKeyValuesMemory>(pKeyValuesSystem, 1)(pKeyValuesSystem, iAllocSize);
		}

		void operator delete(void * pMem)
		{
			static PVOID pKeyValuesSystem;
			if (!pKeyValuesSystem)
			{
				typedef PVOID(__cdecl* oKeyValuesSystem)();
				oKeyValuesSystem KeyValuesSystem = (oKeyValuesSystem)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "KeyValuesSystem");

				pKeyValuesSystem = KeyValuesSystem();
			}

			typedef void(__thiscall* oFreeKeyValuesMemory)(PVOID, PVOID);
			VMT::VMTHookManager::GetFunction<oFreeKeyValuesMemory>(pKeyValuesSystem, 2)(pKeyValuesSystem, pMem);
		}
		KeyValues() {}
		char pad[0x20];
	};
	enum MaterialVarFlags_t
	{
		MATERIAL_VAR_DEBUG = (1 << 0),
		MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
		MATERIAL_VAR_NO_DRAW = (1 << 2),
		MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

		MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
		MATERIAL_VAR_VERTEXALPHA = (1 << 5),
		MATERIAL_VAR_SELFILLUM = (1 << 6),
		MATERIAL_VAR_ADDITIVE = (1 << 7),
		MATERIAL_VAR_ALPHATEST = (1 << 8),
		//	MATERIAL_VAR_UNUSED					  = (1 << 9),
		MATERIAL_VAR_ZNEARER = (1 << 10),
		MATERIAL_VAR_MODEL = (1 << 11),
		MATERIAL_VAR_FLAT = (1 << 12),
		MATERIAL_VAR_NOCULL = (1 << 13),
		MATERIAL_VAR_NOFOG = (1 << 14),
		MATERIAL_VAR_IGNOREZ = (1 << 15),
		MATERIAL_VAR_DECAL = (1 << 16),
		MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
											   //	MATERIAL_VAR_UNUSED					  = (1 << 18),
											   MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
											   MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
											   MATERIAL_VAR_TRANSLUCENT = (1 << 21),
											   MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
											   MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
											   MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
											   MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
											   MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
											   MATERIAL_VAR_HALFLAMBERT = (1 << 27),
											   MATERIAL_VAR_WIREFRAME = (1 << 28),
											   MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
											   MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
											   MATERIAL_VAR_VERTEXFOG = (1 << 31),

											   // NOTE: Only add flags here that either should be read from
											   // .vmts or can be set directly from client code. Other, internal
											   // flags should to into the flag enum in IMaterialInternal.h
	};

	struct MaterialVideoMode_t {
		int m_Width;
		int m_Height;
		int m_Format;
		int m_RefreshRate;
	};
	struct MaterialSystem_Config_t
	{
		MaterialVideoMode_t m_VideoMode;
		float m_fMonitorGamma;
		float m_fGammaTVRangeMin;
		float m_fGammaTVRangeMax;
		float m_fGammaTVExponent;
		bool m_bGammaTVEnabled;
		bool m_bTripleBuffered;
		int m_nAASamples;
		int m_nForceAnisotropicLevel;
		int m_nSkipMipLevels;
		int m_nDxSupportLevel;
		int m_nFlags;
		bool m_bEditMode;
		char m_nProxiesTestMode;
		bool m_bCompressedTextures; // nothin
		bool m_bFilterLightmaps;
		bool m_bFilterTextures; // nothin
		bool m_bReverseDepth; // looks like asus walls
		bool m_bBufferPrimitives;
		bool m_bDrawFlat;
		bool m_bMeasureFillRate;
		bool m_bVisualizeFillRate; // red everywhere not usable
		bool m_bNoTransparency;
		bool m_bSoftwareLighting;
		bool m_bAllowCheats;
		char m_nShowMipLevels; // rainbows EVERYWHERE :O
		bool m_bShowLowResImage; // minecraft mode
		bool m_bShowNormalMap; // nothin too
		bool m_bMipMapTextures; // nothin too 
		char m_nFullbright; // good for fps
		bool m_bFastNoBump;
		bool m_bSuppressRendering;
		bool m_bDrawGray; // gray walls
		bool m_bShowSpecular; // nothin too
		bool m_bShowDiffuse;
		int m_nWindowedSizeLimitWidth;
		int m_nWindowedSizeLimitHeight;
		int m_nAAQuality;
		bool m_bShadowDepthTexture; // nothing afaik
		bool m_bMotionBlur;
		bool m_bSupportFlashlight; // nothin afaik
		bool m_bPaintEnabled;
		char pad[0xC];
	};

	enum PreviewImageRetVal_t
	{
		MATERIAL_PREVIEW_IMAGE_BAD = 0,
		MATERIAL_PREVIEW_IMAGE_OK,
		MATERIAL_NO_PREVIEW_IMAGE,
	};

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
		IMAGE_FORMAT_RGB323232F,	// NOTE: D3D9 does not have this format
		IMAGE_FORMAT_RGBA32323232F,
		IMAGE_FORMAT_RG1616F,
		IMAGE_FORMAT_RG3232F,
		IMAGE_FORMAT_RGBX8888,

		IMAGE_FORMAT_NULL,			// Dummy format which takes no video memory

		// Compressed normal map formats
		IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
		IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

		IMAGE_FORMAT_RGBA1010102,	// 10 bit-per component render targets
		IMAGE_FORMAT_BGRA1010102,
		IMAGE_FORMAT_R16F,			// 16 bit FP format

		// Depth-stencil texture formats
		IMAGE_FORMAT_D16,
		IMAGE_FORMAT_D15S1,
		IMAGE_FORMAT_D32,
		IMAGE_FORMAT_D24S8,
		IMAGE_FORMAT_LINEAR_D24S8,
		IMAGE_FORMAT_D24X8,
		IMAGE_FORMAT_D24X4S4,
		IMAGE_FORMAT_D24FS8,
		IMAGE_FORMAT_D16_SHADOW,	// Specific formats for shadow mapping
		IMAGE_FORMAT_D24X8_SHADOW,	// Specific formats for shadow mapping

		// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
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

		NUM_IMAGE_FORMATS
	};

	class IMaterial
	{
	public:
		void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, MaterialVarFlags_t, bool);
			return VMT::VMTHookManager::GetFunction<OriginalFn>(this, 29)(this, flag, on);
		}
		void AlphaModulate(float alpha)
		{
			typedef void(__thiscall* Fn)(void*, float);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 27)(this, alpha);
		}
		void ColorModulate(float r, float g, float b)
		{
			typedef void(__thiscall* Fn)(void*, float, float, float);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 28)(this, r, g, b);
		}
		void ColorModulate(CColor color)
		{
			typedef void(__thiscall* Fn)(void*, float, float, float);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 28)(this, color.RGBA[0] / 255.f, color.RGBA[1] / 255.f, color.RGBA[2] / 255.f);
		}
		const char* GetTextureGroupName()
		{
			typedef const char*(__thiscall* oGetTextureGroupName)(void*);
			return VMT::VMTHookManager::GetFunction<oGetTextureGroupName>(this, 1)(this);
		}
		const char* GetMaterialName()
		{
			typedef const char*(__thiscall* oGetName)(void*);
			return VMT::VMTHookManager::GetFunction<oGetName>(this, 0)(this);
		}
		void SetShader(const char *pShaderName)
		{
			typedef void(__thiscall* SetShaderFn)(void*, const char *pShaderName);
			VMT::VMTHookManager::GetFunction<SetShaderFn>(this, 34)(this, pShaderName);
		}
		void IncrementReferenceCount()
		{
			typedef void(__thiscall *Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 12)(this);
		}
		void DecrementReferenceCount()
		{
			typedef void(__thiscall *Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 13)(this);
		}
		float GetAlphaModulation()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 44)(this);
		}
		void GetColorModulation(float* red, float* green, float* blue)
		{
			///45
			typedef void(__thiscall* Fn)(void*, float*, float*, float*);
			VMT::VMTHookManager::GetFunction<Fn>(this, 45)(this, red, green, blue);
		}
	};
	class IMaterialSystem // works
	{
	public:

		IMaterial* CreateMaterial(const char* c, KeyValues* k)
		{
			typedef IMaterial*(__thiscall* tCreateMaterial)(PVOID, const char*, KeyValues*);
			return VMT::VMTHookManager::GetFunction<tCreateMaterial>(this, 83)(this, c, k);
		}
		IMaterial* FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
		{
			typedef IMaterial*(__thiscall* OriginalFn)(PVOID, char const*, const char*, bool, const char*);
			return VMT::VMTHookManager::GetFunction<OriginalFn>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
		}
		MaterialHandle_t FirstMaterial()
		{
			typedef MaterialHandle_t(__thiscall *oFirstMaterial)(void*);
			return VMT::VMTHookManager::GetFunction<oFirstMaterial>(this, 86)(this);
		}

		MaterialHandle_t NextMaterial(MaterialHandle_t h)
		{
			typedef MaterialHandle_t(__thiscall *oNextMaterial)(void*, MaterialHandle_t);
			return VMT::VMTHookManager::GetFunction<oNextMaterial>(this, 87)(this, h);
		}

		MaterialHandle_t InvalidMaterial()
		{
			typedef MaterialHandle_t(__thiscall *oInvalidMaterial)(void*);
			return VMT::VMTHookManager::GetFunction<oInvalidMaterial>(this, 88)(this);
		}

		IMaterial* GetMaterial(MaterialHandle_t h)
		{
			typedef IMaterial*(__thiscall *oGetMaterial)(void*, MaterialHandle_t);
			return VMT::VMTHookManager::GetFunction<oGetMaterial>(this, 89)(this, h);
		}
	};


}