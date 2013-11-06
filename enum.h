/*
    This file is part of Etripator,
    copyright (c) 2009--2013 Vincent Cruz.

    Etripator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Etripator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
*/
#if !defined(ENUM_DECLARATION)
#ifndef ENUM_DEFINITION
#define ENUM_DEFINITION

#define BEGIN_ENUM(enum_name) \
typedef enum \
{ \


#define ENUM(enum_element, description) enum_element, \


#define END_ENUM(enum_name) \
} enum_name##_t; \
const char* get_##enum_name##_string(enum_name##_t e); \

#endif ENUM_DEFINITION

#else
#undef BEGIN_ENUM
#undef ENUM
#undef END_ENUM

#define BEGIN_ENUM(enum_name) \
static const char* enum_name##_name [] = \
{ \


#define ENUM(enum_element, description) description, \

#define END_ENUM(enum_name) \
}; \
const char* get_##enum_name##_string(enum_name##_t e) \
{ \
	if(e >= sizeof(enum_name##_name) / sizeof(enum_name##_name[0])) \
	{ \
		return "unknown"; \
	} \
	return enum_name##_name[e]; \
} \

#endif
