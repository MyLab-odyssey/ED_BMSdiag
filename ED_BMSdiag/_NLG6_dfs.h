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
//! \file    NLG6_dfs.h
//! \brief   Definitions and structures for the NLG6-Charger module.
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.2.0
//--------------------------------------------------------------------------------
#ifndef NLG6_DFS_H
#define NLG6_DFS_H

//Definitions for NLG6
#define TEMP_OFFSET 40

//NLG6 data structure
typedef struct {       
  unsigned int MainsAmps[3];      //!< AC current of L1, L2, L3
  unsigned int MainsVoltage[3];   //!< AC voltage of L1, L2, L3
  byte Amps_setpoint;             //!< AC charging current set by user in BC (Board Computer)
  unsigned int AmpsCableCode;     //!< Maximum current cable (resistor coded)
  unsigned int AmpsChargingpoint; //!< Maxiumum current of chargingpoint
  unsigned int DC_Current;        //!< DC current measured by charger
  unsigned int DC_HV;             //!< DC HV measured by charger
  byte LV;                        //!< 12V onboard voltage of Charger DC/DC
  byte Temps[7];                  //!< internal temperatures in charger unit and heat exchanger
  byte ReportedTemp;              //!< mean temperature, reported by charger
  byte SocketTemp;                //!< temperature of mains socket charger
  byte CoolingPlateTemp;          //!< temperature of cooling plate 
} ChargerDiag_t; 

const PROGMEM byte rqChargerPresent[4]            = {0x03, 0x22, 0x01, 0x01};
const PROGMEM byte rqChargerVoltages[4]           = {0x03, 0x22, 0x02, 0x26};
const PROGMEM byte rqChargerAmps[4]               = {0x03, 0x22, 0x02, 0x25};
const PROGMEM byte rqChargerSelCurrent[4]         = {0x03, 0x22, 0x02, 0x2A};
const PROGMEM byte rqChargerTemperatures[4]       = {0x03, 0x22, 0x02, 0x23}; 

#endif // of #ifndef NLG6_DFS_H
