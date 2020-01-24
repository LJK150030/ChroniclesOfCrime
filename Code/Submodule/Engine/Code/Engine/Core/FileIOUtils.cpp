#include "Engine/Core/FileIOUtils.hpp"
#include <fstream>
#include "ErrorWarningAssert.hpp"

unsigned long CreateBufferForFile(char const* file_name, char** out_data)
{
	std::ifstream file_stream;
	//Open the file in binary and at the end
	//This tells us where we are in the file and. 
	//We open in binary so it doesn't fuck with carriage returns
	//We open at end of file so it's easier to get the file size
	file_stream.open(file_name, std::ios::binary | std::ios::ate);

	if (file_stream.is_open())
	{
		//The file is open

		//tellg is an int telling us how far into the file we are. Since we are in the end we can assume this returns the file size
		const unsigned long file_size = (unsigned long)file_stream.tellg();

		//reseting the cursour to the start of the file
		file_stream.seekg(std::ios::beg);

		*out_data = new char[file_size];
		file_stream.read(*out_data, file_size);

		file_stream.close();
		return file_size;
	}
	//We didn't open the file. Maybe the file doesn't exist
	//Don't assert the user because we could be opening a save file and we may not need to warn the user

	//Note: Remove this later when you don't need the assert anymore
	GUARANTEE_RECOVERABLE(true, "The file did not exist when creating File Buffer");
	return 0;
}
