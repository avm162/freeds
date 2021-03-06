/*
  display.ino - Display Support
  Derivador de excedentes para ESP32 DEV Kit // Wifi Kit 32

  Based in opends+ (https://github.com/iqas/derivador)
  
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

void data_display(void)
{
  DEBUGLN("\r\nDATA_DISPLAY()");
#ifdef OLED
  if ((millis() - timers.FlashDisplay) > 1000)
  {
    timers.FlashDisplay = millis();
    Flags.flash = !Flags.flash;
  }

  if (config.wifi)
  {
    switch (screen)
    {
      case 0:
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        switch (config.wversion)
        {
          case 4:
          case 5:
          case 6:
            display.drawString(0, 0, (Flags.flash ? "Voltage" : "Current"));
            break;
          default:
            display.drawString(0, 0, (_SOLAR_));
            break;
        }

        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 0, (_GRID_));

        display.setTextAlignment(TEXT_ALIGN_CENTER);
        if (Flags.flash)
        {
          display.drawString(64, 0, (String(Error.ConexionInversor ? "S " : "S ") + String(Error.ConexionWifi ? "W " : "W ") + String(Error.ConexionMqtt ? "M " : "M  ")));
          display.drawString(85, 0, (String(config.wversion)));
        }
        else
        {
          display.drawString(64, 0, (String(Error.ConexionInversor ? "_ " : "S ") + String(Error.ConexionWifi ? "_ " : "W ") + String(Error.ConexionMqtt ? "_ " : "M  ")));
          display.drawString(85, 0, "v");
        }

        if (Flags.Updating)
          display.drawString(64, 38, _UPDATING_);
        else if ((!config.P01_on || (!config.pwm_man && (Error.LecturaDatos || Error.ConexionInversor))) && invert_pwm <= 1)
          display.drawString(64, 38, WiFi.localIP().toString());
        else
          display.drawProgressBar(0, 38, 120, 10, progressbar); // draw the progress bar

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        if (config.pwm_man) { display.drawString(0, 52, "PWM:MAN"); }
        else { display.drawString(0, 52, (config.P01_on ? "PWM:" + pro + "%" : "PWM:OFF")); }

        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 52, (_RELAY_ + String((digitalRead(PIN_RL1) ? "1 " : "_ ")) + String((digitalRead(PIN_RL2) ? "2 " : "_ ")) + String((digitalRead(PIN_RL3) ? "3 " : "_ ")) + String((digitalRead(PIN_RL4) ? "4 " : "_ "))));

        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        switch (config.wversion)
        {
          case 4:
          case 5:
          case 6:
            display.setFont(ArialMT_Plain_16);
            display.drawString(0, 14, (Flags.flash ? String(meter.voltage) : String(meter.current)));
            break;
          default:
            display.drawString(0, 12, (String)(int)inverter.wsolar);
            break;
        }

        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 12, (String)(int)inverter.wgrid);
        display.display();
        break;

      case 1:
          if (config.wversion < 4 || config.wversion > 6) {
            display.clear();
            display.setFont(ArialMT_Plain_10);
            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.drawString(64, 0,  _INVERTERINFO_);
            display.drawString(19, 12, _OLEDPOWER_);
            display.drawString(60, 12, _GRID_);
            display.drawString(102, 12, _OLEDTODAY_);
            display.drawString(19, 22, (String(int(inverter.wsolar)) + "W"));
            display.drawString(60, 22, (String(int(inverter.wgrid)) + "W"));
            display.drawString(102, 22, String(inverter.wtoday) + "Kw");
            display.drawString(30, 34, "STRING 1");
            display.drawString(30, 44, (String(int(inverter.pw1)) + "W"));
            display.drawString(30,  54, (String(int(inverter.pv1v)) + "V " + String(inverter.pv1c) + "A"));
            display.drawString(100, 34, "STRING 2");
            display.drawString(100, 44, (String(int(inverter.pw2)) + "W"));
            display.drawString(100,  54, (String(int(inverter.pv2v)) + "V " + String(inverter.pv2c) + "A"));
            display.display();
          } else { screen++; }
          break;
      
      case 2:
          if (config.wversion >= 4 && config.wversion <= 6) {
            display.clear();
            display.setFont(ArialMT_Plain_10);
            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.drawString(64, 0,  _METERINFO_);
            display.drawString(19, 12, _OLEDPOWER_);
            display.drawString(60, 12, _VOLTAGE_);
            display.drawString(102, 12, _CURRENT_);
            display.drawString(19, 22, (String(int(meter.activePower)) + "W"));
            display.drawString(60, 22, (String(int(meter.voltage)) + "V"));
            display.drawString(102, 22, String(meter.current) + "A");
            display.drawString(30, 34, _IMPORT_);
            display.drawString(30, 44, (String(meter.importActive) + "KWH"));
            display.drawString(100, 34, _EXPORT_);
            display.drawString(100, 44, (String(meter.exportActive) + "KWH"));
            display.drawString(64,  54, ("Total: " + String(meter.energyTotal) + "KWH"));
            display.display();
          } else { screen++; }
          break;

      case 3:
          display.clear();
          display.setFont(ArialMT_Plain_10);
          display.setTextAlignment(TEXT_ALIGN_LEFT);    
          display.drawString(0, 0,  ("IP: " + WiFi.localIP().toString()));
          display.drawString(0, 12, ("SSID: " + WiFi.SSID()));
          display.drawString(0, 24, ("FREC. PWM: " + String((float)config.pwmFrequency / 1000) + "Khz"));
          display.drawString(0, 36, ("PWM: " + pro + "% (" + String(invert_pwm) + ")"));
          display.drawString(0, 48, print_Uptime_Oled());
          display.display();
          break;

      case 4:
          display.clear();
          display.setFont(ArialMT_Plain_24);
          display.setTextAlignment(TEXT_ALIGN_CENTER);    
          display.drawString(64, 0, "FreeDS");
          display.setFont(ArialMT_Plain_10);
          display.drawString(64, 25, "Derivador de excedentes");
          display.drawString(64, 40, "Fecha Compilación:");
          display.drawString(64, 50, ("(" + String(compile_date) + ")"));
          display.display();
          break;
    }
  }
#endif
}

void showLogo(String Texto, bool timeDelay)
{
  display.clear();
  display.flipScreenVertically();
  display.drawFastImage(0, 0, 128, 64, FreeDS);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  int8_t saltoLinea = Texto.indexOf("\n");
  if (saltoLinea == -1){
    display.drawString(87, 45, Texto);
  } else {
    display.drawString(87, 40, Texto.substring(0, saltoLinea));
    display.drawString(87, 50, Texto.substring(saltoLinea + 1, Texto.length()));
  }
  display.display();
  if (timeDelay) { delay(2000); } // Innecesario salvo para mostrar el mensaje ;-)
}

