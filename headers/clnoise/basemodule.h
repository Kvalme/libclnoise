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

#pragma once

#include <string>
#include <vector>

#include "clnoise/attribute.h"

namespace CLNoise
{

class BaseModule
{
public:
	struct ContactInfo
	{
		enum CONTACT_TYPE
		{
			FLOAT,
			RGBA
		};
		std::string name;
		CONTACT_TYPE type;
		BaseModule *input;
		ContactInfo(const std::string &n, CONTACT_TYPE t) : name(n), type(t) {}
	};

	enum MODULE_TYPE
	{
		NONE = 0,
		GENERATOR = 1,
		OUTPUT = 2,
		FUNCTION = 3,
		FILTER = 4
	};

	BaseModule(const std::string &mName, MODULE_TYPE type);
	virtual ~BaseModule();

	/**
	 * Returns module name
	 * @return module name
	 */
	virtual std::string getName() const;

	/**
	 * Sets module OpenCL source for module
	 * @param source pointer to the null-terminated string, containing module source
	 */
	virtual void setModuleSource(const char *source);

	/**
	 * Sets module prototype.
	 * @param proto pointer to the null-terminated string, containing module prototype. Module prototype should be terminated with ';'
	 */
	virtual void setModuleProto(const char *proto);

	/**
	 * Return module prototype.
	 * @return null-terminated string with module prototype. Can be nullptr if prototype was not set
	 */
	virtual const char *getModuleProto() const;

	/**
	 * Builds OpenCL code for module and returns it
	 * @return null-terminated string with module source.
	 */
	virtual const char *getBuildedSource() = 0;

	/**
	 * Returns number of inputs that should be set for this module to work
	 * @return number of inputs
	 */
	virtual int getInputCount() const;

	/**
	 * Return number of attributes for module.
	 * @return number of attributes
	 */
	virtual int getAttributeCount() const;

	/**
	 * Returns output type of module
	 * @return module's output type
	 */
	virtual ContactInfo::CONTACT_TYPE getOutputType() const;

	/**
	 * Returns information about requested input slot.
	 * Returned information will contain pointer to input module if this input was set before or will be nullptr otherwise
	 * @return information about requested slot
	 */
	virtual ContactInfo getInput(int inputId) const;
	/**
	 * Returns vector, containing information about all inputs.
	 * Returned set shouldn't be modified
	 * @return constant reference to inputs information.
	 */
	virtual const std::vector<ContactInfo> &getInputs() const;

	/**
	 * Returns requested attribute information
	 * @return information about requested attribute
	 */
	virtual Attribute getAttribute(int attributeId) const;

	/**
	 * Return set of attributes for module.
	 * Returned set shouldn't be modified.
	 * @return constant reference to attributes information.
	 */
	virtual const std::vector<Attribute> &getAttributes() const;

	/**
	 * Connects given input slot to provided module. If any other module already connected to the requested slot it will be overwritten.
	 * Input type of given slot and output type of given module should be the same or exception will be raised
	 * @param inputId slot id
	 * @param input pointer to the input module
	 */
	virtual void setInput(int inputId, BaseModule *input);

	/**
	 * Sets attribute. Attribute will be found using Attribute::name.
	 * If no such attribute exists it will be added to the end of the attribute list. This can be used to create custom modules.
	 * If attribute can be found in existed attribute set - only attribute value will be set.
	 * @param attribute attribute description
	 */
	virtual void setAttribute(const Attribute &attribute);

	/**
	 * Sets attribute by id. If id less then attribute map size - value from @attribute will be copied into existed attribute,
	 * if id is bigger then attribute map size - new attribute will be created with values from @attribute
	 * @param id attribute slot
	 * @param attribute attribute description
	 */
	virtual void setAttribute(int id, const Attribute &attribute);

	/**
	 * Adds module dependency. Module will require all dependencies to be added into final OpenCL code.
	 * @param dep dependency name
	 */
	virtual void addDependency(const char *dep);

	/**
	 * Returns constant reference to dependency list. It shouldn't be modified.
	 * @return constant reference to dependency list
	 */
	virtual const std::vector<std::string>& getDependencyList() const;


protected:
	MODULE_TYPE moduleType;
	std::string moduleName;
	int moduleId;
	
	const char *kernelSource;
	const char *kernetlProto;


	std::vector<std::string> dependencies;
	std::vector<Attribute> attributes;
	std::vector<ContactInfo> inputs;

	static int CurrentModuleId;
};

}