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
 *  @file       tmc_ipc.c
 *  @version    0.1
 *  @brief      This file contains implementation of the TMC IPC part.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <tmc_ipc.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

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

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initializes TMC's inter process communication part
 *  @param tmcipi address of the TMC's inter process communication entity.
 *  @param receiver PID of the process to which messages will be adressed to.
 *
 *  Fills TMCIPCInterface structure with default data.
 */
void ip_init (TMCIPCInterface * tmcipi, int receiver)
{
        if (tmcipi == INITPTR) {
                MIN_ERROR ("TMCIPInterface is not initialized.");
                return;
        }

        tmcipi->mqid_ = mq_open_queue ((int)'a');
        tmcipi->sender_ = getpid ();
        tmcipi->receiver_ = receiver;

        tmcipi->in_.receiver_ = 0;
        tmcipi->in_.sender_ = 0;
        tmcipi->in_.type_ = 0;
        tmcipi->in_.param_ = 0;
        tmcipi->in_.desc_[0] = '\0';
        tmcipi->in_.message_[0] =  '\0';
}

/* ------------------------------------------------------------------------- */
/** Returns status of the initialization process.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 *  @return 0 when init was successfull, -1 otherwise.
 */
int ip_status (const TMCIPCInterface * tmcipi)
{
        if (tmcipi->mqid_ == -1) {
                MIN_ERROR ("TMCIPInterface is not initialized correctly.");
                return -1;
        }
        else {
                MIN_DEBUG ("TMCIPInterface is initialized correctly.");
                return 0;
        }
}

/* ------------------------------------------------------------------------- */
/** Sends MSG_OK message.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 */
void ip_send_ok (const TMCIPCInterface * tmcipi)
{
        if (tmcipi == INITPTR)
                return;

        mq_send_message2 (tmcipi->mqid_, tmcipi->receiver_, MSG_OK, 0,
                          "MSG_OK");
}

/* ------------------------------------------------------------------------- */
/** Sends MSG_KO message.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 *  @param errnum enumerated error value.
 *  @param desc textual description of the error.
 */
void ip_send_ko (const TMCIPCInterface * tmcipi, int errnum, const char *desc)
{
        if (tmcipi == INITPTR) {
                MIN_ERROR ("TMCIPInterface is not initialized");
                return;
        }

        mq_send_message2 (tmcipi->mqid_, tmcipi->receiver_, MSG_KO, errnum,
                          desc);
}

/* ------------------------------------------------------------------------- */
/** Sends MSG_TCD message.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 *  @param file configuration file that was used to retrieve test cases.
 *  @param it DLList iterator pointing on the beginning of the test cases list.
 *  @param module version 
 */
void ip_send_tcd (const TMCIPCInterface * tmcipi, const char *file,
                  DLListIterator it, int module_version)
{
        MsgBuffer       out;
        TestCaseInfo   *tci = INITPTR;
        if (tmcipi == INITPTR) {
                MIN_ERROR ("TMCIPInterface is not initialized.");
                return;
        }

        MIN_DEBUG ("%s:%s(%p,%s,%p)",__FILE__,__FUNCTION__,tmcipi,file,it);

        out.receiver_ = tmcipi->receiver_;
        out.sender_ = tmcipi->sender_;
        out.type_ = MSG_TCD;

        while (it != DLListNULLIterator) {

                tci = (TestCaseInfo *) dl_list_data (it);
                if (tci != INITPTR) {
                        out.param_ = tci->id_;
                        STRCPY (out.desc_, file, MaxFileName);
                        STRCPY (out.message_, tci->name_, MaxTestCaseName);
                        mq_send_message (tmcipi->mqid_, &out);
			if (module_version >= 200950 
			    && strlen (tci->desc_) > 0) {
				out.type_ = MSG_TCDESC;
				STRCPY (out.message_, tci->desc_, 
					MaxUsrMessage);
				mq_send_message (tmcipi->mqid_, &out);
			}
                } else {
                        MIN_WARN ("Test Case not found on list.");
                }
                it = dl_list_next (it);
        }
}

/* ------------------------------------------------------------------------- */
/** Sends MSG_RET message.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 *  @param result test case result, @see TPResult.
 *  @param desc textual description of the test result
 */
void ip_send_ret (const TMCIPCInterface * tmcipi, int result,
                  const char *desc)
{
        if (tmcipi == INITPTR) {
                MIN_ERROR ("TMCIPInterface is not initialized");
                return;
        }

        mq_send_message2 (tmcipi->mqid_, tmcipi->receiver_, MSG_RET, result,
                          desc);

}

/* ------------------------------------------------------------------------- */
/** Sends end mark for MSG_TCD transmission.
 *  @param tmcipi adress of the TMC's inter process communication entity.
 */
void ip_send_eotcd (const TMCIPCInterface * tmcipi)
{
        if (tmcipi == INITPTR) {
                MIN_ERROR ("TMCIPInterface is not initialized");
                return;
        }

        mq_send_message2 (tmcipi->mqid_, tmcipi->receiver_, MSG_TCD, 0, "\0");
}



/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
