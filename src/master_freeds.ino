/*
  master_freeds.ino - Master Freeds Support
  Derivador de excedentes para ESP32 DEV Kit // Wifi Kit 32
  
  Copyright (C) 2020 Pablo Zerón (https://github.com/pablozg/freeds)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void getMasterFreeDsData(void)
{
  DEBUGLN("\r\nGETMASTERFREEDSDATA()");

  if (config.wifi)
  {
    if (config.wversion == 12)
    {
        MasterFreeDsCom();
    }
  }
}

void MasterFreeDsCom(void)
{
    HTTPClient clientHttp;
    WiFiClient clientWifi;
    clientHttp.setConnectTimeout(1500);
    httpcode = -1;
    
    String url = "http://" + (String)config.sensor_ip + "/masterdata";
    clientHttp.begin(clientWifi, url);
    httpcode = clientHttp.GET();

    DEBUGLN("HTTPCODE ERROR: " + (String)httpcode);

    if (httpcode == HTTP_CODE_OK)
    {
      String Resp = clientHttp.getString();
      parseMasterFreeDs(Resp);
      Error.ConexionInversor = false;
    }
    clientHttp.end();
    clientWifi.stop();
}

///////// Procesa los datos recibidos desde el ESP32 Maestro
void parseMasterFreeDs(String json)
{
  DEBUGLN("JSON:" + json);
  DeserializationError error = deserializeJson(root, json);
  
  if (error) {
    INFO("deserializeJson() failed: ");
    INFOLN(error.c_str());
    httpcode = -1;
  } else {
    DEBUGLN("deserializeJson() OK");
    masterMode = (int)root["wversion"];
    inverter.wgrid =  (float)root["wgrid"]; // Potencia de red
    
    if ((int)root["PwmMaster"] >= config.pwmSlaveOn) {
      config.P01_on = true;
    } else {
      config.P01_on = false;
    }
    
    switch (masterMode)
    {
      case 3: // Mqtt
        inverter.pv1c =   (float)root["pv1c"]; // Corriente string 1
        inverter.pv2c =   (float)root["pv2c"]; // Corriente string 2
        inverter.pv1v =   (float)root["pv1v"]; // Tension string 1
        inverter.pv2v =   (float)root["pv2v"]; // Tension string 2
        inverter.pw1 =    (float)root["pw1"];    // Potencia string 1
        inverter.pw2 =    (float)root["pw2"];    // Potencia string 2
        inverter.gridv =  (float)root["gridv"];   // Tension de red
        inverter.wtoday = (float)root["wtoday"];    // Potencia solar diaria
        inverter.wsolar = (float)root["wsolar"];    // Potencia solar actual
        break;
    case 4:
    case 5:
    case 6:
      meter.voltage =      (float)root["mvoltage"];
      meter.current =      (float)root["mcurrent"];
      meter.powerFactor =  (float)root["mpowerFactor"];
      meter.frequency =    (float)root["mfrequency"];
      meter.importActive = (float)root["mimportActive"];
      meter.exportActive = (float)root["mexportActive"];
      meter.energyTotal =  (float)root["menergyTotal"];
      meter.activePower =  (float)root["mactivePower"];
      meter.aparentPower = (float)root["maparentPower"];
      meter.reactivePower =  (float)root["mreactivePower"];
      meter.importReactive = (float)root["mimportReactive"];
      meter.exportReactive = (float)root["mexportReactive"];
      meter.phaseAngle =     (float)root["mphaseAngle"];
      break;
    case 9:
    case 10:
      meter.voltage =      (float)root["mvoltage"];
      meter.powerFactor =  (float)root["mpowerFactor"];
      meter.importActive = (float)root["mimportActive"];
      meter.exportActive = (float)root["mexportActive"];
      meter.activePower =  (float)root["mactivePower"];
      meter.reactivePower =  (float)root["mreactivePower"];
      inverter.wsolar =    (float)root["wsolar"];
      inverter.gridv =    (float)root["gridv"];
      break;
    default:
      inverter.wtoday = (float)root["wtoday"];
      inverter.wsolar = (float)root["wsolar"];
      inverter.gridv =  (float)root["gridv"];
      inverter.pv1c =   (float)root["pv1c"];
      inverter.pv1v =   (float)root["pv1v"];
      inverter.pw1 =    (float)root["pw1"];
      inverter.pv2c =   (float)root["pv2c"];
      inverter.pv2v =   (float)root["pv2v"];
      inverter.pw2 =    (float)root["pw2"];
      break;
    }
    Error.ConexionInversor = false;
    timers.ErrorConexionRed = millis();
  }
}