/*
  void ADC_SetGain(){
  if(ADS1015_Mode==true){                                                    //FOR ADS1015 12-BIT ADC MODEL
    if(ADC_GainSelect==0){ads.setGain(GAIN_TWOTHIRDS);ADC_BitReso=3.0000;}   // Gain: 2/3x  Range: +/- 6.144V
    else if(ADC_GainSelect==1){ads.setGain(GAIN_ONE);ADC_BitReso=2.0000;}    // Gain: 1x    Range: +/- 4.096V
    else if(ADC_GainSelect==2){ads.setGain(GAIN_TWO);ADC_BitReso=1.0000;}    // Gain: 2x    Range: +/- 2.048V
  }
  else{                                                                      //FOR ADS1115 16-BIT ADC MODEL
    if(ADC_GainSelect==0){ads.setGain(GAIN_TWOTHIRDS);ADC_BitReso= 0.1875;}  // Gain: 2/3x  Range: +/- 6.144V
    else if(ADC_GainSelect==1){ads.setGain(GAIN_ONE);ADC_BitReso= 0.125;}    // Gain: 1x    Range: +/- 4.096V
    else if(ADC_GainSelect==2){ads.setGain(GAIN_TWO);ADC_BitReso= 0.0625;}   // Gain: 2x    Range: +/- 2.048V
  }
  }
*/

void checkConfig(INA226 &ina)
{
  Serial.print("Mode:                  ");
  switch (ina.getMode())
  {
    case INA226_MODE_POWER_DOWN:      Serial.println("Power-Down"); break;
    case INA226_MODE_SHUNT_TRIG:      Serial.println("Shunt Voltage, Triggered"); break;
    case INA226_MODE_BUS_TRIG:        Serial.println("Bus Voltage, Triggered"); break;
    case INA226_MODE_SHUNT_BUS_TRIG:  Serial.println("Shunt and Bus, Triggered"); break;
    case INA226_MODE_ADC_OFF:         Serial.println("ADC Off"); break;
    case INA226_MODE_SHUNT_CONT:      Serial.println("Shunt Voltage, Continuous"); break;
    case INA226_MODE_BUS_CONT:        Serial.println("Bus Voltage, Continuous"); break;
    case INA226_MODE_SHUNT_BUS_CONT:  Serial.println("Shunt and Bus, Continuous"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Samples average:       ");
  switch (ina.getAverages())
  {
    case INA226_AVERAGES_1:           Serial.println("1 sample"); break;
    case INA226_AVERAGES_4:           Serial.println("4 samples"); break;
    case INA226_AVERAGES_16:          Serial.println("16 samples"); break;
    case INA226_AVERAGES_64:          Serial.println("64 samples"); break;
    case INA226_AVERAGES_128:         Serial.println("128 samples"); break;
    case INA226_AVERAGES_256:         Serial.println("256 samples"); break;
    case INA226_AVERAGES_512:         Serial.println("512 samples"); break;
    case INA226_AVERAGES_1024:        Serial.println("1024 samples"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Bus conversion time:   ");
  switch (ina.getBusConversionTime())
  {
    case INA226_BUS_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_BUS_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_BUS_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_BUS_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_BUS_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_BUS_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_BUS_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_BUS_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Shunt conversion time: ");
  switch (ina.getShuntConversionTime())
  {
    case INA226_SHUNT_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_SHUNT_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_SHUNT_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_SHUNT_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_SHUNT_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_SHUNT_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_SHUNT_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_SHUNT_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Max possible current:  ");
  Serial.print(ina.getMaxPossibleCurrent());
  Serial.println(" A");

  Serial.print("Max current:           ");
  Serial.print(ina.getMaxCurrent());
  Serial.println(" A");

  Serial.print("Max shunt voltage:     ");
  Serial.print(ina.getMaxShuntVoltage());
  Serial.println(" V");

  Serial.print("Max power:             ");
  Serial.print(ina.getMaxPower());
  Serial.println(" W");
}

void resetVariables() {
  secondsElapsed = 0;
  energySavings  = 0;
  daysRunning    = 0;
  timeOn         = 0;
}
void Read_Sensors() {

  /////////// TEMPERATURE SENSOR /////////////
  if (sampleStoreTS <= avgCountTS) {                           //TEMPERATURE SENSOR - Lite Averaging
    TS = TS + analogRead(TempSensor);
    sampleStoreTS++;
  } else {
    TS = TS / sampleStoreTS;
    TSlog = log(ntcResistance * (4095.00 / TS - 1.00));
    temperature = (1.0 / (1.009249522e-03 + 2.378405444e-04 * TSlog + 2.019202697e-07 * TSlog * TSlog * TSlog)) - 273.15+10;
    sampleStoreTS = 0;
    TS = 0;
  }

  /////////// VOLTAGE & CURRENT SENSORS /////////////
  VSI = 0.0000;      //Clear Previous Input Voltage
  VSO = 0.0000;      //Clear Previous Output Voltage
  CSI = 0.0000;      //Clear Previous Input Current
  CSO = 0.0000;      //Clear Previous Output Current

  //VOLTAGE SENSOR - Instantenous Averaging
 /*
  for (int i = 0; i < avgCountVS; i++) {
    VSI = VSI + ads.computeVolts(ads.readADC_SingleEnded(3));
    VSO = VSO + ads.computeVolts(ads.readADC_SingleEnded(1));
    VSI = VSI + ina1.readBusVoltage()*(R1_VOLTAGE1+R2_VOLTAGE1)/R1_VOLTAGE1;
    VSO = VSO + ina2.readBusVoltage()*(R1_VOLTAGE2+R2_VOLTAGE2)/R1_VOLTAGE2;
  }
  voltageInput  = (VSI/avgCountVS)*inVoltageDivRatio;
  voltageOutput = (VSO/avgCountVS)*outVoltageDivRatio;
  voltageInput  = VSI/avgCountVS;
  voltageOutput = VSO/avgCountVS;
 */
  voltageInput  = ina1.readBusVoltage() * inVoltageDivRatio;
  voltageOutput = ina2.readBusVoltage() * outVoltageDivRatio;


  //CURRENT SENSOR - Instantenous Averaging
 /*
  for (int i = 0; i < avgCountCS; i++) {
    CSI = CSI + ads.computeVolts(ads.readADC_SingleEnded(2));
    CSI = CSI + ina1.readShuntCurrent();
    CSO = CSO + ina2.readShuntCurrent();
  }
  CSI_converted = (CSI/avgCountCS)*1.3300;
  currentInput  = ((CSI_converted-currentMidPoint)*-1)/currentSensV;
  CSI_converted = (CSI/avgCountCS);
  CSO_converted = (CSO/avgCountCS);
 */
  CSI_converted = ina1.readShuntCurrent();
  CSO_converted = ina2.readShuntCurrent();
  currentInput  = CSI_converted;

  if (currentInput < 0) {
    currentInput = 0.0000;
  }
  if (voltageOutput <= 0) {
    currentOutput = 0.0000;
  }
  //else{currentOutput = (voltageInput*currentInput)/voltageOutput;}
  else {
    currentOutput = CSO_converted;
  }

  //POWER SOURCE DETECTION
  if (voltageInput <= 3 && voltageOutput <= 3) {
    inputSource = 0; //System is only powered by USB port
  }
  else if (voltageInput > voltageOutput)    {
    inputSource = 1; //System is running on solar as power source
  }
  else if (voltageInput < voltageOutput)    {
    inputSource = 2; //System is running on batteries as power source
  }

  //////// AUTOMATIC CURRENT SENSOR CALIBRATION ////////
  if (buckEnable == 0 && FLV == 0 && OOV == 0) {
    //currentMidPoint = ((CSI/avgCountCS)*1.3300)-0.003;
    currentMidPoint = (CSI / avgCountCS) - 0.003;
  }

  //POWER COMPUTATION - Through computation
  //powerInput      = voltageInput*currentInput;
  powerInput      = ina1.readBusPower();
  //powerOutput     = voltageInput*currentInput*efficiencyRate;
  powerOutput     = ina2.readBusPower();
  outputDeviation = (voltageOutput / voltageBatteryMax) * 100.000;

  //STATE OF CHARGE - Battery Percentage
  batteryPercent  = ((voltageOutput - voltageBatteryMin) / (voltageBatteryMax - voltageBatteryMin)) * 101;
  batteryPercent  = constrain(batteryPercent, 0, 100);

  //TIME DEPENDENT SENSOR DATA COMPUTATION
  currentRoutineMillis = millis();
  if (currentRoutineMillis - prevRoutineMillis >= millisRoutineInterval) { //Run routine every millisRoutineInterval (ms)
    prevRoutineMillis = currentRoutineMillis;                          //Store previous time
    Wh = Wh + (powerInput / (3600.000 * (1000.000 / millisRoutineInterval))); //Accumulate and compute energy harvested (3600s*(1000/interval))
    kWh = Wh / 1000.000;
    MWh = Wh / 1000000.000;
    daysRunning = timeOn / (86400.000 * (1000.000 / millisRoutineInterval)); //Compute for days running (86400s*(1000/interval))
    timeOn++;                                                          //Increment time counter
  }

  //OTHER DATA
  secondsElapsed = millis() / 1000;                                    //Gets the time in seconds since the was turned on  and active
  energySavings  = electricalPrice * (Wh / 1000.0000);                 //Computes the solar energy saving in terms of money (electricity flag rate)
}
