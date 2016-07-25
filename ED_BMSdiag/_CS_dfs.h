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
//! \file    CS_dfs.h
//! \brief   Definitions and structures for the Cooling- & Subsystems module.
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.3.0
//--------------------------------------------------------------------------------
#ifndef CS_DFS_H
#define CS_DFS_H

//CS data structure
typedef struct {       

  unsigned int  CoolingTemp;      //!< main cooling temperatur measurement / 8
  byte CoolingPumpTemp;           //!< temperature at cooling pump, offset 50
  byte CoolingPumpLV;             //!< 12V onboard voltage of cooling pump / 10
  unsigned int CoolingPumpAmps;   //!< current measured at cooling pump / 5
  byte CoolingPumpRPM;            //!< RPM in % of cooling pump (value / 255 * 100%)
  unsigned int CoolingPumpOTR;    //!< operating time record of cooling pump in hours
  unsigned int BatteryHeaterOTR;  //!< operating time record of PTC heater of battery
  byte BatteryHeaterON;           //!< Status of the PTC heater of the battery
  unsigned long VaccumPumpOTR;    //!< operating time record of vaccum pump
  int VaccumPumpPress1;           //!< pressure of vaccum pump measuerement #1 in mbar?
  int VaccumPumpPress2;           //!< pressure of vaccum pump measuerement #2 in mbar?

} CoolingSub_t; 

const PROGMEM byte rqCoolingTemp[4]               = {0x03, 0x22, 0x20, 0x47};
const PROGMEM byte rqCoolingPumpTemp[4]           = {0x03, 0x22, 0x23, 0x0A};
const PROGMEM byte rqCoolingPumpLV[4]             = {0x03, 0x22, 0x23, 0x08};
const PROGMEM byte rqCoolingPumpAmps[4]           = {0x03, 0x22, 0x23, 0x09};
const PROGMEM byte rqCoolingPumpRPM[4]            = {0x03, 0x22, 0xD0, 0x32}; 
const PROGMEM byte rqCoolingPumpOTR[4]            = {0x03, 0x22, 0x63, 0x09}; //operating time record
const PROGMEM byte rqBatteryHeaterOTR[4]          = {0x03, 0x22, 0x63, 0x21};
const PROGMEM byte rqBatteryHeaterON[4]           = {0x03, 0x22, 0xD3, 0x02};
const PROGMEM byte rqVacuumPumpOTR[4]             = {0x03, 0x22, 0x63, 0x03};
const PROGMEM byte rqVacuumPumpPress1[4]          = {0x03, 0x22, 0x20, 0x41};
const PROGMEM byte rqVacuumPumpPress2[4]          = {0x03, 0x22, 0x20, 0x43};

#endif // of #ifndef CS_DFS_H
