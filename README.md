# WiRocS
From V12 I am not going to keep changing the Ino to WiRocs.ino, but keep it as WiRocS-vxx.ino, as this is how I keep it on home my system. 
You will need to rename the most recent .ino to "WiRocS.ino" after you have downloaded the sketch. (sorry)

WiFi Rocnet node for esp 32 and 8266
For documentation please read https://wiki.rocrail.net/doku.php?id=users:dagnall53:description
Use of this software is at your own risk!
V15 Internally the 

V8(a) Revised interface to OLEDS. I had some issues with my ESP32 board so could not use the same I2C pins for both ESP8266 and ESP32. The code now expects 
ESP8266: (OLED SCL to connect to ESP8266/NodeMCU D4 , OLED SDA to ESP8266/NodeMCU D3)
ESP32: (OLED SCL = gpio5 OLED SDA = gpio 4) 

If an OLED is found during startup, it should display messages and a clock once running. 
Sound comes from ESP8266/nodemcu Pin D9 (rx) (or ESP32 pin 22) Connect this to base of an NPN via 10k, and collector to speaker, other side of speaker to 5V. Then try switching an output set to Interface MQTT, Bus (the RocNet address of the node), Address 101-109 you should get sound effects. Make sure you have uploaded the "data" to the spiffs. And programmed using Spiffs(4M(3MSpiffs).   



V4b Allows Oled_SCL to be set to 0 to free up D2. Changed Signal Led (NodeMcu D4) so it only works if Display is NOT connected.
     
V4 tried to correct error whereby a clean ESP's EEPROM was not properly set, so was being continually set to defaults.

