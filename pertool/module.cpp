#include "pch.h"
#include "module.h"

int Module::actualID = 0;

//**************************************************************************************************************************************
void Module::main(std::string path) {
	int choice = 0;
	std::vector<Module> m;
	File f(path);

	do {
		Module::actualID = 0;

		m = Module::moduleVector(f);
		
		Module::printTable(m);

		std::cout << "Choose number of module | Select '0' to exit: ";
		std::cin >> choice;
		
		if (!std::cin) {
			throw(std::logic_error("---Wrong choice!---"));
		}

		for (auto element : m) {
			if (element.id == choice) {
				Register::main(element.name,element.source,element.baseAddress,element.parameters);
			}
		}
	} while (choice);
}

//**************************************************************************************************************************************

std::vector<std::vector<std::string>> Module::attempt_task() {
	//////////////////---Przygotowanie zmiennych---//////////////////
	std::string const _baseTaskPath{ "sifExplainer\\baseAttemptTask\\persh7251.p" };
	std::string const _separator{ "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" };
	std::string _comparedTaskPath;
	std::string _disk;

	std::vector<std::string> pFileErrors;
	std::vector<std::string> separatorBar{ _separator };
	std::vector<std::vector<std::string>> errorsVector;
	std::vector<Module> moduleBaseVector;					//! Zawiera modu³y z pliku bazowego
	std::vector<Module> moduleComparedVector;				//! Zawiera modu³y z podanego pliku
	////////////////////////////////////////////////////////////////
	File f("");

	moduleComparedVector = moduleVector(f);
	_disk = moduleComparedVector[0].source.substr(0, 3);
	moduleBaseVector = moduleVector(_disk + _baseTaskPath);

	separatorBar.push_back("            *.p file");
	errorsVector.push_back(separatorBar);
	errorsVector.push_back(pFileErrors);
	separatorBar.pop_back();

	if (moduleBaseVector.size() != moduleComparedVector.size()) {										//! Nieodpowiednia iloœæ modu³ów
		pFileErrors.push_back("Wrong number of modules.");
	}

	for (auto moduleBase : moduleBaseVector) {
		for (auto moduleCompared : moduleComparedVector) {
			if (moduleBase.name == moduleCompared.name) {
				if (moduleBase.baseAddress != moduleCompared.baseAddress) {								//! Porównanie adresów bazowych
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong base address.");
				}
				if (moduleBase.memoryClass != moduleCompared.memoryClass) {								//! Porównanie klase pamiêci
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong memory class.\n\t\t\t" 
						+ moduleCompared.memoryClass + " -> " + moduleBase.memoryClass);
				}
				if (moduleBase.parameters != moduleCompared.parameters) {								//! Porównanie oarametrów
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong parameters.");
				}


				separatorBar.push_back("            " + moduleBase.name);								//! Wstawianie b³êdów z pliku ph
				separatorBar.push_back("      " + moduleBase.source);									//! Wstawianie b³êdów z pliku ph
				errorsVector.push_back(separatorBar);													//! do vectora z b³êdami
				separatorBar.pop_back();																//! wraz z nag³ówkiem
				separatorBar.pop_back();																//! wraz z nag³ówkiem
				errorsVector.push_back(Register::attempt_task(moduleBase, moduleCompared));


				break;
			} else if (moduleBase.baseAddress == moduleCompared.baseAddress) {
				if (moduleBase.name != moduleCompared.name) {											//! Porównanie nazw
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong name.");
				}
				if (moduleBase.memoryClass != moduleCompared.memoryClass) {								//! Porównanie klase pamiêci
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong memory class.\n\t\t\t"
						+ moduleCompared.memoryClass + " -> " + moduleBase.memoryClass);
				}
				if (moduleBase.parameters != moduleCompared.parameters) {								//! Porównanie oarametrów
					pFileErrors.push_back("Module <<" + moduleCompared.name + ">> have wrong parameters.");
				}


				separatorBar.push_back("            " + moduleBase.name);								//! Wstawianie b³êdów z pliku ph
				separatorBar.push_back("      " + moduleBase.source);									//! Wstawianie b³êdów z pliku ph
				errorsVector.push_back(separatorBar);													//! do vectora z b³êdami
				separatorBar.pop_back();																//! wraz z nag³ówkiem
				separatorBar.pop_back();																//! wraz z nag³ówkiem
				errorsVector.push_back(Register::attempt_task(moduleBase, moduleCompared));


				break;
			} else if(moduleCompared.name == moduleComparedVector.back().name) {
				pFileErrors.push_back("Missing module " + moduleBase.name);
			}
		}
	}

	errorsVector[1] = pFileErrors;

	return errorsVector;
}

//**************************************************************************************************************************************
std::vector<Module> Module::moduleVector(File f) {
	//////////////////---Przygotowanie zmiennych---//////////////////
	std::string const _tree{ "tree " };
	std::string const _treeOpen{ "tree.open" };
	std::string const _treeEnd{ "tree.end" };
	std::string const _slash{ "\"" };
	std::string const _base{ "base " };
	std::string const _hex{ "0x" };
	std::string const _include{ "%include " };
	std::string const _topdir{ "${TOPDIR}/" };
	std::string const _ph{ ".ph" };
	std::string const _if{ "if " };
	std::string const _endif{ "endif" };

	unsigned long long baseAddress = 0;
	std::string name, memoryClass, parameters_str, prev_name, source, topdir, path;
	path = f.path;
	std::vector<std::string> file = f.file, parameters_vec;
	path = path.erase(path.find_last_of("\\")+1, std::string::npos);
	topdir = path.substr(0, path.find("szn\\") + 3);
	std::vector<Module> module_vec;
	int tree_level = 0, x = 0;
	bool inside_if = false;
	////////////////////////////////////////////////////////////////


	for (int i = 0; i < file.size(); ++i) {
		if ((file[i].find(_tree) != std::string::npos) || (file[i].find(_treeOpen) != std::string::npos)) {
			++tree_level;
			if (name.size()) name += " - ";
			prev_name = file[i].assign(file[i], file[i].find(_slash) + 1, file[i].find_last_of(_slash) - file[i].find(_slash) - 1);
			name += prev_name;
		} else if (file[i].find(_base) != std::string::npos){
			memoryClass = file[i].substr(file[i].find(_base) + 5, file[i].find(_hex) - file[i].find(_base) - 5);
			baseAddress = std::strtoul(file[i].erase(0, file[i].find(_hex)).c_str(),0,16);
		} else if ((file[i].find(_include) != std::string::npos)&&(baseAddress)){
			if (file[i].find(_topdir) != std::string::npos) {
				source = topdir + file[i].erase(0, file[i].find(_topdir) + 9);
			}else {
				source = path + file[i].erase(0, file[i].find(_include) + 9);
			}
			if (source.size() > source.find(_ph) + 3) {
				parameters_str = source.substr(source.find(_ph) + 4, source.size() - source.find(_ph));
				parameters_str.resize(parameters_str.size() + 1, ' ');
				do {
					parameters_vec.push_back(parameters_str.substr(0, parameters_str.find(' ')));
					parameters_str.erase(0, parameters_str.find(' ') + 1);
				} while (parameters_str.find(' ') != std::string::npos);
				source.assign(source, 0, source.find(_ph) + 3);
			}
		} else if ((file[i].find(_treeEnd) != std::string::npos)) {
			if (name.size() && baseAddress && source.size()) {
				if (inside_if) {
					name += "*";
				}

				Module m(name, memoryClass, baseAddress, source, parameters_vec);
				module_vec.push_back(m);
			}
			--tree_level;
			if (tree_level) {
				name.resize(name.find_last_of("-") -1);					// PROBLEM Z KASOWANIE DRZEW JE¯ELI W NAZWIÊ DRZEWA WYSTÊPUJÊ "-" np R-Mode
				//if (name.size()) name.resize(name.size() - 3);
			} else{
				name.clear();
			}
			baseAddress = 0;
			parameters_str.clear();
			parameters_vec.clear();
			source.clear();
		}
		else if (file[i].find(_if) != std::string::npos) {
			if ((file[i].find("%"+ _if) != std::string::npos) || (file[i].find("s"+ _if) != std::string::npos) || (file[i].find(_if+"(") != std::string::npos)) {
				inside_if = true;
			}
			else if (file[i].find(_endif) != std::string::npos) {
				inside_if = false;
			}
		}
	}
	return module_vec;
}
//**************************************************************************************************************************************

void Module::printTable(std::vector<Module> module_vec) {
	size_t width = 11;
	system("cls");

	for (auto element : module_vec) {
		if (element.name.size() > width) width = element.name.size();
	}
	std::string s(36 + width, '_');
	std::string e(35 + width, ' ');

	std::cout << ' ' << s << ' ' << std::endl;
	std::cout << "| " << std::left
		<< std::setw(5) << "ID" << "| "
		<< std::setw(width) << "Module name" << "| "
		<< std::setw(12) << "Memory class" << "| "
		<< std::setw(11) << "Base address" << "| "
		<< std::endl;
	std::cout << '|' << s << '|' << std::endl;

	if (module_vec.size()) {
		for (auto element : module_vec) {
			std::cout << std::left << "| "
				<< std::setw(5) << std::dec << element.id << "| "
				<< std::setw(width) << element.name << "| "
				<< std::setw(12) << element.memoryClass << "| 0x"
				<< std::setw(10) << std::hex << std::uppercase << element.baseAddress << "| "
				<< std::endl;
			std::cout << '|' << s << '|' << std::endl;
		}
	}
	else {
		std::cout  << "| " << std::setw(s.size()-6) << "\t  Something went wrong!" << "| " << std::endl;
		std::cout  << "| " << std::setw(s.size()-1) << e << "| " << std::endl;
		std::cout  << "| " << std::setw(s.size()-6) << "\tProbably the file that was given" << "| " << std::endl;
		std::cout  << "| " << std::setw(s.size()-6) << "\tdoes not contain any modules." << "| " << std::endl;
		std::cout << '|' << s << '|' << std::endl;
	}

}
//**************************************************************************************************************************************
