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
#include "clnoiseerror.h"

namespace CLNoise
{
class ModuleAttribute
{
public:
    enum ATTRIBUTE_TYPE
    {
        INVALID = -1,
        INT     = 0,
        FLOAT   = 1,
    };
    ModuleAttribute() : type ( INVALID ) {};
    explicit ModuleAttribute ( const std::string &name, int val, int min, int max ) :
        type ( INT ), attributeName ( name )
    {
        intValue[0] = val;
        intValue[1] = min;
        intValue[2] = max;
        floatValue[0] = floatValue[1] = floatValue[2] = -1.0f;
    };
    explicit ModuleAttribute ( const std::string &name, float val, int min, int max ) :
        type ( FLOAT ), attributeName ( name )
    {
        floatValue[0] = val;
        floatValue[1] = min;
        floatValue[2] = max;
        intValue[0] = intValue[1] = intValue[2] = -1;
    };

    void setValue ( int val )
    {
        type = INT;
        intValue[0] = val;
    }

    void setValue ( float val )
    {
        type = FLOAT;
        floatValue[0] = val;
    }
    int getIntMin() const
    {
        if ( type != INT ) THROW ( "Invalid type requested" );
        return intValue[1];
    }
    int getIntMax() const
    {
        if ( type != INT ) THROW ( "Invalid type requested" );
        return intValue[2];
    }
    int getInt() const
    {
        if ( type != INT ) THROW ( "Invalid type requested" );
        return intValue[0];
    }

    float getFloat() const
    {
        if ( type != FLOAT ) THROW ( "Invalid type requested" );
        return floatValue[0];
    }
    float getFloatMin() const
    {
        if ( type != FLOAT ) THROW ( "Invalid type requested" );
        return floatValue[1];
    }
    float getFloatMax() const
    {
        if ( type != FLOAT ) THROW ( "Invalid type requested" );
        return floatValue[2];
    }

    ATTRIBUTE_TYPE getType() const
    {
        return type;
    }

    const std::string &getName() const
    {
        return attributeName;
    }

private:
    int intValue[3];
    float floatValue[3];
    ATTRIBUTE_TYPE type;
    std::string attributeName;
};
}
