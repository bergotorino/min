/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       equal.h
 *  @version    0.1
 *  @brief      Header for equal.c
 */

#ifndef EQUAL_H
#define EQUAL_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */


/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */

/** Compares two values
 *  @param a pointer to value to be compared
 *  @param b pointer to value to be compared
 *  @returns 0 if values are equal, -1 if not
 */
int equal (const void *a, const void *b);


/* ------------------------------------------------------------------------- */


#endif                          /* EQUAL_H */
