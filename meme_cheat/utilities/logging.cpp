#include "../../includes.h"


namespace UTILS
{
	void Log(float message)
	{
		Log(std::to_string(message));
	}

	void Log(std::string message)
	{
		CreateDirectory("C:\\Bameware Logs", NULL);

		char file_path[MAX_PATH];
		sprintf(file_path, "C:\\Bameware Logs\\%s.log", __DATE__);

		std::fstream file;

		static bool is_init = false;
		if (!is_init)
		{
			is_init = true;
			file.open(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		}
		else
			file.open(file_path, std::ios::out | std::ios::in | std::ios::app);



		if (!file.is_open())
		{
			file.close();
			return;
		}

		file << message << std::endl;
		file.close();
	}
}