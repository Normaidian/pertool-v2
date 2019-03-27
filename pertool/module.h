#pragma once
#include "file.h"
#include "register.h"
#include <string>
#include <iostream>
#include <iomanip>


class Module
{
private:
	friend std::vector<std::string> Register::attempt_task(Module baseModule, Module comparedModule);;
	int id;
	
	std::string name;
	std::string memoryClass;
	unsigned long long baseAddress;
	std::string source;
	std::vector<std::string> parameters;

	static void printTable(std::vector<Module> module_vec);
	static std::vector<Module> moduleVector(File f);

public:
	static int actualID;
	Module(){};
	Module( std::string name, std::string memoryClass, unsigned long long baseAddress, std::string source, std::vector<std::string> parameters) {
		this->id = ++actualID;
		this->name = name;
		this->memoryClass = memoryClass;
		this->baseAddress = baseAddress;
		this->source = source;
		this->parameters = parameters;
	};

	static void main(std::string path="");
	static std::vector<std::vector<std::string>> attempt_task();
};

