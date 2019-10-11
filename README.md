# WiRocS

WiFi Rocnet node for ESP 32 and 8266
For documentation please read https://wiki.rocrail.net/doku.php?id=users:dagnall53:description
Use of this software is at your own risk!

 For syntax see. https://wiki.rocrail.net/doku.php?id=users:dagnall53:rdtextalign


=== General note ===


I am trying to save binaries compiled for the NodeMCU (ESP8266). These should be uploadable via an ESP Flash Programmer and set up a  stationary nodes without having to compile the whole code in Arduino. You will need to use FileZilla or Arduino_Sketch_Data_Uploader to upload the sounds from the Data directory to the Node's SPIFFS, but generally this only needs doing once (unless you want to make changes to the sound effects!) 

===LIBRARIES NOTE:===
See https://github.com/earlephilhower/ESP8266Audio#installation 
Make sure you have the right settings and the most recent library.

  https://github.com/earlephilhower/ESP8266Audio
  https://github.com/Gianbacchio/ESP8266_Spiram
  https://github.com/knolleary/pubsubclient
  https://github.com/ThingPulse/esp8266-oled-ssd1306
  https://github.com/nailbuster/esp8266FTPServer (includes esp32 support)

For ESP 32 (still needed for esp8266 compile)
  https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/latest
  https://github.com/madhephaestus/ESP32Servo/tree/master/src


===Version History===

V31

Some improvements in the WiFi "Antenna" signal level display on OLED. From -90db the "antenna V wings" first dissapear and the antenna upright starts to shorten. This signal level (less than -90db) is about where the ESP8266 can still work but is likely to lose WiFi. If the "antenna" display starts to go the signal is VERY LOW. This display is useful for tracking the edges of the "suitable WiFi" signal zone.   


V30

Changed "search for MQTT", so that it now only increments the MQTT address IF the WIFI signal is reasonably big. (Better than -85 dB reported). If the signal is worse, it will not increment the address, but will keep trying to find the broker at the previously set address. This prevents WiFi dropouts from causing the node to start a long hunt for the MQTT. 

Modified sound volume control. If NOT a loco, then sounds are played at 90% volume. 
Previously, sound volumes were set using CV(100) - on . This was fine if the node is a loco, (or has been used as a loco in the past), but setting CV values is not as easy on a standard node, so I have simply set the volume at 90%. (If node is used as a Loco, then the CV's remain active). 

V29

Fixed issue that truncated scrolling on some messages 

V28

Added small delay in some OLED display status task (Should allow the processor to do other tasks, like Audio more frequently.)


V27

Fixed scrolling issue that affected scrolling in messages 1 and 5

V26

Revised Serial input so it should be improved.
Re-Organised MQTT debug messages so they do not try to send before the MQTT is set up!.
Made the _32 mode "global", so changing it for any OLED should change all to _32 mode.
Corrected which RN messages can be sent to the _32 version OLED with Addr 60 ("OLED6") on second I2C bus. This was RN 3 and 4, but now corrected to RocMessges 5 and 6 to be consistent with the option of this OLED alternatively being a "_64" OLED ("OLED2"), which displays RN messages 5,6,7,8.  
  

V25

Corrected fault that changed OLED settings.
OLED_Settings (Flipped,inverted _32 etc) are now automatically saved to EEPROM if changed. 


V24 

Improved text parsing to help formatting of messages in Rocview.: 
Code now ignores the first space before a { and the first space after a }. 

V23

Added {wnnn} command to modify First tab {T0} (Platform number) position
{Wnn}modifies Second Tab {T1} (Train Time) position
Added Screen orientation and inversion commands: {SF} flips display {SI} inverts display (Black on white) {SN} reverts     to standard   orientation and colour, {Sf} Black on white and flipped.
These commands need only be sent once and the orientation etc will be saved until the display is reset.

V20 

Added scrolling for text. text placed between {JS} and {JC} will scroll.


V18 

Changed Default second I2C ports on ESP32  to 33 ("D13") and 34("D14") 
Modified ESP32 PWM code. 
Added switchable ability to send Setup information out on Debug MQTT. But have left this switched off as it severely slows down Setup sequence.
Added serial println notice if servos above addr 8 are requested.  
All PWM channels now start at "left" value, and not "0" as previously.


V17 

Added more checks in Audio setup. Code now disables audio if it cannot find the F6 and F3 wav files that must be played as part of setup.Added 6616 PWM driver and got the PWM Loco option working again. Revised addressing of PI02 and Pi03 parameters  Faster BigClock on OLED displays. Changed Button debounce test to >=10ms.

*Remaining issue at V17: Servo numbers are hard fixed and have range of only 1-8 so on ESP32, the Pi03 SERVO option will not work on I/O 9-16.

V16(b) 

moved the OledDisplayfonts into the arduino sketch. - Removes the need to modify the version in the SSD1306 library. (I may separate out the various font ".h" files later to make them easier to edit, but for now I use Notepad++ and Glypheditor)

V15 Internally the Code now references each of the four possible OLEDS as OLED1-4. This saves some confusion with the RocDisplay "display" numbers. 
I2C bus: the defaults are now:  
  ESP32: OLED_SDA = 4; OLED_SCL = 5; OLED_SDA2 = 19; OLED_SCL2 = 21;
  ESP8266 OLED_SDA2 = 5;("D1")OLED_SCL2 = 4("D2); OLED_SDA = 0; ("D3") OLED_SCL = 2; ("D4") 

If one or more OLEDs are found during startup, they should display messages during startup and a clock once running. 

Sound comes from ESP8266/nodemcu Pin D9 (rx) (or ESP32 pin 22) Connect this to base of an NPN via 10k, and collector to speaker, other side of speaker to 5V. Then try switching an output set to Interface MQTT, Bus (the RocNet address of the node), Address 101-109 you should get sound effects. Make sure you have uploaded the "data" to the spiffs. And programmed using Spiffs(4M(3MSpiffs).   



V4b Changed Signal Led (NodeMcu D4) so it only works if Display is NOT connected.
     
V4 tried to correct error whereby a clean ESP's EEPROM was not properly set, so was being continually set to defaults.

