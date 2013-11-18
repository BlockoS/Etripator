/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

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
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
}
#endif

/* Main */
int main(int argc, char** argv)
{
	int res;
	lua_State *state;
	int count;
	int i, j;
	const char* code = 
		"mpr = {}\n"
		"mpr[4] = 1\n"
		"mpr[1] = 17\n"
		"test = {\n"
		"    { first=\"foo\", second=\"bar\" },\n"
		"    { first=\"bozo\", second=\"plop\" },\n"
		"}\n";

	/*
	conf.mpr[3] = 3
	conf.mpr[4] = 3
	conf.bank = 5
	conf.org  = 8000
	conf.size = -1

	decode(conf, file("font.asm")) 
	*/

	// PrintMsgOpenFile("log.txt");

	state = luaL_newstate();
	res = luaL_dostring(state, code);
	if(!res)
	{
		lua_getglobal(state, "mpr");
		for(lua_pushnil(state); lua_next(state, -2); lua_pop(state, 1))
		{
			// INFO_MSG("\n%s %s\n",
			// 	lua_typename(state, lua_type(state, -2)),
			// 	lua_typename(state, lua_type(state, -1))
			// );
			
			i = lua_tointeger(state, -2);
			j = lua_tointeger(state, -1);

			// INFO_MSG("\n%d %d\n", i, j);
		}

		lua_getglobal(state, "test");
		luaL_checktype(state, -1, LUA_TTABLE);
		count = lua_rawlen(state, -1);
		lua_pushnil(state);
		while(lua_next(state, -2) != 0)
		{
			// INFO_MSG("\n%s %s\n",
			// 	lua_typename(state, lua_type(state, -2)),
			// 	lua_typename(state, lua_type(state, -1))
			// );

			lua_getfield(state, -1, "first");
			if(!lua_isnil(state, -1))
			{
				if(lua_isstring(state, -1))
				{
					// INFO_MSG("%s", (char*)lua_tostring(state, -1));
				}
			}
			lua_pop(state, 1);
			lua_getfield(state, -1, "second");
			if(!lua_isnil(state, -1))
			{
				if(lua_isstring(state, -1))
				{
					// INFO_MSG("%s", (char*)lua_tostring(state, -1));
				}
			}
			lua_pop(state, 2);
		}
		lua_pop(state, 2);
	}
	lua_close(state);

	// INFO_MSG("%s", get_error_string(e_invalid_parameter));

	return res;
}
