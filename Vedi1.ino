#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Piano_AP";  // WiFi SSID
const char* password = "";      // No password

ESP8266WebServer server(80);

#define SPEAKER_PIN D2  // Speaker output pin

// Relay pins (Adjust these according to your relay module)
const int RELAY_PINS[] = {D3, D4, D5, D6, D7, D8};  
const int NUM_RELAYS = 6;

unsigned long relayTimers[NUM_RELAYS];  // Store next toggle times
unsigned long relayIntervals[NUM_RELAYS];  // Store random intervals

// Frequency mapping for more musical notes (in Hz)
const int notes[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};  
const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C5"};

int currentFrequency = 300; // Default frequency for note play

void playTone(int frequency, int duration) {
    tone(SPEAKER_PIN, frequency, duration);
    delay(50); // Small delay to separate notes
}

void handleNotePress() {
    if (server.hasArg("note")) {
        int noteIndex = server.arg("note").toInt();
        if (noteIndex >= 0 && noteIndex < 13) {  // Updated for 13 keys
            Serial.print("Playing Note: ");  // Print to Serial Monitor
            Serial.println(noteNames[noteIndex]);  
            playTone(notes[noteIndex] + currentFrequency, 300);  // Play note for 300ms with adjusted frequency
        }
    }
    server.send(200, "text/plain", "OK");
}

void handleFrequencyChange() {
    if (server.hasArg("frequency")) {
        currentFrequency = server.arg("frequency").toInt();
        Serial.print("Adjusting Frequency by: ");
        Serial.println(currentFrequency);
    }
    server.send(200, "text/plain", "OK");
}

// Function to control relays asynchronously
void handleRelays() {
    unsigned long currentMillis = millis();
    for (int i = 0; i < NUM_RELAYS; i++) {
        if (currentMillis >= relayTimers[i]) {
            // Toggle relay state
            digitalWrite(RELAY_PINS[i], !digitalRead(RELAY_PINS[i]));
            
            // Set next random interval (between 5s and 15s)
            relayIntervals[i] = random(5000, 15000);
            relayTimers[i] = currentMillis + relayIntervals[i];

            Serial.print("Relay ");
            Serial.print(i + 1);
            Serial.print(" toggled. Next change in ");
            Serial.print(relayIntervals[i] / 1000);
            Serial.println(" seconds.");
        }
    }
}

void setup() {
    pinMode(SPEAKER_PIN, OUTPUT);

    // Initialize relay pins as OUTPUT and turn them OFF initially
    for (int i = 0; i < NUM_RELAYS; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
        relayTimers[i] = millis() + random(5000, 15000);  // Set initial random time
    }

    WiFi.softAP(ssid, password);
    Serial.begin(115200);
    Serial.println("WiFi AP Started. Connect to 'Piano_AP'");

    server.on("/", []() {
        server.send(200, "text/html", R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
                <title>ESP8266 Piano</title>
                <style>
                    body { text-align: center; font-family: Arial, sans-serif; background: #111; color: white; margin: 0; }
                    h1 { margin: 10px 0; }
                    .keys { display: flex; justify-content: center; width: 100vw; overflow-x: auto; }
                    .key {
                        flex: 1; height: 200px; font-size: 18px; font-weight: bold;
                        background: white; color: black; border: 2px solid black;
                        display: flex; align-items: flex-end; justify-content: center;
                        cursor: pointer; transition: transform 0.1s, background 0.1s;
                        min-width: 60px;
                    }
                    .key:active { background: yellow; transform: scale(0.9); }
                    .black-key {
                        width: 40px; height: 120px; background: black; color: white;
                        position: relative; z-index: 2; border-radius: 5px;
                        margin-left: -20px; margin-right: -20px;
                    }
                    .slider-container {
                        margin-top: 20px;
                        text-align: center;
                    }
                    .slider {
                        width: 80%; 
                        height: 20px;
                    }
                </style>
                <script>
                    function playNote(note) {
                        fetch('/play?note=' + note, { method: 'GET' });
                    }

                    function adjustFrequency() {
                        var frequency = document.getElementById("frequencySlider").value;
                        fetch('/frequency?frequency=' + frequency, { method: 'GET' });
                    }
                </script>
            </head>
            <body>
                <h1>ESP8266 Web Piano</h1>
                <div class="keys">
                    <div class="key" onclick="playNote(0)">C</div>
                    <div class="black-key key" onclick="playNote(1)">C#</div>
                    <div class="key" onclick="playNote(2)">D</div>
                    <div class="black-key key" onclick="playNote(3)">D#</div>
                    <div class="key" onclick="playNote(4)">E</div>
                    <div class="key" onclick="playNote(5)">F</div>
                    <div class="black-key key" onclick="playNote(6)">F#</div>
                    <div class="key" onclick="playNote(7)">G</div>
                    <div class="black-key key" onclick="playNote(8)">G#</div>
                    <div class="key" onclick="playNote(9)">A</div>
                    <div class="black-key key" onclick="playNote(10)">A#</div>
                    <div class="key" onclick="playNote(11)">B</div>
                    <div class="key" onclick="playNote(12)">C</div>
                </div>
                <div class="slider-container">
                    <h3>Adjust Frequency</h3>
                    <input type="range" min="-50" max="50" value="0" class="slider" id="frequencySlider" onchange="adjustFrequency()">
                    <p>Frequency Adjustment: <span id="frequencyValue">0</span></p>
                </div>
            </body>
            </html>
        )rawliteral");
    });

    server.on("/play", HTTP_GET, handleNotePress);
    server.on("/frequency", HTTP_GET, handleFrequencyChange);

    server.begin();
    Serial.println("Server Started");
}

void loop() {
    server.handleClient();
    handleRelays();  // Check and update relay states
}