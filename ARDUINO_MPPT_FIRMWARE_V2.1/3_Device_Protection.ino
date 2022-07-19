void backflowControl() {                                               //PV BACKFLOW CONTROL (INPUT MOSFET)
  if (output_Mode == 0) {
    bypassEnable = 1; //PSU MODE: Force backflow MOSFET on
  }
  else {                                                               //CHARGER MODE: Force backflow MOSFET on
    if (voltageInput > voltageOutput + voltageDropout) {
      bypassEnable = 1; //CHARGER MODE: Normal Condition - Turn on Backflow MOSFET (on by default when not in MPPT charger mode)
    }
    else {
      bypassEnable = 0; //CHARGER MODE: Input Undervoltage - Turn off bypass MOSFET and prevent PV Backflow (SS)
    }
  }
  digitalWrite(backflow_MOSFET, bypassEnable);                         //Signal backflow MOSFET GPIO pin
}

void Device_Protection() {
  //ERROR COUNTER RESET
  currentRoutineMillis = millis();
  if (currentErrorMillis - prevErrorMillis >= errorTimeLimit) {                                     //Run routine every millisErrorInterval (ms)
    prevErrorMillis = currentErrorMillis;                                                           //Store previous time
    if (errorCount < errorCountLimit) {
      errorCount = 0; //Reset error count if it is below the limit before x milliseconds
    }
    else {}                                                                                         // TO ADD: sleep and charging pause if too many errors persists
  }
  //FAULT DETECTION
  ERR = 0;                                                                                          //Reset local error counter
  backflowControl();                                                                                //Run backflow current protection protocol
  if (temperature > temperatureMax)                           {
    OTE = 1;  //OTE - OVERTEMPERATURE: System overheat detected
    ERR++;
    errorCount++;
  } else {
    OTE = 0;
  }
  if (currentInput > currentInAbsolute)                       {
    IOC = 1;  //IOC - INPUT  OVERCURRENT: Input current has reached absolute limit
    ERR++;
    errorCount++;
  } else {
    IOC = 0;
  }
  if (currentOutput > currentOutAbsolute)                     {
    OOC = 1;  //OOC - OUTPUT OVERCURRENT: Output current has reached absolute limit
    ERR++;
    errorCount++;
  } else {
    OOC = 0;
  }
  if (voltageOutput > voltageBatteryMax + voltageBatteryThresh) {
    OOV = 1;  //OOV - OUTPUT OVERVOLTAGE: Output voltage has reached absolute limit
    ERR++;
    errorCount++;
  } else {
    OOV = 0;
  }
  if (voltageInput < vInSystemMin && voltageOutput < vInSystemMin) {
    FLV = 1;  //FLV - Fatally low system voltage (unable to resume operation)
    ERR++;
    errorCount++;
  } else {
    FLV = 0;
  }

  if (output_Mode == 0) {                                                                           //PSU MODE specific protection protocol
    REC = 0; BNC = 0;                                                                               //Clear recovery and battery not connected boolean identifiers
    if (voltageInput < voltageBatteryMax + voltageDropout) {
      IUV = 1;  //IUV - INPUT UNDERVOLTAGE: Input voltage is below battery voltage (for psu mode only)
      ERR++;
      errorCount++;
    } else {
      IUV = 0;
    }
  }
  else {                                                                                            //Charger MODE specific protection protocol
    backflowControl();                                                                              //Enable backflow current detection & control
    if(voltageOutput<vInSystemMin)                   {BNC=1;ERR++;}      else{BNC=0;}               //BNC - BATTERY NOT CONNECTED (for charger mode only, does not treat BNC as error when not under MPPT mode)
    if (voltageInput < voltageBatteryMax + voltageDropout) {
      IUV = 1;  //IUV - INPUT UNDERVOLTAGE: Input voltage is below max battery charging voltage (for charger mode only)
      ERR++;
      REC = 1;
    } else {
      IUV = 0;
    }
  }
}
