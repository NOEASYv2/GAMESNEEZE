#pragma once

class CChams
{
public:
	sdk::IMaterial *CreateMaterial(bool ignorez, bool lit, bool wireframe);
	sdk::IMaterial * CreateMaterial( std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert );

private:
	void InitKeyValues(sdk::KeyValues* keyValues, std::string name);
	void LoadFromBuffer(sdk::KeyValues* keyValues, std::string name_, std::string buffer_);
};

extern CChams* chams;