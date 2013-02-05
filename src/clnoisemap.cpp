/*
    libnoisecl - procedural noise generation tool based on OpenCL library
    Copyright (C) 2013  Denis Biryukov <denis.birukov@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>

#include "clnoisemap.h"
#include "clnoiseerror.h"
#include "clnoiseoutput.h"
#include "clnoiselibrary.h"
#include "clnoisemoduleattribute.h"

using namespace CLNoise;

NoiseMap::NoiseMap() :
	kernelSource (""),
	buildedOutput (nullptr)
{

}

NoiseMap::~NoiseMap()
{

}

void NoiseMap::build (Output *output)
{
	if (!output) THROW ("Invalid output passed");

	auto outputInputs = output->getInputs();
	auto outputControls = output->getControls();

	if (output->getInputCount() != outputInputs.size()) THROW ("Invalid amount of input modules in output module");
	if (output->getControlCount() != outputControls.size()) THROW ("Invalid amount of input modules in output module");

	processedDeps.clear();
	attributeMap.clear();
	intAttributes.clear();
	floatAttributes.clear();

	buildedOutput = output;

	std::string modulesProtos, modulesCode, kernelCode;

	for (auto input : outputInputs)
	{
		processModule (input, &modulesProtos, &modulesCode, &kernelCode);
	}

	for (auto control : outputControls)
	{
		processModule (control, &modulesProtos, &modulesCode, &kernelCode);
	}

	buildCode(modulesProtos, modulesCode, kernelCode);
}

void NoiseMap::updateAttributes()
{
	for (auto moduleIt : attributeMap)
	{
		for (ModuleAttribute attribute : moduleIt.first->getAttributes())
		{
			auto attIt = moduleIt.second.find(attribute.getName());
			if (attIt == moduleIt.second.end()) THROW (std::string("Attribute \"") + attribute.getName() + "\" not listed in attribute map.");

			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					if (floatAttributes.size() <= attIt->second) THROW ("Attribute position is out of bounds");
					floatAttributes[attIt->second] = attribute.getFloat();
					break;
				case ModuleAttribute::INT:
					if (intAttributes.size() <= attIt->second) THROW ("Attribute position is out of bounds");
					intAttributes[attIt->second] = attribute.getInt();
					break;
				default:
					THROW ("Invalid module attribute type");
			}
		}
	}
}

void NoiseMap::processModule (BaseModule *module, std::string *proto, std::string *code, std::string *kernelCode)
{
	if (!module) THROW ("Unable to process NULL module");
	if (module->getType() == BaseModule::OUTPUT) THROW ("Impossible to have more then one Output module in noise map");

	processDeps (module, proto, code);

	Function *function = dynamic_cast<Function *> (module);
	proto->append (function->getProto());
	code->append (function->getKernelSource());

	if (module->getType() == BaseModule::BASE)
	{
		Module *baseModule = dynamic_cast<Module *> (module);

		for (auto input : baseModule->getInputs())
		{
			processModule (input, proto, code, kernelCode);
		}

		for (auto control : baseModule->getControls())
		{
			processModule (control, proto, code, kernelCode);
		}

		generateAttributes (baseModule);
		generateKernelCode (baseModule, kernelCode);
	}
}

void NoiseMap::processDeps (BaseModule *module, std::string *proto, std::string *code)
{
	for (auto & str : module->getDependencyList())
	{
		if (processedDeps.find (str) != processedDeps.end()) continue;

		BaseModule *mod = Library::getInstance().getModule(str);
		if (!mod) THROW ("Invalid dependency: " + str);
		if (mod->getType() == BaseModule::OUTPUT) THROW ("Output module can't be set as dependency");

		Function *function = dynamic_cast<Function *> (mod);
		proto->append (function->getProto());
		code->append (function->getKernelSource());

		processedDeps.insert (str);

		processDeps(mod, proto, code);
	}
}

void NoiseMap::generateAttributes (Module *module)
{
	if (module->getType() == BaseModule::FUNCTION) THROW ("Modules of type \"Function\" can't have attributes");

	auto attributeMapEntryIt = attributeMap.find (module);
	if (attributeMapEntryIt == attributeMap.end())
	{
		attributeMapEntryIt = attributeMap.insert (std::make_pair (module, std::map<std::string, unsigned>())).first;
	}

	for (ModuleAttribute attribute : module->getAttributes())
	{
		unsigned position;

		//try to find attribute in existed set
		auto attIt = attributeMapEntryIt->second.find (attribute.getName());
		if (attIt != attributeMapEntryIt->second.end())
		{
			position = attIt->second;
			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					if (floatAttributes.size() <= position) THROW ("Attribute position is out of bounds");
					floatAttributes[position] = attribute.getFloat();
					break;
				case ModuleAttribute::INT:
					if (intAttributes.size() <= position) THROW ("Attribute position is out of bounds");
					intAttributes[position] = attribute.getInt();
					break;
				default:
					THROW ("Unknown attribute type");
			}
		}
		else
		{
			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					position = floatAttributes.size();
					floatAttributes.push_back (attribute.getFloat());
					break;
				case ModuleAttribute::INT:
					position = intAttributes.size();
					intAttributes.push_back (attribute.getInt());
					break;
				default:
					THROW ("Unknown attribute type");
			}
			attributeMapEntryIt->second.insert(std::make_pair(attribute.getName(), position));
		}
	}
}

void NoiseMap::generateKernelCode (Module *module, std::string *kernelCode)
{
	kernelCode->append(std::string("float ") + module->getName() + "Out = " + module->getName() + "( ");
	std::vector<std::string> args;

	//Generator module - pass position as first argument
	if(module->getInputCount() == 0)
	{
		args.push_back("position");
	}
	else
	{
		for ( auto input : module->getInputs())
		{
			args.push_back(input->getName() + "Out");
		}
	}
	for ( auto control : module->getControls())
	{
		args.push_back(control->getName() + "Out");
	}

	auto attributeMapEntryIt = attributeMap.find(module);
	if (attributeMapEntryIt == attributeMap.end()) THROW ("Unable to found attribute map for module");

	for ( ModuleAttribute att : module->getAttributes())
	{
		auto attInfoIt = attributeMapEntryIt->second.find(att.getName());
		if(attInfoIt == attributeMapEntryIt->second.end()) THROW ("Unmapped attribute");

		char value[256];

		switch (att.getType())
		{
			case ModuleAttribute::FLOAT:
				snprintf(value, 255, "floatAttributes[%d]", attInfoIt->second);
				break;
			case ModuleAttribute::INT:
				snprintf(value, 255, "intAttributes[%d]", attInfoIt->second);
				break;
			default:
				THROW ("Unknown attribute type");
		}
		args.push_back(value);
	}

	bool isFirst = true;
	for (auto str : args)
	{
		if (!isFirst) kernelCode->append(" ,");
		kernelCode->append(str);
		isFirst = false;
	}
	kernelCode->append(");");
}

void NoiseMap::buildCode (const std::string &proto, const std::string &code, const std::string &kernelCode)
{
	kernelSource.clear();
	kernelSource.append(proto);
	kernelSource.append("\n");
	kernelSource.append(code);
	kernelSource.append("\n");


	kernelSource.append(kernelCode);
}

