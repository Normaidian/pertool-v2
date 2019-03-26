#pragma once
#include <string>
#include <iostream>
#include "file.h"

class Module;
class Register
{
private:
	std::string name;
	std::string access;
	unsigned long long address;
	unsigned long long offset;
	std::string size;

	static std::vector<Register> registerVector(std::string source, std::vector<std::string> parameters, unsigned long long baseAddress);
	static void printTable(std::vector<Register> register_vec);

public:
	Register(std::string access, unsigned long long address, unsigned long long offset, std::string size, std::string name="---"){
		this->name = name;
		this->access = access;
		this->address = address;
		this->offset = offset;
		this->size = size;
	};
	
	static void main(std::string name, std::string source, unsigned long long baseAddress, std::vector<std::string> parameters);
	static std::vector<std::string> attempt_task(Module baseModule, Module comparedModule);
};

