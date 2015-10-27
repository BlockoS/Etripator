/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#include "pass.h"
#include "message.h"

/**
 * Run pass.
 * \param [in] pass Pass callbacks and data.
 * \return 1 if the pass was succesfully performed, or 0 if an error occured.
 */
int perform(Pass *pass)
{
    int ret = 0;
    /* Sanity check. */
    if(NULL == pass)
    {
        ERROR_MSG("Invalid pass data.");
        return 0;
    }
    if( (NULL == pass->data) 
     || (NULL == pass->initialize)
     || (NULL == pass->finalize)
     || (NULL == pass->iterate) )
    {
        ERROR_MSG("Invalid callback data.");
        return 0;
    }
    
    /* Loop until iterations need to be performed or an error occured. */
    for(ret = pass->initialize(pass->data);
        ret > 0;
        ret = pass->iterate(pass->data) )
    {
        /* nothing atm */    
    }
    if(0 == ret)
    {
        ret = pass->finalize(pass->data);
    }
    return ret;
}
