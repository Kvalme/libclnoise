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
#include "clnoise/moduleattribute.h"
#include "clnoise/function.h"

namespace CLNoise
{

class Module : public Function
{

public:

	Module (unsigned  attCount, unsigned  inpCount, unsigned  outCount, unsigned  contCount, const std::string &mName, const char *kSource);
	virtual ~Module();

	unsigned getInputCount() const
	{
		return m_inputCount;
	}
	unsigned getControlCount() const
	{
		return m_controlCount;
	}
	unsigned getOutputCount() const
	{
		return m_outputCount;
	}
	unsigned getAttributeCount() const
	{
		return m_attributeCount;
	}

	void setAttribute (const std::string &name, int value);
	void setAttribute (const std::string &name, float value);
	void setAttribute (unsigned int id, const CLNoise::ModuleAttribute &attribute);

	void setInput (unsigned int id, CLNoise::Module *source);
	void setControl (unsigned int id, CLNoise::Module *control);

	int addInput (Module *source);
	int addControl (Module *control);

	void removeInput (unsigned int id);
	void removeControl (unsigned int id);

	const std::vector<Module *>& getInputs() const
	{
		return m_inputs;
	}
	const std::vector<Module *>& getControls() const
	{
		return m_controls;
	}
	const std::vector<ModuleAttribute>& getAttributes() const
	{
		return m_attributes;
	}

protected:

	unsigned m_inputCount;
	unsigned m_controlCount;
	unsigned m_outputCount;
	unsigned m_attributeCount;

	std::vector<ModuleAttribute> m_attributes;
	std::vector<Module *> m_inputs;
	std::vector<Module *> m_controls;

};
}
