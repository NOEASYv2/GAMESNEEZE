#pragma once

#include "../utilities/CUtlVector.h"

namespace sdk
{
	enum class EGlowStyle
	{
		/// <summary> 
		/// Default outline glow effect.
		/// </summary>
		Outlined = 0,
		/// <summary>
		/// Glowing model. 
		/// </summary>
		Model,
		/// <summary>
		/// Inline glow effect. 
		/// </summary>
		Inline,
		/// <summary> 
		/// Inline pulsating glow effect.
		/// </summary>
		Inline_Pulsate
	};
	class CGlowObjectDefinition {
	public:
		uint32_t get_entity( ) {
			return baseEntity;
		}
		bool IsUnused( ) const {
			return nextFreeSlot != ENTRY_IN_USE;
		}
	public:
		uint32_t baseEntity;           /// 0x00
		float    red;                  /// 0x04
		float    green;                /// 0x08
		float    blue;                 /// 0x0C
		float    alpha;                /// 0x10
	private:
		uint8_t  _pad0014 [ 0x10 ];     /// 0x14
	public:
		bool     renderWhenOccluded;   /// 0x24
		bool     renderWhenUnoccluded; /// 0x25
		bool     fullBloomRender;      /// 0x26
	private:
		uint8_t  __pad0027 [ 0x5 ];     /// 0x27
	public:
		int32_t  style;                /// 0x2C
		int32_t  splitScreenSlot;      /// 0x30
		int32_t  nextFreeSlot;         /// 0x34

	public:
		static constexpr int32_t END_OF_FREE_LIST = -1;
		static constexpr int32_t ENTRY_IN_USE = -2;
	}; /// Size=0x38


	class CGlowObjectManager
	{
	public:
		CGlowObjectDefinition* m_GlowObjectDefinitions;

		int GetSize()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + 0xC);
		}

	private:
	};
}