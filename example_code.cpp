
// Libraries setup

#include <MIDI.h>  // Library created by Francois Best

// MIDI Baud Rate

struct MIDISettings : public midi::DefaultSettings {
  static const long BaudRate =
      115200;  // 115200 (Serial MIDI USB), 31250 (MIDI Connector Application)
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MIDISettings);

// MIDI Channel, Note and CC initial setup

byte midiCh = 1;  // MIDI channel
byte note = 60;   // Initial note
byte cc = 1;  // Initial MIDI CC. The first pot will send cc1 (0-127) messages,
              // second pot cc2 (0-127) messages...

// Variables

const int Number_Pots = 2;  // Number of pots you have
const int Pot_Pin[Number_Pots] = {A0,
                                  A1};  // Pins where your pots are connected,
                                        // in the case of Arduino Nano: A0-A7
const int Number_Buttons = 2;           // Number of buttons you have
const int Button_Pin[Number_Buttons] = {
    2, 3};  // Pins where your buttons are connected, in the case of Arduino
            // Nano: 2-12

int potCurrentState[Number_Pots] = {0};
int oldPotState[Number_Pots] = {0};
int midiCurrentState[Number_Pots] = {0};
int buttonState[Number_Buttons] = {0};
int lastButtonState[Number_Buttons] = {0};

void setup() {
  Serial.begin(
      115200);  // 115200 (Serial MIDI USB), 31250 (MIDI Connector Application)
  for (int i = 0; i < Number_Buttons; i++) {
    pinMode(Button_Pin[i], INPUT_PULLUP);
  }
}

void loop() {
  potentiometers();
  buttons();
}

// POTENTIOMETERS
void potentiometers() {
  for (int i = 0; i < Number_Pots; i++) {
    potCurrentState[i] =
        map(analogRead(Pot_Pin[i]), 0, 1023, 0,
            127);  // Arduino reads all pots and convert the analog voltage
                   // (0-5V) into digital values (0-127)

    if (abs(potCurrentState[i] - oldPotState[i]) > 1) {
      // Send the current pot value
      MIDI.sendControlChange(cc + i, potCurrentState[i],
                             midiCh);  // cc number, cc value, midi channel
      oldPotState[i] = potCurrentState[i];
    }
  }
}

// BUTTONS
void buttons() {
  for (int i = 0; i < Number_Buttons; i++) {
    buttonState[i] = digitalRead(Button_Pin[i]);
    if (buttonState[i] != lastButtonState[i]) {
      if (buttonState[i] == HIGH) {
        // if the button is not pressed (pin state High) send the following
        // message
        MIDI.sendNoteOn(note + i, 0, midiCh);
      } else {
        // if the button is pressed (pin state LOW) send the following message
        MIDI.sendNoteOn(note + i, 127, midiCh);
      }

      delay(20);
    }
    lastButtonState[i] = buttonState[i];
  }
}
