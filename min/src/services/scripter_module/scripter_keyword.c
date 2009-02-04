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
 *  @file       scripter_keyword.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Scripter keyword
 *              interface.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <min_logger.h>
#include <scripter_keyword.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
TScripterKeyword get_keyword (const char *label)
{
        TScripterKeyword retval = EKeywordUnknown;

        if (label == INITPTR) {
                MIN_ERROR ("parameter is INITPTR");
                goto EXIT;
        }
        if (strcmp (label, "createx") == 0) {
                retval = EKeywordCreate;
        } else if (strcmp (label, "delete") == 0) {
                retval = EKeywordDelete;
        } else if (strcmp (label, "title") == 0) {
                retval = EKeywordTitle;
        } else if (strcmp (label, "run") == 0) {
                retval = EKeywordRun;
        } else if (strcmp (label, "request") == 0) {
                retval = EKeywordRequest;
        } else if (strcmp (label, "release") == 0) {
                retval = EKeywordRelease;
        } else if (strcmp (label, "wait") == 0) {
                retval = EKeywordWait;
        } else if (strcmp (label, "set") == 0) {
                retval = EKeywordSet;
        } else if (strcmp (label, "unset") == 0) {
                retval = EKeywordUnset;
        } else if (strcmp (label, "loop") == 0) {
                retval = EKeywordLoop;
        } else if (strcmp (label, "endloop") == 0) {
                retval = EKeywordEndloop;
        } /*else if (strcmp (label, "set") == 0) {
                retval = EKeywordSkipIt;
        } else if (strcmp (label, "unset") == 0) {
                retval = EKeywordSkipIt;
        }*/ else if (strcmp (label, "print") == 0) {
                retval = EKeywordPrint;
        } else if (strcmp (label, "pause") == 0) {
                retval = EKeywordPause;
        } else if (strcmp (label, "resume") == 0) {
                retval = EKeywordResume;
        } else if (strcmp (label, "cancel") == 0) {
                retval = EKeywordCancel;
        } else if (strcmp (label, "allocate") == 0) {
                retval = EKeywordAllocate;
        } else if (strcmp (label, "free") == 0) {
                retval = EKeywordFree;
        } else if (strcmp (label, "remote") == 0) {
                retval = EKeywordRemote;
        } else if (strcmp (label, "allownextresult") == 0) {
                retval = EKeywordAllowNextResult;
        } else if (strcmp (label, "allowerrorcodes") == 0) {
                retval = EKeywordAllowNextResult;
        } else if (strcmp (label, "complete") == 0) {
                retval = EKeywordComplete;
        } else if (strcmp (label, "canceliferror") == 0) {
                retval = EKeywordCancelIfError;
        } else if (strcmp (label, "timeout") == 0) {
                retval = EKeywordTimeout;
        } else if (strcmp (label, "sleep") == 0) {
                retval = EKeywordSleep;
        } else if (strcmp (label, "pausecombiner") == 0) {
                retval = EKeywordSleep;
        } else if (strcmp (label, "waittestclass") == 0) {
                /* We need to recognize this keyword, but we are using Events 
                   for that stuff so this keyword will be skipped. */
                retval = EKeywordSkipIt;
        } else if (strcmp (label, "priority") == 0) {
                retval = EKeywordSkipIt;
        } else if (strcmp (label, "expect") == 0) {
                retval = EKeywordExpect;
        } else if (strcmp (label, "sendreceive") == 0) {
                retval = EKeywordSendreceive;
        } else if (strcmp (label, "var") == 0) {
                retval = EKeywordVar;
        } else if (strcmp (label, "testinterference") == 0){
                retval = EKeywordInterference;
        } else if (strcmp (label, "if") == 0){
                retval = EKeywordIf;
        } else if (strcmp (label, "else") == 0){
                retval = EKeywordElse;
        } else if (strcmp (label, "endif") == 0){
                retval = EKeywordEndif;
        } else if (strcmp (label, "breakloop") == 0){
                retval = EKeywordBreakloop;
        } else {
                retval = EKeywordClassName;
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_scripter_if.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
