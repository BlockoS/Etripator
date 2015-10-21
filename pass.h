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
#ifndef PASS_H
#define PASS_H

/**
 * (todo)
 */
typedef int (*initialize_callback)(void *callback_data);
/**
 * (todo)
 */
typedef int (*iterate_callback)(void *callback_data);
/**
 * (todo)
 */
typedef int (*finalize_callback)(void *callback_data);

/**
 * (todo)
 */
typedef struct
{
    initialize_callback initialize;
    iterate_callback iterate;
    finalize_callback finalize;
    void *data;
} Pass;

/**
 * (todo)
 * \param [in] pass (todo)
 * \return 1 if the pass was succesfully performed, or 0 if an error occured.
 */
int perform(Pass *pass);

#endif // PASS_H
