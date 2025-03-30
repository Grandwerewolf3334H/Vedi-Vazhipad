# Vedi-Vazhipad

**Stage Management System with ESP8266**
This project is an ESP8266-based Stage Management System that syncs lights, mist, and capacitor explosions with music via a web interface.

**Features**
Web Piano Interface: Play notes using a speaker.
Relay-Controlled Effects: Lights, mist maker, and capacitor explosions.
WiFi AP Mode: No external WiFi needed.

**Components**
ESP8266 (NodeMCU) – Main controller
6-Channel Relay Module – Controls stage effects
Speaker (Buzzer) – Generates tones
Transformer & Capacitors – Used for explosion effect

**Setup**
Flash ESP8266 using Arduino IDE & required libraries:
->ESP8266WiFi
->ESP8266WebServer
Connect Components as per circuit diagram.
Access Web Interface:
Connect to "Piano_AP" WiFi.
Open ESP8266's IP in a browser.
