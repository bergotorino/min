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
 *  @file       min_lego_interface.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Lego Interface.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <min_logger.h>
#include <min_lego_interface.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern DLList  *defines;
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
/* ------------------------------------------------------------------------- */
/** Stack for loop begin pieces, so that we can hande nested loops. */
LOCAL DLList   *loopstack = INITPTR;
/** the prepcompilled script that is represented by linked pieces. */
LOCAL LegoBasicType *legosnake = INITPTR;
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Creates Start piece. 
 *  @return pointer to the allocated Start piece.
 */
LOCAL LegoStartType *mli_create_start ();
/* ------------------------------------------------------------------------- */
/** Creates End piece.
 *  @return pointer to the allocated End piece.
 */
LOCAL LegoEndType *mli_create_end ();
/* ------------------------------------------------------------------------- */
/** Creates Passive piece.
 *  @param keyword [in] the keyword that comes from the script.
 *  @param mip [in] the script line which includes the keyword. 
 *  @return pointer to the allocated Passive piece.
 */
LOCAL LegoPassiveType *mli_create_passive (TScripterKeyword keyword,
                                           MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Creates Active piece.
 *  @param keyword [in] the keyword that comes from the script.
 *  @param mip [in] the script line which includes the keyword. 
 *  @return pointer to the allocated Active piece.
 */
LOCAL LegoActiveType *mli_create_active (TScripterKeyword keyword,
                                         MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Creates Start Loop piece.
 *  @param keyword [in] the keyword that comes from the script.
 *  @param loops [in] number of times the loop will be executed. 
 *  @param timeout [in] indicates normal loop or timeout loop.
 *  @return pointer to the allocated Start Loop piece.
 *
 *  NOTE: The loops parameter meaning depends on timeout parameter. If it is
 *        timeouted loop then loops indicates the time in [ms] otherwise
 *        loops indicates the number of loop iterations.
 *
 *  NOTE: The created piece is also pushed to the stack. It also handles 
 *        creation of the stack.
 */
LOCAL LegoLoopType *mli_create_loop (TScripterKeyword keyword,
                                     unsigned int loops, TSBool timeout);
/* ------------------------------------------------------------------------- */
/** Creates End Loop piece.
 *  @param keyword [in] the keyword that comes from the script.
 *  @return pointer to the allocated End Loop piece.
 *
 *  NOTE: The matching Start Loop piece is taken from the stack. It also
 *        handled destroying of the stack if empty.
 */
LOCAL LegoEndloopType *mli_create_endloop (TScripterKeyword keyword);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL LegoStartType *mli_create_start ()
{
        LegoStartType  *retval = INITPTR;
        retval = NEW (LegoStartType);
        retval->type_ = ELegoStart;
        retval->next_ = INITPTR;
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL LegoEndType *mli_create_end ()
{
        LegoEndType    *retval = INITPTR;
        retval = NEW (LegoEndType);
        retval->type_ = ELegoEnd;
        retval->next_ = INITPTR;
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL LegoPassiveType *mli_create_passive (TScripterKeyword keyword,
                                           MinItemParser * mip)
{
        LegoPassiveType *retval = INITPTR;
        if (mip == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        retval = NEW (LegoPassiveType);
        retval->type_ = ELegoPassive;
        retval->next_ = INITPTR;
        retval->keyword_ = keyword;
        retval->line_ = mip;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL LegoActiveType *mli_create_active (TScripterKeyword keyword,
                                         MinItemParser * mip)
{
        LegoActiveType *retval = INITPTR;
        if (mip == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        retval = NEW (LegoActiveType);
        retval->type_ = ELegoActive;
        retval->next_ = INITPTR;
        retval->keyword_ = keyword;
        retval->line_ = mip;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL LegoLoopType *mli_create_loop (TScripterKeyword keyword,
                                     unsigned int loops, TSBool timeout)
{
        LegoLoopType   *retval = INITPTR;
        retval = NEW (LegoLoopType);
        retval->type_ = ELegoLoop;
        retval->next_ = INITPTR;
        retval->keyword_ = keyword;
        retval->oryginal_ = loops;
        retval->timeout_ = timeout;
        retval->started_ = ESFalse;
        
	retval->oryginal2_.tv_sec = loops/1000;
	retval->oryginal2_.tv_usec = (loops%1000) *1000;

        retval->endtime_.tv_sec = 0;
        retval->endtime_.tv_usec = 0;

        /* This one is also pushed to the stack */
        if (loopstack == INITPTR) {
                loopstack = dl_list_create ();
        } else {
                MIN_DEBUG ("Loopstack already exists");
        }
        dl_list_add (loopstack, (void *)retval);
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL LegoEndloopType *mli_create_endloop (TScripterKeyword keyword)
{
        DLListIterator  it = DLListNULLIterator;
        LegoEndloopType *retval = INITPTR;
        retval = NEW (LegoEndloopType);
        retval->type_ = ELegoEndloop;
        retval->next_ = INITPTR;
        retval->keyword_ = keyword;
        retval->loopback_ = INITPTR;
        retval->beyondloop_ = INITPTR;
        retval->oryginal_ = INITPTR;
        retval->counter_ = 0;
        retval->timeout_ = ESFalse;
        retval->oryginal2_ = INITPTR;
        retval->endtime_ = INITPTR;
        retval->started_ = INITPTR;

        /* Loopback should point to the LegoLoopType taken from stack. If
         * stack becames empty it will be destroyed. */
        it = dl_list_tail (loopstack);
        retval->loopback_ = (LegoLoopType *) dl_list_data (it);
        dl_list_remove_it (it);
        if (dl_list_tail (loopstack) == DLListNULLIterator) {
                dl_list_free (&loopstack);
        } else {
                MIN_DEBUG ("Loopstack not empty");
        }

        /* Some variables depend on values from LegoLoopType piece. */
        retval->oryginal_ = &retval->loopback_->oryginal_;
        retval->timeout_ = retval->loopback_->timeout_;
        retval->oryginal2_ = &retval->loopback_->oryginal2_;
        retval->endtime_ = &retval->loopback_->endtime_;
        retval->started_ = &retval->loopback_->started_;

        /* By default next step will take us to the beginning of the loop */
        retval->next_ = (LegoBasicType *) retval->loopback_;

        return retval;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
void mli_create (MinSectionParser * msp)
{
        MinItemParser *mip = INITPTR;
        char           *token = INITPTR;
        TScripterKeyword keyword = EKeywordUnknown;
        LegoBasicType  *last = INITPTR;
        LegoBasicType  *tmp = INITPTR;
        int             loop_iter = 0;
        char           *loop_param = INITPTR;
        TSBool          loop_tmout = ESFalse;
        DLListIterator  it = DLListNULLIterator;
        struct define  *def = INITPTR;
        int             ret = 0;


        if (msp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        /* 1) Create START piece */
        last = mli_create_start ();
        legosnake = last;

        /* 2) Construct the rest of snake */
        mip = mmp_get_item_line (msp, "", ESNoTag);
        while (mip != INITPTR) {

                /* first we have to replace possible define labels with
                 * its value. */
                it = dl_list_head (defines);
                while (it != DLListNULLIterator) {
                        def = (struct define *)dl_list_data (it);
                        ret = mip_replace (mip, def->label_, def->value_);

                        if (ret == -1) {
                                MIN_WARN ("Error when replacing,"
                                          " invalid parameter to mip_replace");
                        }

                        it = dl_list_next (it);
                }

                /* then we can interprete line */
                if (token != INITPTR)
                        DELETE (token);
                mip_get_string (mip, "", &token);
                keyword = get_keyword (token);

                switch (keyword) {
                case EKeywordTitle:
                case EKeywordSkipIt:
                        tmp =
                            (LegoBasicType *) mli_create_passive (keyword,
                                                                  mip);
                        last->next_ = tmp;
                        last = tmp;
                        mip = mmp_get_next_item_line (msp);
                        break;
                case EKeywordClassName:
                case EKeywordCreate:
                case EKeywordDelete:
                case EKeywordPause:
                case EKeywordPrint:
                case EKeywordResume:
                case EKeywordCancel:
                case EKeywordRelease:
                case EKeywordRequest:
                case EKeywordRun:
                case EKeywordSet:
                case EKeywordUnset:
                case EKeywordWait:
                case EKeywordAllocate:
                case EKeywordFree:
                case EKeywordInterference:
                case EKeywordRemote:
                case EKeywordAllowNextResult:
                case EKeywordComplete:
                case EKeywordCancelIfError:
                case EKeywordTimeout:
                case EKeywordSleep:
                case EKeywordVar:
                case EKeywordExpect:
                case EKeywordSendreceive:
                        tmp =
                            (LegoBasicType *) mli_create_active (keyword,
                                                                 mip);
                        last->next_ = tmp;
                        last = tmp;
                        mip = mmp_get_next_item_line (msp);
                        break;
                case EKeywordLoop:
                        /* Take number of iterations */
                        mip_get_int (mip, "", &loop_iter);
                        /* Take parameters (msec or passrate) */
                        mip_get_next_string (mip, &loop_param);

                        if (loop_param != INITPTR) {
                                loop_tmout = ESTrue;
                        } else {
                                MIN_DEBUG ("No. of iterations: %d", 
                                             loop_iter);
                        }

                        tmp =
                            (LegoBasicType *) mli_create_loop (keyword,
                                                               loop_iter,
                                                               loop_tmout);
                        last->next_ = tmp;
                        last = tmp;
                        mip_destroy (&mip);
                        mip = mmp_get_next_item_line (msp);
                        break;
                case EKeywordEndloop:
                        tmp = (LegoBasicType *) mli_create_endloop (keyword);
                        last->next_ = tmp;
                        last = tmp;
                        mip_destroy (&mip);
                        mip = mmp_get_next_item_line (msp);
                        break;
                case EKeywordUnknown:
                        MIN_ERROR ("Unknown keyword [%s]", token);
                        mip_destroy (&mip);
                        mip = mmp_get_next_item_line (msp);
                        break;
                default:
                        MIN_ERROR ("Unknown keyword [INITPTR]");
                        mip_destroy (&mip);
                        mip = mmp_get_next_item_line (msp);
                        break;
                }
        }
        if (token != INITPTR)
                DELETE (token);
        /* 3) Add End element */
        tmp = mli_create_end ();
        last->next_ = tmp;
        last = tmp;
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
void mli_destroy ()
{
        LegoBasicType  *tmp = INITPTR;
        LegoBasicType  *next = INITPTR;
        LegoActiveType *lat = INITPTR;
        LegoLoopType   *llt = INITPTR;
        LegoEndloopType *let = INITPTR;

        if (legosnake == INITPTR) {
                MIN_WARN ("Lego snake does not exist");
                goto EXIT;
        }

        tmp = legosnake;

        while (tmp != INITPTR) {
                next = tmp->next_;
                switch (tmp->type_) {
                case ELegoPassive:
                case ELegoActive:
                        lat = (LegoActiveType *) tmp;
                        mip_destroy (&lat->line_);
                        DELETE (lat);
                        lat = INITPTR;
                        tmp = INITPTR;
                        break;
                case ELegoStart:
                case ELegoEnd:
                        DELETE (tmp);
                        tmp = INITPTR;
                        break;
                case ELegoLoop:
                        llt = (LegoLoopType *) tmp;
                        DELETE (llt);
                        llt = INITPTR;
                        tmp = INITPTR;
                        break;
                case ELegoEndloop:
                        let = (LegoEndloopType *) tmp;
                        DELETE (let);
                        let = INITPTR;
                        tmp = INITPTR;
                        break;
                default:
                        MIN_WARN ("Unknown Lego piece [%d]", tmp->type_);
                }
                tmp = next;
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LegoBasicType  *mli_get_legosnake ()
{
        return legosnake;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_lego_interface.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
