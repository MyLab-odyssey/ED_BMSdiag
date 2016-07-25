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
//! \version 0.5.0
//--------------------------------------------------------------------------------

#define VERBOSE 1                //!< VERBOSE mode will output individual cell data
#define EXPDATA 1                //!< EXPDATA mode will output experimental / NOT VERIFIED data
#define HELP 1                   //!< HELP menu active
#define NLG6TEST 1               //!< Test if the NLG6 fast charger is installed

#include <mcp_can.h>
#include <Timeout.h>
#include <Cmd.h>
#include "canDiag.h"

//Global definitions
char* const PROGMEM version = "0.5.0";
#define FAILURE F("* Measurement failed *")
#define MSG_OK F("OK")
#define MSG_FAIL F("F")
#define MSG_DOT F(".")

#define CS     10                //!< chip select pin of MCP2515 CAN-Controller
#define CS_SD  8                 //!< CS for SD card, if you plan to use a logger...
MCP_CAN CAN0(CS);                //!< Set CS pin

canDiag DiagCAN;
BatteryDiag_t BMS;
ChargerDiag_t NLG6;
CoolingSub_t CLS;

CTimeout CAN_Timeout(5000);     //!< Timeout value for CAN response in millis
CTimeout CLI_Timeout(500);      //!< Timeout value for CLI polling in millis
CTimeout LOG_Timeout(30000);    //!< Timeout value for LOG activity in millis

//Menu levels
typedef enum {MAIN, subBMS, subNLG6, subCS} submenu_t;

//deviceStatus struct to store menu settings
typedef struct {
  submenu_t menu = MAIN;
  boolean NLG6present = false;
  unsigned int timer = 30;
  bool logging = false;
  unsigned int logCount = 0;
} deviceStatus_t;

deviceStatus_t myDevice;
