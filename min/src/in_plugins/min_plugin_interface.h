/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
 * Contact e-mail: DG.MIN-Support@nokia.com
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */


/**
 *  @file       min_plugin_interface.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN plugin interface
 */

#ifndef MIN_PLUGIN_INTERFACE_H
#define MIN_PLUGIN_INTERFACE_H

#include "min_engine_api.h"

/* ------------------------------------------------------------------------- */
/** Binds callbacks to pointers
 *  @param out_callback [out] structure with callbacks called from component
 *                            that use plugin.
 *  @param in_callback [in] structure with callback called from the plugin.
 */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback);
/* ------------------------------------------------------------------------- */
/** Initializes the plugin.
 */
void pl_open_plugin ();
/* ------------------------------------------------------------------------- */
/** Deinitializes the plugin.
 */
void pl_close_plugin ();
/* ------------------------------------------------------------------------- */
/** Unbinds plugin, callbacks are set to NULLs
 */
void pl_detach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback);
/* ------------------------------------------------------------------------- */
#endif  /* MIN_PLUGIN_INTERFACE_H */
/* End of file */
