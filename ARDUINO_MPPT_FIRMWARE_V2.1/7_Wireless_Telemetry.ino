void setupWiFi() {
  if (enableWiFi == 1) {
    //Blynk.begin(auth,ssid,pass);
    if (blinker_id == "") {
      if (!AutoWifiConfig())
      {
        SmartConfig();
      }
    } else {
      // 初始化blinker
      Blinker.begin(blinker_id, ssid, pass);

      Blinker.attachData(dataRead);
      Blinker.attachHeartbeat(heartbeat);
      Blinker.attachDataStorage(dataStorage);
    }
    WIFI = 1;
  }
}

bool AutoWifiConfig()
{

  //设置固定IP地址
  //IPAddress staticIP(192, 168, 5, 24); //ESP static ip
  //IPAddress gateway(192, 168, 5, 1);   //IP Address of your WiFi Router (Gateway)
  //IPAddress subnet(255, 255, 255, 0);  //Subnet mask
  //IPAddress dns(192, 168, 5, 1);  //DNS
  //WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.begin(ssid, pass); //Wifi接入到网络
  //WiFi.begin(); //Wifi接入到网络
  //如果觉得时间太长可改
  for (int i = 0; i < 20; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("WIFI AutoConfig Success");
      Serial.print("LocalIP:");
      Serial.print(WiFi.localIP());
      Serial.print(" ,GateIP:");
      Serial.println(WiFi.gatewayIP());
      return true;
    }
    else
    {
      Serial.print("WIFI AutoConfig Waiting......");
      Serial.println(wstatus);
      delay(1000);
    }
  }
  Serial.println("WIFI AutoConfig Faild!" );
  return false;
}

void SmartConfig()
{
  int delay_times = 0;
  // WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_STA);
  Serial.println("\r\n wait for smartconfig....");
  WiFi.beginSmartConfig();
  while (1)
  {
    if (delay_times > 60) {
      ESP.restart();
    }
    Serial.print(".");
    delay(500);
    if ( WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      //Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      //Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      break;
    }
    delay_times++;
  }
}


void Wireless_Telemetry() {

  ////////// WIFI TELEMETRY //////////
  if (enableWiFi == 1) {
    int LED1, LED2, LED3, LED4;                      //Declare LED brightness variable
    if (buckEnable == 1)      {
      LED1 = 200; //BATTERY CHARGING STATUS
    } else {
      LED1 = 0;
    }
    if (batteryPercent >= 99 ) {
      LED2 = 200; //FULL CHARGE STATUS
    } else {
      LED2 = 0;
    }
    if (batteryPercent <= 10) {
      LED3 = 200; //LOW BATTERY STATUS
    } else {
      LED3 = 0;
    }
    if (IUV == 0)             {
      LED4 = 200; //PV INPUT PRESENCE STATUS
    } else {
      LED4 = 0;
    }

    if (blinker_id == "") {
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        AutoWifiConfig();
        Serial.print(".");
      }
    } else {
      if (WiFi.status() == WL_CONNECTED) {
        Blinker.run();
      } else {
        AutoWifiConfig();
      }
    }

    /*
        Blynk.run();
        Blynk.virtualWrite(1,powerInput);
        Blynk.virtualWrite(2,batteryPercent);
        Blynk.virtualWrite(3,voltageInput);
        Blynk.virtualWrite(4,currentInput);
        Blynk.virtualWrite(5,voltageOutput);
        Blynk.virtualWrite(6,currentOutput);
        Blynk.virtualWrite(7,temperature);
        Blynk.virtualWrite(8,Wh/1000);
        Blynk.virtualWrite(9,energySavings);
        Blynk.virtualWrite(10,LED1);               //LED - Battery Charging Status
        Blynk.virtualWrite(11,LED2);               //LED - Full Battery Charge Status
        Blynk.virtualWrite(12,LED3);               //LED - Low Battery Charge Status
        Blynk.virtualWrite(13,LED4);               //LED - PV Harvesting

        Blynk.virtualWrite(14,voltageBatteryMin);  //Minimum Battery Voltage (Read & Write)
        Blynk.virtualWrite(15,voltageBatteryMax);  //Maximum Battery Voltage (Read & Write)
        Blynk.virtualWrite(16,currentCharging);    //Charging Current  (Read & Write)
        Blynk.virtualWrite(17,electricalPrice);    //Electrical Price  (Write)
    */
  }
  ////////// WIFI TELEMETRY //////////
  if (enableBluetooth == 1) {
    //ADD BLUETOOTH CODE
  }

}
