# WiRocS
WiFi Rocnet node for esp 32 and 8266
For documentation please read https://wiki.rocrail.net/doku.php?id=users:dagnall53:description
Use of this software is at your own risk!

V7(a) Revised interface to Oleds. Now Expects OLED SCL to connect to ESP8266/NodeMCU D4 OledSDA to ESP8266/NodeMCU D3
(OledSCL = gpio2 oled SDA =gpio0) If an Oled is found during startup, it should display messages and a clock once running. 
Sound comes from ESP8266/nodemcu Pin D9 (rx) Connect this to base of an NPN via 10k, and collector to speaker, other side of speaker to 5V. Then try switching an output set to Interface MQTT, Bus (the RocNet address of the node), Address 101-109 you should get sound effects. Make sure you have uploaded the "data" to the spiffs. And programmed using Spiffs(4M(3MSpiffs).   



V4b Allows Oled_SCL to be set to 0 to free up D2. Changed Signal Led (NodeMcu D4) so it only works if Display is NOT connected.
     
V4 tried to correct error whereby a clean ESP's EEPROM was not properly set, so was being continually set to defaults.

