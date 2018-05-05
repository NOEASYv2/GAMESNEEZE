#pragma once

#include "IMaterial.h"


namespace sdk
{
	struct model_t;
	struct mstudioanimdesc_t {};
	struct mstudioseqdesc_t {};

	enum HitboxList
	{
		HITBOX_PELVIS,
		HITBOX_L_THIGH,
		HITBOX_L_CALF,
		HITBOX_L_FOOT,
		HITBOX_R_THIGH,
		HITBOX_R_CALF,
		HITBOX_R_FOOT,
		HITBOX_SPINE1,
		HITBOX_SPINE2,
		HITBOX_SPINE3,
		HITBOX_NECK,
		HITBOX_HEAD,
		HITBOX_L_UPPERARM,
		HITBOX_L_FOREARM,
		HITBOX_L_HAND,
		HITBOX_R_UPPERARM,
		HITBOX_R_FOREARM,
		HITBOX_R_HAND,
		HITBOX_L_CLAVICLE,
		HITBOX_R_CLAVICLE,
		HITBOX_SPINE4,
		HITBOX_MAX,
	};
	struct mstudiobbox_t
	{
		int					bone;
		int					group;
		Vector				bbmin;
		Vector				bbmax;
		int					szhitboxnameindex;
		int					m_iPad01[3];
		float				m_flRadius;
		int					m_iPad02[4];
	};
	/*struct mstudiobone_t
	{
		int					sznameindex;
		inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
		int		 			parent;		// parent bone
		int					bonecontroller[6];	// bone controller index, -1 == none

												// default values
		Vector				pos;
		Quaternion			quat;
		Vector			rot;
		// compression scale
		Vector				posscale;
		Vector				rotscale;

		VMatrix			poseToBone;
		Quaternion			qAlignment;
		int					flags;
		int					proctype;
		int					procindex;		// procedural rule
		mutable int			physicsbone;	// index into physically simulated bone
		inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
		int					surfacepropidx;	// index into string tablefor property name
		inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
		int					contents;		// See BSPFlags.h for the contents flags

		int					unused[8];		// remove as appropriate

		mstudiobone_t() {}
	private:
		// No copy constructors allowed
		mstudiobone_t(const mstudiobone_t& vOther);
	};*/
	struct mstudiobone_t
	{
		int					sznameindex;
		inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
		int		 			parent;		// parent bone
		int					bonecontroller[6];	// bone controller index, -1 == none

												// default values
		Vector				pos;
		Quaternion			quat;
		RadianEuler			rot;
		// compression scale
		Vector				posscale;
		Vector				rotscale;

		matrix3x4_t			poseToBone;
		Quaternion			qAlignment;
		int					flags;
		int					proctype;
		int					procindex;		// procedural rule
		mutable int			physicsbone;	// index into physically simulated bone
		inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
		int					surfacepropidx;	// index into string tablefor property name
		inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
		int					contents;		// See BSPFlags.h for the contents flags

		int					unused[8];		// remove as appropriate

		mstudiobone_t() {}
	private:
		// No copy constructors allowed
		mstudiobone_t(const mstudiobone_t& vOther);
	};
	/*struct mstudiohitboxset_t
	{
		int                                             sznameindex;
		inline char* const              GetName(void) const { return ((char*)this) + sznameindex; }
		int                                             numhitboxes;
		int                                             hitboxindex;
		inline mstudiobbox_t *GetHitbox(int i) const { return (mstudiobbox_t*)(((unsigned char*)this) + hitboxindex) + i; };
	};*/
	struct mstudiohitboxset_t
	{
		int	sznameindex;
		int	numhitboxes;
		int	hitboxindex;

		inline mstudiobbox_t* GetHitbox(int i) const
		{
			return (mstudiobbox_t*)(((DWORD)this) + hitboxindex) + i;
		};
	};

	/*struct studiohdr_t
	{
		int                                     id;
		int                                     version;

		int                                     checksum;

		char                            name[64];
		int                                     length;


		Vector                          eyeposition;

		Vector                          illumposition;

		Vector                          hull_min;
		Vector                          hull_max;

		Vector                          view_bbmin;
		Vector                          view_bbmax;

		int                                     flags;

		int                                     numbones;
		int                                     boneindex;

		inline mstudiobone_t *GetBone(int i) const { return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
		//	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

		int                                     numbonecontrollers;
		int                                     bonecontrollerindex;

		int                                     numhitboxsets;
		int                                     hitboxsetindex;

		/*mstudiohitboxset_t* GetHitboxSet(int i) const
		{
			return (mstudiohitboxset_t*)(((byte*)this) + hitboxsetindex) + i;
		}
		// Look up hitbox set by index
		mstudiohitboxset_t	*pHitboxSet(int i) const
		{
			Assert(i >= 0 && i < numhitboxsets);
			return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex) + i;
		};
		inline mstudiobbox_t* GetHitbox(int i, int set) const
		{
			mstudiohitboxset_t const* s = GetHitboxSet(set);

			if (!s)
				return NULL;

			return s->GetHitbox(i);
		}

		inline int GetHitboxCount(int set) const
		{
			mstudiohitboxset_t const* s = GetHitboxSet(set);

			if (!s)
				return 0;

			return s->numhitboxes;
		}*//*
		mstudiohitboxset_t	*GetHitboxSet(int i) const //phitboxset
		{
			Assert(i >= 0 && i < numhitboxsets);
			return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex) + i;
		};

		// Calls through to hitbox to determine size of specified set
		inline mstudiobbox_t *GetHitbox(int i, int set) const
		{
			mstudiohitboxset_t const *s = GetHitboxSet(set);
			if (!s)
				return NULL;

			return s->GetHitbox(i);
		};

		// Calls through to set to get hitbox count for set
		inline int			iHitboxCount(int set) const
		{
			mstudiohitboxset_t const *s = GetHitboxSet(set);
			if (!s)
				return 0;

			return s->numhitboxes;
		};

		int                                     numlocalanim;
		int                                     localanimindex;

		int                                     numlocalseq;
		int                                     localseqindex;

		mutable int                     activitylistversion;
		mutable int                     eventsindexed;

		int                                     numtextures;
		int                                     textureindex;

		int                                     numcdtextures;
		int                                     cdtextureindex;

		int                                     numskinref;
		int                                     numskinfamilies;
		int                                     skinindex;

		int                                     numbodyparts;
		int                                     bodypartindex;

		int                                     numlocalattachments;
		int                                     localattachmentindex;

		int                                     numlocalnodes;
		int                                     localnodeindex;
		int                                     localnodenameindex;

		int                                     numflexdesc;
		int                                     flexdescindex;

		int                                     numflexcontrollers;
		int                                     flexcontrollerindex;

		int                                     numflexrules;
		int                                     flexruleindex;

		int                                     numikchains;
		int                                     ikchainindex;

		int                                     nummouths;
		int                                     mouthindex;

		int                                     numlocalposeparameters;
		int                                     localposeparamindex;

		int                                     surfacepropindex;

		int                                     keyvalueindex;
		int                                     keyvaluesize;


		int                                     numlocalikautoplaylocks;
		int                                     localikautoplaylockindex;

		float                           mass;
		int                                     contents;

		int                                     numincludemodels;
		int                                     includemodelindex;

		mutable void            *virtualModel;

		int                                     szanimblocknameindex;
		int                                     numanimblocks;
		int                                     animblockindex;

		mutable void            *animblockModel;

		int                                     bonetablebynameindex;

		void                            *pVertexBase;
		void                            *pIndexBase;

		byte                            constdirectionallightdot;

		byte                            rootLOD;

		byte                            numAllowedRootLODs;

		byte                            unused[1];

		int                                     unused4;

		int                                     numflexcontrollerui;
		int                                     flexcontrolleruiindex;
		float                           flVertAnimFixedPointScale;
		int                                     unused3[1];
		int                                     studiohdr2index;
		int                                     unused2[1];
	};*/

struct studiohdr_t
{
	int                                     id;
	int                                     version;

	int                                     checksum;

	char                            name[64];
	int                                     length;


	Vector                          eyeposition;

	Vector                          illumposition;

	Vector                          hull_min;
	Vector                          hull_max;

	Vector                          view_bbmin;
	Vector                          view_bbmax;

	int                                     flags;

	int                                     numbones;
	int                                     boneindex;

	inline mstudiobone_t *GetBone(int i) const { return (mstudiobone_t *)(((BYTE *)this) + boneindex) + i; };
	//	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int                                     numbonecontrollers;
	int                                     bonecontrollerindex;

	int                                     numhitboxsets;
	int                                     hitboxsetindex;

	mstudiohitboxset_t* GetHitboxSet(int i) const
	{
		return (mstudiohitboxset_t*)(((BYTE*)this) + hitboxsetindex) + i;
	}

	inline mstudiobbox_t* GetHitbox(int i, int set) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet(set);

		if (!s)
			return NULL;

		return s->GetHitbox(i);
	}

	inline int GetHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet(set);

		if (!s)
			return 0;

		return s->numhitboxes;
	}

	int                                     numlocalanim;
	int                                     localanimindex;

	int                                     numlocalseq;
	int                                     localseqindex;

	mutable int                     activitylistversion;
	mutable int                     eventsindexed;

	int                                     numtextures;
	int                                     textureindex;

	int                                     numcdtextures;
	int                                     cdtextureindex;

	int                                     numskinref;
	int                                     numskinfamilies;
	int                                     skinindex;

	int                                     numbodyparts;
	int                                     bodypartindex;

	int                                     numlocalattachments;
	int                                     localattachmentindex;

	int                                     numlocalnodes;
	int                                     localnodeindex;
	int                                     localnodenameindex;

	int                                     numflexdesc;
	int                                     flexdescindex;

	int                                     numflexcontrollers;
	int                                     flexcontrollerindex;

	int                                     numflexrules;
	int                                     flexruleindex;

	int                                     numikchains;
	int                                     ikchainindex;

	int                                     nummouths;
	int                                     mouthindex;

	int                                     numlocalposeparameters;
	int                                     localposeparamindex;

	int                                     surfacepropindex;

	int                                     keyvalueindex;
	int                                     keyvaluesize;


	int                                     numlocalikautoplaylocks;
	int                                     localikautoplaylockindex;

	float                           mass;
	int                                     contents;

	int                                     numincludemodels;
	int                                     includemodelindex;

	mutable void            *virtualModel;

	int                                     szanimblocknameindex;
	int                                     numanimblocks;
	int                                     animblockindex;

	mutable void            *animblockModel;

	int                                     bonetablebynameindex;

	void                            *pVertexBase;
	void                            *pIndexBase;

	BYTE                            constdirectionallightdot;

	BYTE                            rootLOD;

	BYTE                            numAllowedRootLODs;

	BYTE                            unused[1];

	int                                     unused4;

	int                                     numflexcontrollerui;
	int                                     flexcontrolleruiindex;
	float                           flVertAnimFixedPointScale;
	int                                     unused3[1];
	int                                     studiohdr2index;
	int                                     unused2[1];
};
	//0x3F00FB33
	class CModelInfo
	{
	public:
		studiohdr_t* GetStudioModel(const model_t* Model)
		{
			typedef studiohdr_t*(__thiscall* Fn)(void*, const model_t*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 32)(this, Model);
		}

		char* GetModelName(const model_t *Model)
		{
			typedef char*(__thiscall* Fn)(void*, const model_t*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 3)(this, Model);
		}

		void GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterial)
		{
			typedef char*(__thiscall* Fn)(void*, const model_t*, int, IMaterial**);
			VMT::VMTHookManager::GetFunction<Fn>(this, 17)(this, model, count, ppMaterial);
		}
	};
}