/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       min_test_interference.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Event interface
 */

#ifndef MIN_TEST_INTERFERENCE_H
#define MIN_TEST_INTERFERENCE_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Interference Types */
typedef enum {
        ECpuLoad,
        EMemLoad,
        EIOLoad
} TInterferenceType;
/** pause status*/
typedef enum {
        ERunning,
        EPausePending,
        EPaused,
        EResumePending
} TPauseStatus;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */
typedef struct interference_controller_ {
        TInterferenceType       type_;
        pid_t                   controller_pid_;
        int                     idle_time_;
        int                     busy_time_;
        int                     cpu_load_;
        int                     memory_load_;
        char*                   io_fname_;
        pid_t                   instance_pid_;
        TPauseStatus            instance_paused_;
        TSBool                  terminated_;
        pthread_t               controller_handle_;
} testInterference;


/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Function that creates test interference “instance” and starts the 
 * interference process.
 * @param aType type of interference
 * @param aLoadValue value of load - percent for cpu, megabytes for memory,
 * ignored for ioload
 * @return pointer to interference struct.
*/

testInterference   *ti_start_interference (TInterferenceType aType,
                                               int aLoadValue
/** Function that creates test interference “instance” and starts the 
 * interference process, which will contiuously sleep and resume
 * for given time periods
 * @param aType type of interference
 * @param aIdleTime time for which process will be sleeping during cycle
 * @param aBusyTime time for which interference will be active
 * @param aLoadValue value of load - percent for cpu, megabytes for memory,
 * ignored for ioload
 * @return pointer to interference struct.
*/
                                              );
testInterference   *ti_start_interference_timed (TInterferenceType aType,
                                               int aIdleTime,
                                               int aBusyTime,
                                               int aLoadValue
                                              );

/** Stops the interference and destroys interference struct.
 * @param aInterference interference structure pointer, 
 * destroyed inside the function, no need to deallocate it.
 * @return none.
*/
void ti_stop_interference(testInterference* aInterference);

/** Pauses the interference 
 * @param aInterference interference structure pointer, 
 * @return none.
*/
void ti_pause_interference(testInterference* aInterference);

/** Resumes the interference 
 * @param aInterference interference structure pointer, 
 * @return none.
*/
void ti_resume_interference(testInterference* aInterference);



#endif                          /* MIN_TEST_INTERFERENCE_H */
