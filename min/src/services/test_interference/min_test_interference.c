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
 *  @file       min_test_interference.c
 *  @version    0.1
 *  @brief      Test interference system.
 *              
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <signal.h>
#include <errno.h>
#include <tmc.h>
#include <tmc_ipc.h>
#include <min_logger.h>
#include <min_test_interference.h>
#include <pthread.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */


/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL inline void msleep (int period);
/* ------------------------------------------------------------------------- */
LOCAL int         check_package_avail ();
/* ------------------------------------------------------------------------- */
LOCAL void       *ti_cpu_controller (void *Arg);
/* ------------------------------------------------------------------------- */
LOCAL void       *ti_mem_controller (void *Arg);
/* ------------------------------------------------------------------------- */
LOCAL void       *ti_io_controller (void *Arg);
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Sleep for number of miliseconds given in argument
 */
LOCAL inline void msleep(int period)
{
        sleep ((int)(period/1000));
        usleep (1000*(period%1000));
}
/* ------------------------------------------------------------------------- */
/** Check that the tools used in interference are available in the system.
 *  @return 0 if tools available, -1 if not
 */
LOCAL int check_package_avail ()
{
        int             retval = 0;
        if ((access ("/usr/bin/cpuload", X_OK) != 0) ||
            (access ("/usr/bin/memload", X_OK) != 0) ||
            (access ("/usr/bin/ioload", X_OK)  != 0))
                retval = -1;

        return retval;
}
/* ------------------------------------------------------------------------- */
/** Implements the CPU load interference.
 * @param *Arg test interference structure.
 */
LOCAL void     *ti_cpu_controller (void *Arg)
{
        testInterference *Ainterference = (testInterference *) Arg;
        MIN_TRACE ("-->");
        if (Ainterference->idle_time_ != 0) {
                MIN_DEBUG ("enter IDLE/BUSY cycle (%d/%d)",
                           Ainterference->idle_time_,
                           Ainterference->busy_time_);
                while (Ainterference->terminated_ == ESFalse) {
                        switch (Ainterference->instance_paused_) {
                        case (ERunning):
                                msleep(Ainterference->busy_time_);
                                kill (Ainterference->instance_pid_, SIGSTOP);
                                msleep(Ainterference->idle_time_);
                                kill (Ainterference->instance_pid_, SIGCONT);
                                break;
                        case (EPausePending):
                                kill (Ainterference->instance_pid_, SIGSTOP);
                                Ainterference->instance_paused_ = EPaused;
                                break;
                        case (EResumePending):
                                kill (Ainterference->instance_pid_, SIGCONT);
                                break;
                        case (EPaused):
                                usleep (1000);
                        }
                }
        } else {
                while (Ainterference->terminated_ == ESFalse) {
                        /*no need to do anything */
                        usleep (10000);
                }
        }
        kill (Ainterference->instance_pid_, SIGKILL);
        DELETE (Ainterference);
        MIN_TRACE ("<--");
        return NULL;            //only to satisfy compiler
}
/* ------------------------------------------------------------------------- */
/** Implements the memory load interference.
 * @param *Arg test interference structure.
 */
LOCAL void     *ti_mem_controller (void *Arg)
{
        FILE           *output = NULL;
        int             num = 0;
        pid_t           inst_pid;
        char          **exec_args = NEW2 (char *, 3);
        testInterference *Ainterference = (testInterference *) Arg;
        MIN_TRACE ("-->");
	if (exec_args == NULL) {
		MIN_FATAL ("OOM");
		return NULL;
	}
	exec_args[0] = exec_args[1] = exec_args[2] = NULL;
        if (Ainterference->idle_time_ != 0) {
                MIN_DEBUG ("enter IDLE/BUSY cycle (%d/%d)",
                           Ainterference->idle_time_,
                           Ainterference->busy_time_);
                while (Ainterference->terminated_ == ESFalse) {
                        switch (Ainterference->instance_paused_) {
                        case (ERunning):
                                MIN_DEBUG ("ERunning");
                                inst_pid = fork ();
                                if (inst_pid == 0) {
                                        output = fopen ("/dev/null", "w+");
                                        num = fileno (output);
                                        dup2 (num, STDOUT_FILENO);
                                        dup2 (num, STDERR_FILENO);
                                        exec_args[0] = "/usr/bin/memload";
                                        exec_args[1] = NEW2 (char, 3);
                                        sprintf (exec_args[1], "%d",
                                                 Ainterference->memory_load_);
                                        exec_args[2] = NULL;
                                        execv (exec_args[0], exec_args);
					fclose (output);
                                } else {
                                        Ainterference->instance_pid_ =
                                            inst_pid;
                                        msleep(Ainterference->busy_time_);
                                        kill (Ainterference->instance_pid_,
                                              SIGKILL);
                                        waitpid(Ainterference->instance_pid_,NULL,0);
                                        msleep(Ainterference->idle_time_);
                                }
                                break;
                        case (EPausePending):
                                MIN_DEBUG ("EPausePending");
                                Ainterference->instance_paused_ = EPaused;
                                break;
                        case (EPaused):
                                MIN_DEBUG ("EPaused");
                                msleep(Ainterference->idle_time_);
                                break;
                        case (EResumePending):
                                MIN_DEBUG ("EResumePending");
                                Ainterference->instance_paused_ = ERunning;
                                break;
                        default:
                                MIN_WARN ("fault in interference status");
                        }
                }
        } else {
                inst_pid = fork ();
                if (inst_pid == 0) {
                        MIN_DEBUG ("starting interference tool");
                        output = fopen ("/dev/null", "w+");
                        num = fileno (output);
                        dup2 (num, STDOUT_FILENO);
                        dup2 (num, STDERR_FILENO);
                        exec_args[0] = "/usr/bin/memload";
                        exec_args[1] = NEW2 (char, 3);
                        sprintf (exec_args[1], "%d",
                                 Ainterference->memory_load_);
                        exec_args[2] = NULL;
                        execv (exec_args[0], exec_args);
			fclose (output);
                } else {
                        Ainterference->instance_pid_ = inst_pid;
                }
                while (Ainterference->terminated_ == ESFalse) {
                        usleep (1000);
                }
                kill (Ainterference->instance_pid_, SIGKILL);
        }
        DELETE (Ainterference);
	if (exec_args) {
		DELETE (exec_args [1]);
		DELETE (exec_args [2]);
		DELETE (exec_args);
	}

        MIN_TRACE ("<--");
        return NULL;            //only to satisfy compiler
}
/* ------------------------------------------------------------------------- */
/** Implements the i/o load interference.
 *  @param *Arg test interference structure.
 */
LOCAL void     *ti_io_controller (void *Arg)
{
        testInterference *Ainterference = (testInterference *) Arg;
        MIN_TRACE ("-->");
        if (Ainterference->idle_time_ != 0) {
                MIN_DEBUG ("enter IDLE/BUSY cycle (%d/%d)",
                           Ainterference->idle_time_,
                           Ainterference->busy_time_);
                while (Ainterference->terminated_ == ESFalse) {
                        switch (Ainterference->instance_paused_) {
                        case (ERunning):
                                msleep(Ainterference->busy_time_);
                                kill (Ainterference->instance_pid_, SIGSTOP);
                                msleep(Ainterference->idle_time_);
                                kill (Ainterference->instance_pid_, SIGCONT);
                                break;
                        case (EPausePending):
                                kill (Ainterference->instance_pid_, SIGSTOP);
                                Ainterference->instance_paused_ = EPaused;
                                break;
                        case (EResumePending):
                                Ainterference->instance_paused_ = ERunning;
                                kill (Ainterference->instance_pid_, SIGCONT);
                                break;
                        case (EPaused):
                                usleep (1000);
				break;
                        default:
                                MIN_WARN ("fault in interference status");
                        }
                }
        } else {
                while (Ainterference->terminated_ == ESFalse) {
                        /*no need to do anything */
                        usleep (10000);
                }
        }
        kill (Ainterference->instance_pid_, SIGKILL);
        DELETE (Ainterference);
        MIN_TRACE ("<--");
        return NULL;            //only to satisfy compiler
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
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
testInterference *ti_start_interference_timed (TInterferenceType aType,
                                               int aIdleTime,
                                               int aBusyTime, int aLoadValue)
{
        testInterference *interf_controller = INITPTR;
        pid_t           ins_pid = 0;
        FILE           *output = NULL;
        int             num = 0, ret;
        char          **exec_args = NULL;
        pthread_t       watcher;
        void           *(*controller) (void *) = NULL;
        MIN_TRACE ("-->");
        interf_controller = NEW (testInterference);
        interf_controller->type_ = aType;
        interf_controller->idle_time_ = aIdleTime;
        interf_controller->busy_time_ = aBusyTime;
        if (check_package_avail () != 0) {
                DELETE (interf_controller);
                MIN_INFO
                    ("Could not find interference tools. Check if sp-stress is available");
                return NULL;
        }
        switch (aType) {
        case (ECpuLoad):
                interf_controller->cpu_load_ = aLoadValue;
                interf_controller->memory_load_ = 0;
                interf_controller->io_fname_ = NULL;
                interf_controller->instance_paused_ = ESFalse;
                break;
        case (EMemLoad):
                interf_controller->cpu_load_ = 0;
                interf_controller->memory_load_ = aLoadValue;
                interf_controller->io_fname_ = NULL;
                interf_controller->instance_paused_ = ESFalse;
                break;
        case (EIOLoad):
                interf_controller->cpu_load_ = 0;
                interf_controller->memory_load_ = 0;
                interf_controller->io_fname_ = "/tmp/intworkf";
                interf_controller->instance_paused_ = ESFalse;
                break;
        }
        /*now fork disturbance process and controller proces if necessary */
        ins_pid = fork ();

        if (ins_pid != 0) {
                interf_controller->instance_pid_ = ins_pid;
                /*leave some time for sp-stress tool to start */
                sleep (3);
                /*now start the thread with controller/timer */
                switch (aType) {
                case (ECpuLoad):
                        controller = ti_cpu_controller;
                        break;
                case (EMemLoad):
                        controller = ti_mem_controller;
                        break;
                case (EIOLoad):
                        controller = ti_io_controller;
                }
                interf_controller->terminated_ = ESFalse;
                interf_controller->instance_paused_ = ERunning;
                pthread_create (&watcher, NULL, controller,
                                (void *)interf_controller);
                interf_controller->controller_handle_ = watcher;
        } else {
                /*change this process into proper
                   sp-stress tool now. */
                /*first, redirect output to /dev/null , to
                   prevent messing up of console ui. */
                output = fopen ("/dev/null", "w+");
                num = fileno (output);
                dup2 (num, STDOUT_FILENO);
                dup2 (num, STDERR_FILENO);
                exec_args = NEW2 (char *, 3);
                switch (interf_controller->type_) {
                case (ECpuLoad):
                        exec_args[0] = "/usr/bin/cpuload";
                        exec_args[1] = NEW2 (char, 3);
                        sprintf (exec_args[1], "%d",
                                 interf_controller->cpu_load_);
                        exec_args[2] = NULL;
                        execv (exec_args[0], exec_args);
			fclose (output);
                        break;
                case (EMemLoad):
                        /*memload is treated differently, since pausing it
                           doesn't stop the memory from being taken */
			fclose (output);
                        exit (0);
                case (EIOLoad):
                        ret = system
                            ("dd if=/dev/zero of=/tmp/workfile count=128 bs=128");
                        exec_args[0] = "/usr/bin/ioload";
                        exec_args[1] = NEW2 (char, strlen("/tmp/workfile") + 1);
			strncpy (exec_args[1], "/tmp/workfile", 
				 strlen ("/tmp/workfile"));
                        exec_args[2] = NULL;
                        execv (exec_args[0], exec_args);
			fclose (output);
                        break;
                }
        }
        MIN_TRACE ("<--");
	if (exec_args) {
		DELETE (exec_args[1]);
		DELETE (exec_args[2]);
		DELETE (exec_args);
	}

        return interf_controller;
}
/* ------------------------------------------------------------------------- */
/** Function that creates test interference “instance” and starts the 
 * interference process.
 * @param aType type of interference
 * @param aLoadValue value of load - percent for cpu, megabytes for memory,
 * ignored for ioload
 * @return pointer to interference struct.
 */
testInterference *ti_start_interference (TInterferenceType aType,
                                         int aLoadValue)
{
        return ti_start_interference_timed (aType, 0, 0, aLoadValue);
}
/* ------------------------------------------------------------------------- */
/** Stops the interference and destroys interference struct.
 * @param aInterference interference structure pointer, 
 * destroyed inside the function, no need to deallocate it.
 * @return none.
 */
void ti_stop_interference (testInterference * aInterference)
{
        MIN_TRACE ("-->");
        aInterference->terminated_ = ESTrue;
        pthread_join (aInterference->controller_handle_, NULL);
        MIN_TRACE ("<--");
}
/* ------------------------------------------------------------------------- */
/** Pauses the interference 
 * @param aInterference interference structure pointer, 
 * @return none.
 */
void ti_pause_interference (testInterference * aInterference)
{
        aInterference->instance_paused_ = EPausePending;
}
/* ------------------------------------------------------------------------- */
/** Resumes the interference 
 * @param aInterference interference structure pointer, 
 * @return none.
 */
void ti_resume_interference (testInterference * aInterference)
{
        aInterference->instance_paused_ = EResumePending;
}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */

/* End of file */
