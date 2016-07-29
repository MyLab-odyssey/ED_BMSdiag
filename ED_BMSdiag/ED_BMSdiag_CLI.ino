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
//! \file    ED_BMSdiag_CLI.ino
//! \brief   Functions for the Command Line Interface (CLI) menu system
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.5.0
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//! \brief   Setup menu items
//--------------------------------------------------------------------------------
void setupMenu() {
  cmdInit();

  if (HELP) {  
    cmdAdd("help", help);
    cmdAdd("?", help);
  } 
  cmdAdd("..", main_menu);
  cmdAdd("#", show_splash);
  cmdAdd("t", get_temperatures);
  cmdAdd("v", get_voltages);
  cmdAdd("bms", bms_sub);
  if (myDevice.NLG6present) {
    cmdAdd("nlg6", nlg6_sub);
  }
  cmdAdd("cs", cs_sub);
  cmdAdd("all", get_all);
  cmdAdd("log", set_logging);
  cmdAdd("info", show_info);
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get all datasets depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_all (int arg_cnt, char **args) {
  switch (myDevice.menu) {
    case subBMS:
      printBMSall();
      break;
    case subNLG6:
      printNLG6all();
      break;
    case subCS:
      printCLSall();
      break;
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get temperature values depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_temperatures (int arg_cnt, char **args) {
  switch (myDevice.menu) {
    case subBMS:
      if (DiagCAN.getBatteryTemperature(&BMS, false)){
        printBMStemperatures();
      }
      break;
    case subNLG6:
      if (DiagCAN.getChargerTemperature(&NLG6, false)){
        printNLG6temperatures();
      }
      break;
    case subCS:
      break;
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get voltages depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_voltages (int arg_cnt, char **args) {
  switch (myDevice.menu) {
    case subBMS:
      if (DiagCAN.getBatteryADCref(&BMS, false)){
        printBMS_CellVoltages();
      }
      break;
    case subNLG6:
      break;
    case subCS:
      break;
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to show a help page depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
#ifdef HELP
void help(int arg_cnt, char **args)
{
  switch (myDevice.menu) {
    case MAIN:
      Serial.println(F("* Main Menu:"));
      Serial.println(F("  BMS   Submenu"));
      Serial.println(F("  CS    Submenu"));
      if (myDevice.NLG6present) {
        Serial.println(F("  NLG6  Submenu"));
      }
      Serial.println(F("  help  List commands"));
      Serial.println(F("  log   Logging"));
      Serial.println(F("        [on/off] or [on/off] [time/s]"));
      Serial.println();
      Serial.println(F("  info  Show logging state"));
      Serial.println(F("  #     Show real time data"));
      break;
    case subBMS:
      Serial.println(F("* BMS Menu:"));
      Serial.println(F("  all   Get complete dataset"));
      Serial.println(F("  v     Get voltages"));
      Serial.println(F("  t     Get temperatures"));
      break;
    case subNLG6:
      Serial.println(F("* NLG6 Menu:"));
      Serial.println(F("  all   Get complete dataset"));
      Serial.println(F("  t     Get temperatures"));
      break;
    case subCS:
     Serial.println(F("* CS Menu:"));
      Serial.println(F("  all   Get complete dataset"));
      break;
  }   
}
#endif

//--------------------------------------------------------------------------------
//! \brief   Callback to show a splash screen for startup or by command
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void show_splash(int arg_cnt, char **args) {
   //Read CAN-Bus IDs related to BMS (sniff traffic)
   byte selected[] = {0,1,2,3,4,5,6,7};
   ReadCANtraffic_BMS(selected, sizeof(selected));
   printSplashScreen();
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get all datasets depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void show_info(int arg_cnt, char **args)
{
  //Serial.print(F("Usable Memory: ")); Serial.println(getFreeRam());
  //Serial.print(F("Menu: ")); Serial.println(myDevice.menu);
  Serial.print(F("NLG6: ")); Serial.println(myDevice.NLG6present);
  Serial.print(F("Logging interval: ")); Serial.print(myDevice.timer, DEC);
  Serial.println(F(" s"));
  Serial.print(F("Logging is "));
  if (myDevice.logging)
  {
    Serial.println(F("ON"));
  }
  else {
    Serial.println(F("OFF"));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to start logging and / or set parameters
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void set_logging(int arg_cnt, char **args) {
  if (arg_cnt > 2) {
    myDevice.timer = (unsigned int) cmdStr2Num(args[2], 10);
  } 
  if (arg_cnt > 1) {
    if (strcmp(args[1], "on") == 0) {
      myDevice.logging = true;
      LOG_Timeout.Reset(myDevice.timer * 1000);
      myDevice.logCount = 0;
    }
    if (strcmp(args[1], "off") == 0) {
      myDevice.logging = false;
    }
  } else {
    if (arg_cnt == 1) {
      show_info(arg_cnt, args);
    }
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to activate main menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void main_menu (int arg_cnt, char **args) {
  myDevice.menu = MAIN;
  set_cmd_display("");            //reset command prompt to "CMD >>"
}

//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the BMS sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void bms_sub (int arg_cnt, char **args) {
  myDevice.menu = subBMS;
  set_cmd_display("BMS >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) {
      get_all(arg_cnt, args);
    }
    if (strcmp(args[1], "t") == 0) {
      get_temperatures(arg_cnt, args);
    }
    if (strcmp(args[1], "v") == 0) {
      get_voltages(arg_cnt, args);
    }
  } else {

  }
}


//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the NLG6 sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void nlg6_sub (int arg_cnt, char **args) {
  myDevice.menu = subNLG6;
  set_cmd_display("NLG6 >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) {
      get_all(arg_cnt, args);
    }
    if (strcmp(args[1], "t") == 0) {
      get_temperatures(arg_cnt, args);
    }
  } else {

  }
}


//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the Cooling sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void cs_sub (int arg_cnt, char **args) {
  myDevice.menu = subCS;
  set_cmd_display("CS >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) get_all(arg_cnt, args);
  } else {

  }
}

//--------------------------------------------------------------------------------
//! \brief   Funcion to check if a NLG6 fast charger is installed 
//! \return  report status / if present (boolean)
//--------------------------------------------------------------------------------
boolean nlg6_installed() {
  myDevice.NLG6present =  DiagCAN.NLG6ChargerInstalled(false);
  return myDevice.NLG6present;
}

//--------------------------------------------------------------------------------
//! \brief   Logging data. Call queryfunctions and output the data
//--------------------------------------------------------------------------------
void logdata(){
  //Read CAN-Bus IDs related to BMS (sniff traffic)
  byte selected[] = {0,1,2,3,4};
  ReadCANtraffic_BMS(selected, sizeof(selected));

  byte selected2[] ={8,11};
  getBMSdata(selected2, sizeof(selected2));

  getNLG6data();

  getCLSdata();

  if (myDevice.logCount == 0) {
    //Print Header
    myDevice.logCount++;
    Serial.println();
    Serial.println(F("SOC;rSOC;A;kW;V;Vc,min;Vc,max;Tb/C;L1/V;L1/A;L2/V;L2/A;L3/V;L3/A;HV/V;HV/A;Tr/C;Tc/C;Ti/C;Tm/C;P/%;Tp/C"));
  }
  //Print logged values
  Serial.print(BMS.SOC,1); Serial.print(F(";"));
  Serial.print((float) BMS.realSOC / 10.0, 1); Serial.print(F(";"));
  Serial.print((float) BMS.Amps/32.0, 2); Serial.print(F(";"));
  if (BMS.Power != 0) {
    Serial.print(((float) (BMS.Power / 8192.0) -1) * 300, 2); Serial.print(F(";"));
  } else {
    Serial.print(F("0.00")); Serial.print(F(";"));
  }
  Serial.print(BMS.HV,1); Serial.print(F(";"));
  Serial.print(BMS.ADCCvolts.min); Serial.print(F(";"));
  Serial.print(BMS.ADCCvolts.max); Serial.print(F(";"));
  Serial.print((float) BMS.Temps[9] / 64, 1); Serial.print(F(";"));
  Serial.print(NLG6.MainsVoltage[0] / 10.0, 1); Serial.print(F(";")); Serial.print(NLG6.MainsAmps[0] / 10.0, 1); Serial.print(F(";"));
  Serial.print(NLG6.MainsVoltage[1] / 10.0, 1); Serial.print(F(";")); Serial.print(NLG6.MainsAmps[1] / 10.0, 1); Serial.print(F(";"));
  Serial.print(NLG6.MainsVoltage[2] / 10.0, 1); Serial.print(F(";")); Serial.print(NLG6.MainsAmps[2] / 10.0, 1); Serial.print(F(";"));
  Serial.print(NLG6.DC_HV / 10.0, 1); Serial.print(F(";")); Serial.print(NLG6.DC_Current / 10.0, 1); Serial.print(F(";"));
  Serial.print(NLG6.ReportedTemp - TEMP_OFFSET, DEC); Serial.print(F(";"));
  Serial.print(NLG6.CoolingPlateTemp - TEMP_OFFSET, DEC); Serial.print(F(";"));
  Serial.print(NLG6.SocketTemp - TEMP_OFFSET, DEC); Serial.print(F(";"));
  Serial.print(CLS.CoolingTemp / 8.0,1); Serial.print(F(";"));
  Serial.print(CLS.CoolingPumpRPM / 255.0 * 100.0, 1); Serial.print(F(";"));
  Serial.print(CLS.CoolingPumpTemp - 50);
  Serial.println();
}

