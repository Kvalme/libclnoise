/*
 *    libnoisecl - procedural noise generation tool based on OpenCL library
 *    Copyright (C) 2013  Denis Biryukov <denis.birukov@gmail.com>
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

#include "time.h"

class Timer
{
public:
	Timer()
	{
		reset();
	}
	~Timer(){};
	uint64_t get_elapsed_time_us()
	{
		timespec tm;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tm);
		uint64_t curTime = tm.tv_sec * 1000000 + tm.tv_nsec / 1000;

		return curTime - _last_access_time_;
	}
	void reset()
	{
		timespec tm;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tm);

		_last_access_time_ = tm.tv_sec * 1000000 + tm.tv_nsec / 1000;
	}
private:
	uint64_t _last_access_time_;
};
