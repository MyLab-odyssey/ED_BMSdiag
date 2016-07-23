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
//! \file    BMS_dfs.h
//! \brief   Definitions and structures for the BMS module.
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.2.0
//--------------------------------------------------------------------------------
#ifndef BMS_DFS_H
#define BMS_DFS_H

//Definitions for BMS
#define DATALENGTH 440
#define CELLCOUNT 93
#define RAW_VOLTAGES 0           //!< Use RAW values or calc ADC offset voltage

//Data structure soft-/hardware-revision
typedef struct {
  byte rev[3];                   //!< year, week, patchlevel
} Revision_t;

//Data structure for statistics (min, mean, max values)
template<typename T>
struct Stats{
  unsigned int min;              //!< minimum
  T mean;                        //!< average
  unsigned int max;              //!< maximum
};

//BMS data structure
typedef struct {     
  Stats<unsigned int> ADCCvolts; //!< average cell voltage in mV, no offset
                                 //!< minimum and maximum cell voltages in mV, add offset +1500
  int ADCvoltsOffset;            //!< calculated offset between RAW cell voltages and ADCref, about 90mV
  
  Stats<unsigned int> Cap_As;    //!< cell capacity statistics from BMS measurement cycle
  float Cap_meas_quality;        //!< some sort of estimation factor??? after measurement cycle
  float Cap_combined_quality;    //!< some sort of estimation factor??? constantly updated
  unsigned int LastMeas_days;    //!< days elapsed since last successful measurement
  
  Stats<float> Cvolts;           //!< calculated statistics from individual cell voltage query              
  int CV_min_at;                 //!< cell number with voltage mininum in pack
  int CV_max_at;                 //!< cell number with voltage maximum in pack
  float Cvolts_stdev;            //!< calculated standard deviation (populated)
  
  Stats<float> Ccap_As;          //!< cell capacity statistics calculated from individual cell data
  int CAP_min_at;                //!< cell number with capacity mininum in pack
  int CAP_max_at;                //!< cell number with capacity maximum in pack
  
  int CapInit;                   //!< battery initial capacity (As/10), at a certain temperature maybe 45 degC
  int CapLoss;                   //!< battery capacity loss (x/1000) in %, reflects aging (distance related?)
  
  unsigned long HVoff_time;      //!< HighVoltage contactor off time in seconds
  unsigned long HV_lowcurrent;   //!< counter time of no current, reset e.g. with PLC heater or driving
  unsigned int OCVtimer;         //!< counter time in seconds to reach OCV state
  
  byte Day;                      //!< day of battery production
  byte Month;                    //!< month of battery production
  byte Year;                     //!< year of battery production

  Revision_t sw;                 //!< soft-revision
  Revision_t hw;                 //!< hardware-revision
  
  byte hour;                     //!< time in car: hour
  byte minutes;                  //!< time in car: minutes
  
  float SOC;                     //!< State of Charge, as reported by vehicle dash
  unsigned int realSOC;          //!< is this the internal SOC value in % (x/10)
    
  int Amps;                      //!< battery current in ampere (x/32)  
  int Power;                     //!< power in kW, drivetrain and charge ((x/8192)-1)*300
  
  float HV;                      //!< total voltage of HV system in V
  float LV;                      //!< 12V onboard voltage / LV system
  
  unsigned long ODO;             //!< Odometer count
  
  int Temps[13];                 //!< three temperatures per battery unit (1 to 3)
                                 //!< + max, min, mean and coolant-in temperatures
  unsigned int Isolation;        //!< Isolation in DC path, resistance in kOhm
  unsigned int DCfault;          //!< Flag to show DC-isolation fault
  
  byte HVcontactState;           //!< contactor state: 0 := OFF, 2 := ON
  long HVcontactCyclesLeft;      //!< counter related to ON/OFF cyles of the car
  long HVcontactCyclesMax;       //!< static, seems to be maxiumum of contactor cycles 
  byte UnknownCounter[3];        //!< some incremental counter - for what?
} BatteryDiag_t; 

const PROGMEM byte rqBattHWrev[4]                 = {0x03, 0x22, 0xF1, 0x50};
const PROGMEM byte rqBattSWrev[4]                 = {0x03, 0x22, 0xF1, 0x51};
const PROGMEM byte rqBattTemperatures[4]          = {0x03, 0x22, 0x02, 0x01}; 
const PROGMEM byte rqBattModuleTemperatures[4]    = {0x03, 0x22, 0x02, 0x02};
const PROGMEM byte rqBattHVstatus[4]              = {0x03, 0x22, 0x02, 0x04};
const PROGMEM byte rqBattADCref[4]                = {0x03, 0x22, 0x02, 0x07};
const PROGMEM byte rqBattVolts[4]                 = {0x03, 0x22, 0x02, 0x08};
const PROGMEM byte rqBattIsolation[4]             = {0x03, 0x22, 0x02, 0x09};
const PROGMEM byte rqBattAmps[4]                  = {0x03, 0x22, 0x02, 0x03};
const PROGMEM byte rqBattDate[4]                  = {0x03, 0x22, 0x03, 0x04};
const PROGMEM byte rqBattCapacity[4]              = {0x03, 0x22, 0x03, 0x10};
const PROGMEM byte rqBattHVContactorCyclesLeft[4] = {0x03, 0x22, 0x03, 0x0B};
const PROGMEM byte rqBattHVContactorMax[4]        = {0x03, 0x22, 0x03, 0x0C};
const PROGMEM byte rqBattHVContactorState[4]      = {0x03, 0x22, 0xD0, 0x00};

//Experimental readouts
const PROGMEM byte rqBattCapInit[4]               = {0x03, 0x22, 0x03, 0x05};
const PROGMEM byte rqBattCapLoss[4]               = {0x03, 0x22, 0x03, 0x09};
const PROGMEM byte rqBattUnknownCounter[4]        = {0x03, 0x22, 0x01, 0x01};

#endif // of #ifndef BMS_DFS_H
