#include "pch.h"
#include "file.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>


File::File(std::string path, std::vector<std::string> parameters)
{
	//!!!!!!!!!!!!!!!!--- Zmienne sta³e do wyszukiwania ---!!!!!!!!!!!!!!!!
	std::string const searchingFor{ "%for " };
	std::string const searchingEndfor{ "%endfor" };
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::fstream file;
	std::string line;
	std::vector<std::string> file_result, for_content;
	std::vector<std::vector<std::string>> for_parameters;
	bool inside_for{ false };
	
	if (path == "") {

		do {
			std::cout << "File path: ";
			std::cin >> path;
				
			file.open(path.c_str(), std::ios::in);

			if(!file.good()){
				std::cout << "\t\t\tWrong file path!" << std::endl;
				file.close();
			}

		} while (!file.good());
	}
	else {
		file.open(path.c_str(), std::ios::in);
	}

	while (std::getline(file, line)) {

		if (!parameters.empty()) {
			for (int element = 1; element <= parameters.size(); ++element) {
				while (line.find("%(" + std::to_string(element) + ')') != std::string::npos) {
					line.replace(line.find("%(" + std::to_string(element) + ')'), 4, parameters[element - 1]);
				}
			}
		}

		if (line.find(searchingFor) != std::string::npos) {
			inside_for = true;
			for_parameters = File::forOperations(line);
		}
		else if (line.find(searchingEndfor) != std::string::npos) {
			for_content.push_back(line);
			
			for (int i = 0; i < for_parameters[0].size(); ++i) {		// number of iterations
				for (auto a : for_content) {							// loop content
					for (int j = 0; j < for_parameters.size(); ++j) {	// number of arguments
						while (a.find("$" + std::to_string(j + 1)) != std::string::npos) {
							a.replace(a.find("$" + std::to_string(j + 1)), 2, for_parameters[j][i]);
						}
					}
					file_result.push_back(a);
				}
			}
		}

		if (inside_for) {
			for_content.push_back(line);
		}
		else {
			file_result.push_back(line);
		}
		
		if (line.find(searchingEndfor) != std::string::npos) {
			inside_for = false;
			for_content.clear();
		}
	}

	file.close();

	this->path = path;
	this->file = file_result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<std::string>> File::forOperations(std::string forLine) {
	//!!!!!!!!!!!!!!!!--- Zmienne sta³e do wyszukiwania ---!!!!!!!!!!!!!!!!
	std::string const _searchingFor{ "%for " };
	std::string const _searchingList{ "list:" };
	std::string const _searchingComa{ "," };
	std::string const _searchingBracket{ ")" };
	std::string const _searchingHex{ "0x" };
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	forLine.erase(0, forLine.find(_searchingFor) + _searchingFor.length());

	std::vector<std::vector<std::string>> parametry;
	std::string tempLine = forLine, parametr;
	int i = 0, iterations;

	do {
		parametr = tempLine.substr(0, tempLine.find(_searchingBracket) + 1);
		tempLine.erase(0, tempLine.find(_searchingBracket) + 1);

		if (!i++) {
			iterations = atoi(parametr.substr(1, parametr.length() - 1).c_str());
		}
		else {
			std::vector<std::string> parameters;

			if (parametr.find(_searchingList) != std::string::npos) {
				parametr.erase(0, parametr.find(_searchingList) + 5);
				parametr.pop_back();
				parametr += ',';

				for (int k = 0; k < iterations; ++k) {
					parameters.push_back(parametr.substr(1, parametr.find(_searchingComa) - 2));
					parametr.erase(0, parametr.find(_searchingComa) + 1);
				}
				parametry.push_back(parameters);
			}
			else {
				std::string firstS = parametr.substr(1, parametr.find(_searchingComa));
				std::string jumpS = parametr.substr(parametr.find(_searchingComa) + 1, parametr.find_last_of(_searchingBracket) - parametr.find(_searchingComa) - 1);

				unsigned long long  firstI, jumpI;

				if (firstS.find(_searchingHex) != std::string::npos) {
					firstI = File::hexToDec(firstS);
				}
				else {
					firstI = atoi(firstS.c_str());
				}
				if (jumpS.find(_searchingHex) != std::string::npos) {
					jumpI = File::hexToDec(jumpS);
				}
				else {
					jumpI = atoi(jumpS.c_str());
				}

				for (int k = 0; k < iterations; ++k) {
					unsigned __int64 wynik = firstI + (jumpI*k);
					if (firstS.find(_searchingHex) != std::string::npos) {
						parameters.push_back(File::decToHex(wynik));
					}
					else {
						parameters.push_back(std::to_string(wynik));
					}
				}
				parametry.push_back(parameters);
			}
		}
	} while (!tempLine.empty());

	return parametry;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void File::makeFile(std::string path, std::vector <std::vector<std::string>> fileVector) {
	std::fstream file;
	file.open(path, std::ios::out);
	for (auto section : fileVector) {
		for (auto row : section) {
			file << row.c_str() << std::endl;
		}
		file << std::endl;
	}
	file.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long long File::hexToDec(std::string hexValue) {
	unsigned long long decValue;
	std::stringstream stream;

	stream << hexValue;
	stream >> std::hex >> decValue;

	return decValue;
}
std::string File::decToHex(unsigned long long decValue) {
	std::stringstream ss;

	ss << std::hex << decValue;
	std::string hexValue = "0x" + ss.str();

	return hexValue;
}

