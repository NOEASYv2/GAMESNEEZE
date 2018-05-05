#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/IMaterial.h"
#include "Chams.h"
#include <deque>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include "../../utilities/render.h"
#include "../../sdk/IVDebugOverlay.h"

void CChams::InitKeyValues(sdk::KeyValues* kv_, std::string name_)
{
	static auto address = UTILS::FindSignature("client.dll", "55 8B EC 51 33 C0 C7 45");
	using Fn = void(__thiscall*)(void* thisptr, const char* name);
	reinterpret_cast< Fn >(address)(kv_, name_.c_str());
}

void CChams::LoadFromBuffer(sdk::KeyValues* vk_, std::string name_, std::string buffer_)
{
	static auto address = UTILS::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
	using Fn = void(__thiscall*)(
		void* thisptr, const char* resourceName,
		const char* pBuffer, void* pFileSystem,
		const char* pPathID, void* pfnEvaluateSymbolProc);

	reinterpret_cast< Fn >(address)(
		vk_, name_.c_str(), buffer_.c_str(), nullptr, nullptr, nullptr);
}

sdk::IMaterial* CChams::CreateMaterial(bool ignorez, bool lit, bool wireframe)
{
	static auto created = 0;

	std::string type = lit ? "VertexLitGeneric" : "UnlitGeneric";

	auto matdata =
		"\"" + type + "\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"" + std::to_string(wireframe) + "\"\
        \n}\n";

	auto matname = "custom_" + std::to_string(created);
	++created;

	auto keyValues = static_cast< sdk::KeyValues* >(malloc(sizeof(sdk::KeyValues)));

	InitKeyValues(keyValues, type.c_str());
	LoadFromBuffer(keyValues, matname, matdata);

	auto material =
		ctx::material_system->CreateMaterial(matname.c_str(), keyValues);

	material->IncrementReferenceCount();
	return material;
}

sdk::IMaterial* CChams::CreateMaterial( std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert ) {
	static int number = 0;
	std::stringstream materialData;
	materialData << "\"" + type + "\"\n"
		"{\n"
		"\t\"$basetexture\" \"" + texture + "\"\n"
		"\t\"$ignorez\" \"" + std::to_string( ignorez ) + "\"\n"
		"\t\"$envmap\" \"" + "" + "\"\n"
		"\t\"$nofog\" \"" + std::to_string( nofog ) + "\"\n"
		"\t\"$model\" \"" + std::to_string( model ) + "\"\n"
		"\t\"$nocull\" \"" + std::to_string( nocull ) + "\"\n"
		"\t\"$selfillum\" \"" + "1" + "\"\n"
		"\t\"$halflambert\" \"" + std::to_string( halflambert ) + "\"\n"
		"\t\"$znearer\" \"" + "0" + "\"\n"
		"\t\"$flat\" \"" + "1" + "\"\n"
		"}\n" << std::flush;

	std::string materialName = "material_" + std::to_string( number ) + ".vmt";
	auto keyValues = static_cast< sdk::KeyValues* >( malloc( sizeof( sdk::KeyValues ) ) );
	number++;
	InitKeyValues( keyValues, type.c_str( ) );
	LoadFromBuffer( keyValues, materialName.c_str( ), materialData.str( ).c_str( ) );

	return ctx::material_system->CreateMaterial( materialName.c_str( ), keyValues );
}

CChams* chams = new CChams();