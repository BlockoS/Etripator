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
 * Called before the loop.
 * Returns 1 upon success or 0 if and error occured.
 */
typedef int (*initialize_callback)(void *callback_data);
/**
 * Perform pass iteration.
 * @note Return value:
 *      * < 0 if an error occured
 *      *   0 for the last loop iteration.
 *      * > 0 upon success and another loop iteration needs to be performed.
 */
typedef int (*iterate_callback)(void *callback_data);
/**
 * Called after the loop.
 * Returns 1 upon success or 0 if and error occured.
 */
typedef int (*finalize_callback)(void *callback_data);

/**
 * Pass callbacks and payload.
 */
typedef struct
{
    /** Init. */
    initialize_callback initialize;
    /** Loop iterator. */
    iterate_callback iterate;
    /** Finalize. */
    finalize_callback finalize;
    /** Callback data. */
    void *data;
    /** Name. */
    const char *name;
} Pass;

/**
 * Run pass.
 * \param [in] pass Pass callbacks and data.
 * \return 1 if the pass was succesfully performed, or 0 if an error occured.
 */
int perform(Pass *pass);

#endif // PASS_H
