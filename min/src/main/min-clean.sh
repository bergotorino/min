#!/bin/sh
# ------------------------------------------------------------------------------
# This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
# and/or its subsidiary(-ies).
# Contact: Konrad Marek Zapalowicz
# Contact e-mail: DG.MIN-Support@nokia.com
#  
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, version 2 of the License. 
# 
# This program is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
# more details. You should have received a copy of the GNU General Public 
# License along with this program. If not,  see <http://www.gnu.org/licenses/>.
# ------------------------------------------------------------------------------


CMDLN_ARGS="$@"
export CMDLN_ARGS
FORCE=0
ANSWER="n"

# ------------------------------------------------------------------------------
# Destroys ipc MQ if exists. Depends if ipcs and ipcrm tools are available.
destroy_mq() {
        # Check for ipcs
        if [ ! -z "$(which ipcs)" ]; then
                # Check for ipcrm
                if [ ! -z "$(which ipcrm)" ]; then
                        # Check for awk
                        if [ ! -z "$(which awk)" ]; then

                                echo "Checking for MQ left behind..."

                                # Fetch info about MQs in a system
                                mq=`ipcs|tail -2|head -1|awk '{print $2}'`
                                mqid="msqid"

                                # If MQ exists we need to destroy it
                                if [ $mq != $mqid ]; then
                                        ipcrm -q $mq
                                        echo "Queue "$mq" removed from the system..."
                                fi
                        fi
                fi
        fi
}
# ------------------------------------------------------------------------------
# Destroys shared memory segment if exists. Depends if ipcs and ipcrm tools are
# available.
destroy_shm() {
        # Check for ipcs
        if [ ! -z "$(which ipcs)" ]; then
                # Check for ipcrm
                if [ ! -z "$(which ipcrm)" ]; then
                        # Check for awk
                        if [ ! -z "$(which awk)" ]; then

                                echo "Checking for SHM left behind..."

                                # Fetch info about SHMs in a system
                                Shm=`ipcs -m | awk '{if ($1!="0x00000000" && $4=="660")  print $2}'`
                                for S in $Shm; do
                                        ipcrm -m $S
                                        echo "SHM "$S" removed from the system..."
                                done
                        fi
                fi
        fi
}
# ------------------------------------------------------------------------------
# Kills min and tmc if running.
kill_min() {
        # Check for killall
        if [ ! -z "$(which killall)" ]; then
                # Check for grep
                if [ ! -z "$(which grep)" ]; then
                        # Check for wc
                        if [ ! -z "$(which wc)" ]; then

                                # 1. Killing MIN

                                echo "Checking for active min processes..."

                                var=`ps aux | grep min | wc -l`

                                # One process is grep min, second this script
                                # any other is The MIN, and needs to be killed.

                                # 1. Gently
                                if [ $var -gt 1 ]; then

                                        if [ $FORCE -eq 0 ]; then
                                                echo "MIN already running!"
                                                echo "Would you like to kill it? Y/n?"
                                                read ANSWER
                                        else
                                                ANSWER="Y"
                                        fi

                                        Y="Y"
                                        if [ $ANSWER = "$Y" ]; then
                                                echo "Attempting to kill gently MIN..."
                                                killall min
                                                sleep 3
                                        fi
                                fi

                                # 2. Force
                                var=`ps aux | grep min | wc -l`

                                # One process is grep min, second this script
                                # any other is The MIN, and needs to be killed.
                                if [ $var -gt 1 ]; then
                                        if [ $ANSWER = "$Y" ]; then
                                                echo "Attempting to force kill of MIN..."
                                                killall -9 min
                                                sleep 3
                                        fi
                                fi

                                # 2. Killing TMCs

                                echo "Checking for active tmc processes..."

                                var=`ps aux | grep tmc | wc -l`

                                # One process is grep tmc,
                                # any other is The MIN, and needs to be killed.

                                # 1. Gently
                                yksi=1
                                if [ $var -gt $yksi ]; then
                                        if [ $ANSWER = "$Y" ]; then
                                                echo "Attempting to kill gently TMCs..."
                                                killall tmc
                                                sleep 3
                                        fi
                                fi

                                # 2. Force
                                var=`ps aux | grep tmc | wc -l`

                                # One process is grep tmc,
                                # any other is The MIN, and needs to be killed.
                                if [ $var -gt 1 ]; then
                                        if [ $ANSWER = "$Y" ]; then
                                                echo "Attempting to force kill of TMCs..."
                                                killall -9 tmc
                                                sleep 3
                                        fi
                                fi
                        fi
                fi
        fi
}
# ------------------------------------------------------------------------------
# Here goes the MAIN of the script

FORCE_opt="--force"
force_opt="-f"

# -- Detecting Force and Create
ii=0
opt=$()
realrun=1

# -- Cleaning stuff
if [ $realrun -eq 1 ]; then
        # kill_min
        destroy_mq
        destroy_shm
fi

 
# -- Running MIN
#min.bin $CMDLN_ARGS

# ------------------------------------------------------------------------------
