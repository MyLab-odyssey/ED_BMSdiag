//--------------------------------------------------------------------------------
// (c) 2016 by MyLab-odyssey
//
// Licensed under "MIT License (MIT)", see license file for more information.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER OR CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//--------------------------------------------------------------------------------
//! \file    ED_BMSdiag.h
//! \brief   Definitions and structures for the main program ED_BMSdiag.ino
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.4.2
//--------------------------------------------------------------------------------

#define VERBOSE 1                //!< VERBOSE mode will output individual cell data
#define EXPDATA 1                //!< EXPDATA mode will output experimental / NOT VERIFIED data

#include <mcp_can.h>
#include <Timeout.h>
#include "canDiag.h"

//Global definitions
char* const PROGMEM version = "0.4.2";
char* const PROGMEM SPACER = "-----------------------------------------";
char* const PROGMEM FAILURE = "---------- Measurement failed !----------";
char* const PROGMEM MSG_OK = "OK";
char* const PROGMEM MSG_FAIL = "F";
char* const PROGMEM MSG_DOT = ".";

#define CS     10                //!< chip select pin of MCP2515 CAN-Controller
#define CS_SD  8                 //!< CS for SD card, if you plan to use a logger...
MCP_CAN CAN0(CS);                //!< Set CS pin

canDiag DiagCAN;
BatteryDiag_t BMS;

CTimeout CAN_Timeout(5000);     //!< Timeout value for CAN response in millis

