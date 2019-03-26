#pragma once
#include <vector>

class File
{
public:
	std::string path;
	std::vector<std::string> file;

	File(std::string path, std::vector<std::string> parameters = {});
	static void makeFile(std::string path, std::vector <std::vector<std::string>> fileVector);

private:
	static std::vector<std::vector<std::string>> forOperations(std::string forLine);
	static unsigned long long hexToDec(std::string hexValue);
	static std::string decToHex(unsigned long long decValue);
};

