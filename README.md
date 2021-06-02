# SolarMQTT_DS18B20_BH
Demo code for Bunker Hill Solar Light Hack

//ESP8266 Solar Powered Temp Sensor/MQTT //by Bill Poulsen iot-techshop.com //Join our FB Group at: https://www.facebook.com/groups/3188341227932417/

//Sends Data to MQTT Server over wifi //Auto-Adjust Intervals based on Battery Voltage to conserve power //Uses NTP Server to obtain time date stamps //Device ID is auto filled using ESP unique MAC address

//MQTT Data String Format: {"ID":"DC4F2211E835","time":"20200806_110320","devTemp":"81.72","vBat:","3.45","rInt:","60"} //DS18B20 Temp; myString = "{"ID":"" + devID + "","time":"" + myDateTime + "","" + "devTemp" + "":"" + myTemp + "","" + "vBat" + "":"" + vBat + "","" + "rInt" + "":"" + rInt +""}"; //DHT11bTemp/Humid; myString = "{"ID":"" + devID + "","tim":"" + myDateTime + "","" + "Tmp" + "":"" + myTemp + "","" + "Hum" + "":"" + humid + "","" + "soil" + "":"" + moist + "","" + "Bat" + "":"" + vBat + "","" + "Int" + "":"" + rInt + ""}";

//rInt is in seconds

//Hardware Config //D2 Temp Pin //To enable wakeup from sleep jumper RST to the D0(GPIO16) pin on your ESP8266. The RTC generates a reset pulse on that //pin which is used to restart the device. The simple concept is that you can use a F/W command to put the device into //into sleepmode, but need a hardware pulse to wake it up again. Wakeup acts like pressing your reset button.

You can get most components on Amazon.com 

MELIFE 2pcs Wemos D1 Mini V3.0 Development Board ESP8266 V3.0.0 4MB Flash Wireless WiFi Internet Development Board Based ESP8266 CH340 CH340G for Arduino Nodemcu V2 MicroPython https://amzn.to/30hZkLQ

DS18B20 Temo Sensors https://amzn.to/2MZ6rGg
