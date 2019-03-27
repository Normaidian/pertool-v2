#include "pch.h"
#include "register.h"
#include "module.h"
#include <vector>

//**************************************************************************************************************************************
void Register::main(std::string name, std::string source, unsigned long long baseAddress, std::vector<std::string> parameters) {
	system("cls");
	std::cout << "CHOSEN MODULE: " << name << std::endl;
	std::cout << "FILE LOCATION: " << source << std::endl;
	std::cout << "BASE ADDRESS:  0x" << baseAddress << std::endl;
	std::cout << "PARAMETERS:    ";

	for (auto element : parameters) std::cout << element << " ";

	std::cout << std::endl << std::endl;
	printTable(registerVector(source,parameters,baseAddress));
	system("pause");
}

//**************************************************************************************************************************************
std::vector<Register> Register::registerVector(std::string source, std::vector<std::string> parameters, unsigned long long baseAddress) {
	//////////////////---Przygotowanie zmiennych---//////////////////
	File f(source, parameters);
	std::vector<std::string> file = f.file;
	std::vector<Register> register_vec;
	std::string g_access, r_size, r_name;
	unsigned long long g_offset, r_offset, r_base;
	int loop_start = 0, actual_loop_i = 0;
	bool inside_if = false;
	////////////////////////////////////////////////////////////////

	for (int i = 0; i < file.size(); ++i) {		
		if (file[i].find("group.") != std::string::npos) {
			if (file[i].find("hgroup.") != std::string::npos) {
				g_access = "Hide";
			}
			else if (file[i].find("rgroup.") != std::string::npos) {
				g_access = "Read Only";
			}
			else if (file[i].find("wgroup.") != std::string::npos) {
				g_access = "Write Only";
			}
			else if (file[i].find("group.") != std::string::npos) {
				g_access = "Read/Write";
			}

			r_size = file[i].substr(file[i].find("group.") + 6, 5);
			
			if (file[i].substr(file[i].find("0x"), file[i].find("++") - file[i].find("0x")).find("+") != std::string::npos) {

				std::string temp_offset = file[i].substr(file[i].find("0x"), file[i].find("++") - file[i].find("0x") - 1);

				unsigned long long temp_a = std::strtoul(temp_offset.substr(2,temp_offset.find("+") - 2).c_str(),0,16);
				unsigned long long temp_b = std::strtoul(temp_offset.substr(temp_offset.find("+") + 3, std::string::npos).c_str(), 0, 16);
				g_offset = temp_a + temp_b;
			}else{
				g_offset = std::strtoul(file[i].substr(file[i].find("0x"), file[i].find("++") - file[i].find("0x")).c_str(), 0, 16);
			}
		}
		else if ((file[i].find("line.") != std::string::npos)||(file[i].find("hide.") != std::string::npos)) {
			r_offset = g_offset + std::strtoul(file[i].substr(file[i].find("0x"), file[i].find(" \"") - file[i].find("0x")).c_str(), 0, 16);
			r_base = baseAddress + r_offset;
			r_name = file[i].substr(file[i].find("\"") + 1, file[i].find(",") - file[i].find("\"") - 1);

			if (inside_if) {
				r_name += "*";
			}

			Register r(g_access, r_base, r_offset, r_size, r_name);
			register_vec.push_back(r);
		}
		else if (file[i].find("if") != std::string::npos){
			if ((file[i].find("%if ") != std::string::npos)||(file[i].find("sif ") != std::string::npos)||(file[i].find("if (") != std::string::npos)) {
				inside_if = true;
			}else if((file[i].find("%endif") != std::string::npos)||(file[i].find("endif") != std::string::npos)) {
				inside_if = false;
			}
		}
		else if (file[i].find("base ") != std::string::npos){
			baseAddress = std::strtoul(file[i].erase(0, file[i].find("0x")).c_str(), 0, 16);
		}
	}

	return register_vec;
}

//**************************************************************************************************************************************
void Register::printTable(std::vector<Register> register_vec) {
	int width = 4;
		
	for (auto element : register_vec) {
		if (element.name.size() > width) width = static_cast<int>(element.name.size());
	}
	std::string s(47 + width, '_');
	std::string e(46 + width, ' ');

	std::cout << ' ' << s << ' ' << std::endl;
	std::cout << "| " << std::left
		<< std::setw(width) << "Name" << " | "
		<< std::setw(11) << "Access" << "| "
		<< std::setw(11) << "Address" << "| "
		<< std::setw(10) << "Offset" << "| "
		<< std::setw(5) << "Size" << "| "
		<< std::endl;
	std::cout << '|' << s << '|' << std::endl;

	if(register_vec.size()){
		for (auto element : register_vec) {
			std::cout << std::left << "| "
				<< std::setw(width + 1) << element.name << "| "
				<< std::setw(11) << element.access << "| 0x"
				<< std::setw(9) << std::hex << std::uppercase << element.address << "| 0x";
			
			if (element.offset < 16) {
				std::cout << std::left << "0"
					<< std::setw(7) << std::uppercase << element.offset << "| ";
			}
			else {
				std::cout << std::left
					<< std::setw(8) << std::uppercase << element.offset << "| ";
			}
		
			std::cout << std::left 
				<< std::setw(5) << std::dec << element.size << "| "
				<< std::endl;
			std::cout << '|' << s << '|' << std::endl;
		}
	}
	else {
		std::cout << "| " << std::setw(s.size() - 6) << "\t  Something went wrong!" << "| " << std::endl;
		std::cout << "| " << std::setw(s.size() - 1) << e << "| " << std::endl;
		std::cout << "| " << std::setw(s.size() - 6) << "\tProbably the file that was given" << "| " << std::endl;
		std::cout << "| " << std::setw(s.size() - 6) << "\tdoes not contain any registers." << "| " << std::endl;
		std::cout << '|' << s << '|' << std::endl;
	}

}

//**************************************************************************************************************************************

std::vector<std::string> Register::attempt_task(Module baseModule, Module comparedModule){
	std::vector<std::string> errorsVector;
	std::vector<Register> registersBaseVector = registerVector(baseModule.source,baseModule.parameters,baseModule.baseAddress);
	std::vector<Register> registersComparedVector = registerVector(comparedModule.source, comparedModule.parameters, comparedModule.baseAddress);;

	if (registersBaseVector.size() != registersComparedVector.size()) {												// Porównanie iloœci rejestrów
		errorsVector.push_back("Wrong numbers of registers in file :  " + comparedModule.source);
	}
	
	for (auto registerBase:registersBaseVector) {
		for (auto registerCompared : registersComparedVector) {
			if (registerBase.name == registerCompared.name) {
				if (registerBase.access != registerCompared.access) {											    // Porównanie dostêpów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another access.\n\t\t"
						+ registerCompared.access + " -> " + registerBase.access);
				}
				if (registerBase.address != registerCompared.address) {												// Porównanie adresów
					if (registerBase.offset != registerCompared.offset) {											// Porównanie offsetów
						errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another offset.\n\t\t"
							+ std::to_string(registerCompared.offset) + " -> " + std::to_string(registerBase.offset));
					}
					else {																							// Wypisanie iformacji o b³êdnym adresie
						errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another address.\n\t\t"
							+ std::to_string(registerCompared.address) + " -> " + std::to_string(registerBase.address));
			
					}
				}
				if (registerBase.size != registerCompared.size) {													// Porównanie wielkoœci rejestrów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another size.\n\t\t"
						+ registerCompared.size + " -> " + registerBase.size);
				}

				break;
			} else if (registerBase.offset == registerCompared.offset) {
				if (registerBase.access != registerCompared.access) {											    // Porównanie dostêpów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another access.\n\t\t"
						+ registerCompared.access + " -> " + registerBase.access);
				}
				if (registerBase.address != registerCompared.address) {												// Porównanie adresów																							// Wypisanie iformacji o b³êdnym adresie
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another address.\n\t\t"
						+ std::to_string(registerCompared.address) + " -> " + std::to_string(registerBase.address));
				}
				if (registerBase.name != registerCompared.name) {													// Porównanie nazw																							// Wypisanie iformacji o b³êdnym adresie
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another name.\n\t\t"
						+ registerCompared.name + " -> " + registerBase.name);
				}
				if (registerBase.size != registerCompared.size) {													// Porównanie wielkoœci rejestrów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another size.\n\t\t"
						+ registerCompared.size + " -> " + registerBase.size);
				}

				break;
			} else if (registerBase.address == registerCompared.address) {
				if (registerBase.access != registerCompared.access) {											    // Porównanie dostêpów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another access.\n\t\t"
						+ registerCompared.access + " -> " + registerBase.access);
				}
				if (registerBase.offset != registerCompared.offset) {												// Porównanie ofsetów																							// Wypisanie iformacji o b³êdnym adresie
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another offset.\n\t\t"
						+ std::to_string(registerCompared.offset) + " -> " + std::to_string(registerBase.offset));
				}
				if (registerBase.name != registerCompared.name) {													// Porównanie nazw																							// Wypisanie iformacji o b³êdnym adresie
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another name.\n\t\t"
						+ registerCompared.name + " -> " + registerBase.name);
				}
				if (registerBase.size != registerCompared.size) {													// Porównanie wielkoœci rejestrów
					errorsVector.push_back("Register <<" + registerCompared.name + ">> have a another size.\n\t\t"
						+ registerCompared.size + " -> " + registerBase.size);
				}

				break;
			} else if (registerCompared.name == registersComparedVector.back().name) {
				errorsVector.push_back("Missing register " + registerBase.name + " on offset "  + std::to_string(registerBase.offset));
			}
		}
	}
	
	return errorsVector;
}