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
 *  @file       scripter_keyword.h
 *  @version    0.1
 *  @brief      This file contains common definitions for MIN Scripter.
 */

#ifndef SCRIPTER_KEYWORD_H
#define SCRIPTER_KEYWORD_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <min_common.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Script syntax keywords */
typedef enum {
        EKeywordCreate,         /**< keyword "create"                   */
        EKeywordClassName,      /**< call to some class                 */
        EKeywordDelete,         /**< keyword "delete"                   */
        EKeywordEndloop,        /**< keyword "EndLoop"                  */
        EKeywordLoop,           /**< keyword "loop"                     */
        EKeywordTitle,          /**< keyword "title"                    */
        EKeywordRequest,        /**< keyword "request"                  */
        EKeywordRun,            /**< keyword "run"                      */
        EKeywordSet,            /**< keyword "set"                      */
        EKeywordUnset,          /**< keyword "unset"                    */
        EKeywordWait,           /**< keyword "wait"                     */
        EKeywordPrint,          /**< keyword "print"                    */
        EKeywordResume,         /**< keyword "resume"                   */
        EKeywordCancel,         /**< keyword "cancel"                   */
        EKeywordSkipIt,         /**< things that we can skip            */
        EKeywordRelease,        /**< keyword "release"                  */
        EKeywordPause,          /**< keyword "pause"                    */
        EKeywordAllocate,       /**< keyword "allocate"                 */
        EKeywordFree,           /**< keyword "free"                     */
        EKeywordRemote,         /**< keyword "remote"                   */
        EKeywordAllowNextResult,/**< keyword "allownextresult"          */
        EKeywordComplete,       /**< keyword "complete"                 */
        EKeywordCancelIfError,  /**< keyword "canceliferror"            */
        EKeywordTimeout,        /**< keyword "timeout"                  */
        EKeywordSleep,          /**< keyword "sleep" or "pausecombiner" */
        EKeywordVar,            /**< keyword "var"                      */
        EKeywordExpect,         /**< keyword "expect"                   */
        EKeywordSendreceive,    /**< keyword "sendreceive"              */
        EKeywordInterference,   /**< keyword "testinterference"         */
        EKeywordIf,             /**< keyword "if"                       */
        EKeywordElse,           /**< keyword "else"                     */
        EKeywordEndif,          /**< keyword "endif"                    */
        EKeywordBreakloop,      /**< keyword "breakloop"                */
        EKeywordUnknown         /**< unknown keyword                    */
} TScripterKeyword;
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Translates literal keyword to mnemonic.
 *  @param label [in] keyword in literal form ("")
 *  @return mnemonic for keyword - enumerator value.
 */
TScripterKeyword get_keyword (const char *label);
/* ------------------------------------------------------------------------- */
#endif                          /* SCRIPTER_KEYWORD_H */
/* End of file */
