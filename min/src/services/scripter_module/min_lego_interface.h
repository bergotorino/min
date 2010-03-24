/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
 * Contact e-mail: min-support@lists.sourceforge.net
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
 *  @file       min_lego_interface.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Lego Interface.
 */

#ifndef MIN_LEGO_INTERFACE_H
#define MIN_LEGO_INTERFACE_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <dllist.h>
#include <scripter_keyword.h>
#include <scripter_common.h>
#include <min_parser.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct _LegoBasicType LegoBasicType;
typedef struct _LegoBasicType _LBT_;
typedef struct _LegoBasicType LegoStartType;
typedef struct _LegoBasicType LegoEndType;
/* ------------------------------------------------------------------------- */
/** The type of the piece. */
typedef enum {
        ELegoStart,     /**< indicates the beginning of script */
        ELegoEnd,       /**< indicates the end of script */
        ELegoPassive,   /**< indicates every keyword that don't require work */
        ELegoActive,    /**< indicates keyword that require some fancy work */
        ELegoLoop,      /**< indicates the beginning of loop */
        ELegoEndloop,   /**< indicates the end of loop */
        ELegoTimeLoop,  /**< indicates the beginning of time loop */
        ELegoEndTimeloop,/**< indicates the end of time loop */
        ELegoBreakloop, /**< indicates a loop breaking point */
        ELegoIfBlock,   /**< indicates start of if/else block */
        ELegoElseBlock, /**< indicates start of else block */
        ELegoEndifBlock /**< indicates end of if/else block */
} TLegoType;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Basic piece. */
struct _LegoBasicType {
        /** The type of the piece. 
         *  This variable have to be the first in every structure that 
         *  represents the particular lego type. */
        TLegoType       type_;
        LegoBasicType  *next_;  /**< connection to the next element. */
};
/* ------------------------------------------------------------------------- */
/** Describes keywords in the script that do not require work on uengine side
 *  for example: title keyword. */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        MinItemParser *line_;          /**< Line of script. */
} LegoPassiveType;
/* ------------------------------------------------------------------------- */
/** Describes keywords in the script that require work on uengine side
 *  for example: create/delete/run/callfunction keywords. */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        MinItemParser *line_;          /**< Line of script. */
} LegoActiveType;
/* ------------------------------------------------------------------------- */
/** Loop piece which indicates the loop starting point. This piece is linked 
 *  from Endloop piece. */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */

        /* Normal loop stuff */

        unsigned int    oryginal_;      /**< Number of loops or timeout [ms] */
        TSBool          timeout_;       /**< Indicates if it is timeout loop */

        /* Timeouted loop stuff */

        struct timeval  oryginal2_;     /**< The time the loop should last */
        struct timeval  endtime_;       /**< The time the loop should end  */
        TSBool          started_;       /**< Indocates if the loop is running*/
} LegoLoopType;
/* ------------------------------------------------------------------------- */
/** Endloop piece. This one is used to controll the script flow. It depends
 *  on counter_/oryginal_ values if next_ points to loopback_ or beyondloop_
 *  because we are always performing next step using next_ pointer.  */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        LegoLoopType   *loopback_;      /**< Loopback */
        LegoBasicType  *beyondloop_;    /**< Next line to be executed */

        /* Normal loop stuff */

        unsigned int   *oryginal_;      /**< Ptr to the number of loops */
        unsigned int    counter_;       /**< Counter of performed loops */
        TSBool          timeout_;       /**< Indicates if it is timeout loop */

        /* Timeouted loop stuff */

        struct timeval *oryginal2_;     /**< Time the loop should last. */
        struct timeval *endtime_;       /**< The end time for loop */
        TSBool         *started_;       /**< Indicates if loop has started */
} LegoEndloopType;
/* ------------------------------------------------------------------------- */
/** TimeLoop piece. Acts in the same way as Loop piece, but is used with
 *  timeouted loops */
typedef struct {
        TLegoType       type_;          /**< The type of the piece.        */
        LegoBasicType  *next_;          /**< Next piece in the queue       */
        TScripterKeyword keyword_;      /**< Script keyword.               */
        struct timeval  oryginal_;      /**< The time the loop should last */
        struct timeval  endtime_;       /**< The time the loop should end  */
        TSBool          started_;       /**< Indocates if the loop is running*/
} LegoTimeLoopType;
/* ------------------------------------------------------------------------- */
/** EndtimeLoop piece. Acts in the same way as Endloop piece, but is used with
 *  timeouted loops */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        struct timeval *oryginal_;      /**< The time the loop should last */
        struct timeval *endtime_;       /**< The time the loop should end  */
        TSBool         *started_;       /**< Indocates if the loop is running*/
} LegoEndTimeloopType;
/* ------------------------------------------------------------------------- */
/** Loop piece which breaks out from current loop.  */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
} LegoBreakloopType;
/* ------------------------------------------------------------------------- */
/** Piece which indicates the beginning of conditional block */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        char           *condition_;     /**< Condition for if  */
        LegoBasicType  *else_;          /**< Else branch */
        LegoBasicType  *block_end_;     /**< End of if block */
} LegoIfBlockType;
/* ------------------------------------------------------------------------- */
/** Piece which indicates the beginning of else block */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        LegoBasicType  *prev_;          /**< Cond */
} LegoElseBlockType;
/* ------------------------------------------------------------------------- */
/** Piece which indicates the end of else/if block */
typedef struct {
        TLegoType       type_;          /**< The type of the piece. */
        LegoBasicType  *next_;          /**< Next piece in the queue */
        TScripterKeyword keyword_;      /**< Script keyword. */
        LegoBasicType  *else_;          /**< back pointer to else branch */
} LegoEndifBlockType;
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            mli_create (MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
void            mli_destroy ();
/* ------------------------------------------------------------------------- */
LegoBasicType  *mli_get_legosnake ();
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_LEGO_INTERFACE */
/* End of file */
