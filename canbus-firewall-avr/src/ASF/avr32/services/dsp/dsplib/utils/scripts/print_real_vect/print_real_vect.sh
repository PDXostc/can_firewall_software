#!/bin/sh
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

# Copyright (C) 2006-2014, Atmel Corporation All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. The name of ATMEL may not be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
# SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#********************* Configuration *********************
#********* Arguments *******************
COM_PORT='COM1'
COM_BAUDRATE='57600'
#********* Software path **************
DATAGET="`pwd`/../../PROGRAMS/WINDOWS/DATA_GET/DataGet.exe"
DATAEXTRACT="`pwd`/../../PROGRAMS/WINDOWS/DATA_EXTRACT/DataExtract.exe"
DATAPRINT="`pwd`/../../PROGRAMS/WINDOWS/DATA_PRINT/DataPrint.exe"
#*********************************************************

TEMP1_FILE="./temp1"
TEMP2_FILE="./temp2"

# To retreive data from the COM port
$DATAGET -f $TEMP1_FILE $COM_PORT $COM_BAUDRATE
# To extract them
$DATAEXTRACT $TEMP1_FILE "%i %f" 2 > $TEMP2_FILE
# And finaly print them
$DATAPRINT $TEMP2_FILE

rm $TEMP1_FILE 2> /dev/null
rm $TEMP2_FILE 2> /dev/null
