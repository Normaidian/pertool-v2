#include "pch.h"
#include "module.h"
#include "file.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>

void sifExplainer(); 
void register_param();
void attempt_task();

int printmenu() {
	int choice;

	std::cout << " ________________________________\n";
	std::cout << "|             MENU               |\n";
	std::cout << "|________________________________|\n";
	std::cout << "|  1 - Module table (*.p file)   |\n";
	std::cout << "|________________________________|\n";
	std::cout << "|  2 - Module table (*.ph file)  |\n";
	std::cout << "|________________________________|\n";
	std::cout << "|  4 - Sif explainer             |\n";
	std::cout << "|________________________________|\n";
	std::cout << "|  5 - KWI                       |\n";
	std::cout << "|________________________________|\n";
	std::cout << "|  0 - Exit                      |\n";
	std::cout << "|________________________________|\n";
	std::cout << "Make choice: ";
	std::cin >> choice;

	if (!std::cin) {
		throw(std::logic_error("---Wrong choice!---"));
	}

	return choice;
}
//**************************************************************************************************************************************

int main() {
	do {
		try {
			switch (printmenu()) {
				case 1:
					Module::main();
					break;
				case 2:
					register_param();
					break;
				case 4:
					sifExplainer();
					break;
				case 5:
					attempt_task();
					break;
				case 0:
					exit(0);
					break;
				default:
					break;
			}
		} catch (std::logic_error &e) {
			std::cout << e.what() << std::endl;
			system("pause");
		}

		system("cls");
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	} while (true);
}
//**************************************************************************************************************************************

void sifExplainer() {
	//////////////////---Przygotowanie zmiennych---//////////////////
	std::string const	_config{ "sifExplainer\\_config.txt" };
	std::string const	_mcus{ "sifExplainer\\mcus.txt" };
	std::string const	_mcusResult{ "mcusResult_sorted.txt" };
	std::string const	_sort{ "_sort.exe" };
	std::string const	_run{ "__run.bat" };
	std::string path, line, disk;
	std::vector<std::string> mcus, lineFromFile;
	////////////////////////////////////////////////////////////////

	std::cout << "Enter file path: ";
	std::cin >> path;

	std::fstream file;

	disk = path.substr(0, 3);
	
	file.open(disk + _config, std::ios::in);

	while (std::getline(file, line)) {
		lineFromFile.push_back(line);
	}

	file.close();

	while (true) {
		std::string mcu;
		std::cout << "Enter MCU (Enter \"0\" to stop): ";
		std::cin >> mcu;
		if (mcu == "0") break;
		mcus.push_back(mcu);
	}

	file.open(disk + _config, std::ios::out);
	file << path << std::endl;
	for (int i = 1; i < lineFromFile.size(); ++i)	file << lineFromFile[i] << std::endl;
	file.close();

	file.open(disk + _mcus, std::ios::out);
	for (int i = 0; i < mcus.size(); ++i) {
		file << mcus[i];
		if (i != mcus.size() - 1)	file << std::endl;
	}
	file.close();

	ShellExecuteA(0, "open", _run.c_str(), 0, 0, SW_HIDE);

	std::cout << "Press \"Enter\" after end Trace32!    ";
	system("pause");
	ShellExecuteA(0, "open", _sort.c_str(), 0, 0, SW_HIDE);
	ShellExecuteA(0, "open", _mcusResult.c_str(), 0, 0, SW_HIDE);
	std::cout << "Sorted was ended. \nCheck file \"mcusResult_sorted.txt\" for results.    ";
	system("pause");
}
//**************************************************************************************************************************************

void register_param() {
	std::string path, parametr = "x",baseAddress;
	std::vector<std::string> parameters;

	std::cout << "Enter file path: ";
	std::cin >> path;


	std::cout << "Enter Base Address: ";
	std::getline(std::cin, baseAddress);
	std::getline(std::cin, baseAddress);

	while (parametr != "0") {
		std::cout << "Enter paramter: ";
		std::cin >> parametr;
		parameters.push_back(parametr);
	}
	
	Register::main("", path,std::strtoul(baseAddress.c_str(), 0, 16), parameters);
}
//**************************************************************************************************************************************

void attempt_task() {
	File::makeFile("errors.txt",Module::attempt_task());
	std::cout << "\tErrors are describet in \"errors.txt\" file." << std::endl;
	ShellExecuteA(0, "open", "errors.txt", 0, 0, SW_HIDE);
	system("pause");
}