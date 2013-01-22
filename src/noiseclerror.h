/*
 *    libnoisecl - procedural noise generation tool based on OpenCL library
 *    Copyright (C) 2013  Messenger of death <messengerofdeath@gmail.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include <string>
#define THROW(reason) throw(NOISECL::Error(reason, __FILE__, __FUNCTION__, __LINE__))
namespace NOISECL
{
class Error
{
public:
    Error ( const std::string &reason, const std::string &file, const std::string &function, int line ) : _file_ ( file ), _function_ ( function ), _line_ ( line ), _reason_ ( reason ) {}
    ~Error() {}
    std::string get_file() const
    {
        return _file_;
    }
    std::string get_function() const
    {
        return _function_;
    }
    std::string get_reason() const
    {
        return _reason_;
    }
    int get_line() const
    {
        return _line_;
    }
    const char *what() const
    {
        return _reason_.c_str();
    }
private:
    std::string _file_;
    std::string _function_;
    int _line_;
    std::string _reason_;
};
}
