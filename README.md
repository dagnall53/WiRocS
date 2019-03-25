# WiRocS

V17b 
--NOTE for self.(  Servo numbers are hard fixed and have range of only 1-8 so on ESP32, SERVO option will not work on I/O 9-16.
  any Possible clashes with the way the LOCO PWM/SERVO settings has not been tested.(Possible issues if SERVO and PWM both set? - NEEDS to be checked out, and the 6616 driver option needs to be written properly and tested))---- 
Updated:   
Revised addressing of PI02 and Pi03 parameters - Using Set bit etc. 
Faster BigClock - (Removed duplicate of OLED.Display which saved ~30ms per OLED display time. Even with two audio files running, the main loop executes at least 100 times per second, meaning that events should be reliably detected (Button test has ~10ms debounce test anyway). Events faster than this will be ignored 
Revised Debug Message MQTT send should now not be "retained", so should not be repeatedly resent by Mosquitto (not very important, but should make debug messages easier to follow if they are not repeated automatically) 

V16(b) moved the OledDisplayfonts into the arduino sketch. - Removes the need to modify the version in teh SSD1306 library.
- I may separate out the various fint .h files later to make them easier to edit, but for now I use Notepad++ and Glypheditor

From V12 I am not going to keep changing the Ino to WiRocs.ino, but keep it as WiRocS-vxx.ino, as this is how I keep it on my home system. 
You will need to rename the most recent .ino to "WiRocS.ino" after you have downloaded the sketch. (sorry, but this simplifies checking that I have uploaded the latest code.)
I am trying to save binaries compiled for the NodeMCU (ESP8266). These should be uploadable via an ESP Flash Programmer and set up a  stationary nodes without having to compile the whole code in Arduino. You will need to use FileZilla or Arduino_Sketch_Data_Uploader to upload the sounds from the Data directory to the Node's SPIFFS, but generally this only needs doing once (unless you want to make changes to the sound effects! - OR if you change the SPIFFS file size during Compile! -- V16 on for ESP8266 needs SPIFFS =2M, to give room for OTA!! -- ESP32 is limited to 1M SPIFFS by current Arduino compile limits --)  

WiFi Rocnet node for ESP 32 and 8266
For documentation please read https://wiki.rocrail.net/doku.php?id=users:dagnall53:description
Use of this software is at your own risk!

V15 Internally the Code now references each of the four possible OLEDS as OLED1-4. This saves some confusion with the RocDisplay "display" numbers. 
I2C bus: the defaults are now:  
  ESP32: OLED_SDA = 4; OLED_SCL = 5; OLED_SDA2 = 19; OLED_SCL2 = 21;
  ESP8266 OLED_SDA2 = 5;("D1")OLED_SCL2 = 4("D2); OLED_SDA = 0; ("D3") OLED_SCL = 2; ("D4") 

If one or more OLEDs are found during startup, they should display messages during startup and a clock once running. 

Sound comes from ESP8266/nodemcu Pin D9 (rx) (or ESP32 pin 22) Connect this to base of an NPN via 10k, and collector to speaker, other side of speaker to 5V. Then try switching an output set to Interface MQTT, Bus (the RocNet address of the node), Address 101-109 you should get sound effects. Make sure you have uploaded the "data" to the spiffs. And programmed using Spiffs(4M(3MSpiffs).   



V4b Changed Signal Led (NodeMcu D4) so it only works if Display is NOT connected.
     
V4 tried to correct error whereby a clean ESP's EEPROM was not properly set, so was being continually set to defaults.

