/*
    libnoisecl - procedural noise generation tool based on OpenCL library
    Copyright (C) 2013  Messenger of death <messengerofdeath@gmail.com>

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


#include "clnoise/basemodule.h"
#include "clnoise/error.h"
#include "clnoise/noisemap.h"
#include "clnoise/gradientattribute.h"

using namespace CLNoise;

int BaseModule::CurrentModuleId = 0;

BaseModule::BaseModule(const std::string &mName, MODULE_TYPE type) :
	moduleType(type),
	moduleName(mName),
	moduleId(CurrentModuleId++)
{
}

BaseModule::~BaseModule()
{

}

void BaseModule::addDependency(const char *dep)
{
	dependencies.push_back(dep);
}

const std::vector<std::string> &BaseModule::getDependencyList() const
{
	return dependencies;
}

Attribute BaseModule::getAttribute(unsigned int attributeId) const
{
	if (attributeId >= attributes.size()) CL_THROW("Requested information for not existed attribute");
	return *attributes[attributeId];
}

unsigned int BaseModule::getAttributeCount() const
{
	return attributes.size();
}

const std::vector< Attribute * > &BaseModule::getAttributes()
{
	return attributes;
}

BaseModule::ContactInfo BaseModule::getInput(unsigned int inputId) const
{
	if (inputId >= inputs.size()) CL_THROW("Requested information for not existed input");
	return inputs[inputId];
}

unsigned int BaseModule::getInputCount() const
{
	return inputs.size();
}

const std::vector< BaseModule::ContactInfo > &BaseModule::getInputs() const
{
	return inputs;
}

std::string BaseModule::getName() const
{
	return moduleName;
}

BaseModule::ContactInfo::CONTACT_TYPE BaseModule::getOutputType() const
{
	return outputType;
}

void BaseModule::setModuleSource(const char *source)
{
	kernelSource = source;
}

void BaseModule::setAttribute(const Attribute &attribute)
{
	bool isFound = false;
	for (unsigned int a = 0 ; a < attributes.size(); ++a)
	{
		Attribute *att = attributes[a];
		if (att->getName() == attribute.getName())
		{
			delete attributes[a];
			attributes[a] = createAttribute(attribute);
			return;
		}
	}
	if (!isFound) attributes.push_back(createAttribute(attribute));
}

void BaseModule::setAttribute(unsigned int id, const Attribute &attribute)
{
	if (id < attributes.size())
	{
		delete attributes[id];
		attributes[id] = createAttribute(attribute);
	}
	else
	{
		attributes.resize(id + 1);
		attributes[id] = createAttribute(attribute);
	}
}

void BaseModule::setInput(unsigned int inputId, BaseModule *input)
{
	if (!input) CL_THROW("NULL module passed as input");
	if (inputId >= inputs.size()) CL_THROW("Requested connection to not existed slot");
	if (input->getOutputType() != inputs[inputId].type) CL_THROW("Unable to connect input to required slot. Types mismatch");

	inputs[inputId].input = input;
}

void BaseModule::addInput(const ContactInfo &ci)
{
	ContactInfo c = ci;
	c.input = nullptr;
	inputs.push_back(c);
}

void BaseModule::setOutputType(BaseModule::ContactInfo::CONTACT_TYPE type)
{
	outputType = type;
}

BaseModule::MODULE_TYPE BaseModule::getType() const
{
	return moduleType;
}


void BaseModule::build(NoiseMap *map)
{
	for (unsigned a = 0; a < dependencies.size(); ++a)
	{
		map->addDependency(dependencies[a]);
	}

	for (unsigned a = 0; a < inputs.size(); ++a)
	{
		ContactInfo ci = inputs[a];
		if (ci.input == nullptr) CL_THROW(std::string("NULL passed as input to module: ") + moduleName);
		ci.input->build(map);
	}

	//Generate header
	buildHeader(map);
	buildSource(map);
}

std::string BaseModule::getModuleCallName() const
{
	return moduleName;
}

Attribute *BaseModule::createAttribute(const Attribute &attribute)
{
	switch (attribute.getType())
	{
		case Attribute::INT:
		case Attribute::FLOAT:
			return new Attribute(attribute);
		case Attribute::GRADIENT:
			return new GradientAttribute(static_cast<const GradientAttribute&>(attribute));
		case Attribute::INVALID:
			CL_THROW("Invalid attribute passed");
			return nullptr;
	}
	return nullptr;
}

Attribute BaseModule::getAttribute(const std::string &attributeName) const
{
	for (auto att : attributes)
	{
		if (att->getName() == attributeName) return *att;
	}
	CL_THROW("Invalid attribute requested");
	return Attribute();
}
