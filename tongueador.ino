#include <TM1637Display.h>
#include "screen_controller.h"
#include "serial_manager.h"
#include "RadioManager.h"

#define CLK A1
#define DIO A0
#define LED_PIN 8
#define BUTTON_RED 2
#define BUTTON_BLUE 3
#define BUTTON_YELLOW 4
#define BUTTON_GREEN 5
#define BUTTON_WHITE 6
#define DEVICE_NAME "TNG02"
#define CE_PIN 9
#define CSN_PIN 10

const byte address[6] = "00001";
RadioManager radio(CE_PIN, CSN_PIN, address);
LEDController ledController(LED_PIN);
DisplayController displayController(CLK, DIO, ledController);
CommunicationManager radioCommandManager(displayController, ledController, radio, DEVICE_NAME);

unsigned long lastDebounceTime[5] = { 0 };
const int debounceDelay = 100;
int lastButtonState[5] = { HIGH, HIGH, HIGH, HIGH, HIGH };
int stableButtonState[5] = { HIGH, HIGH, HIGH, HIGH, HIGH };
const int buttons[] = { BUTTON_RED, BUTTON_BLUE, BUTTON_YELLOW, BUTTON_GREEN, BUTTON_WHITE };

void setup() {
  Serial.begin(9600);
  displayController.init();
  
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_WHITE, INPUT_PULLUP);
  radio.begin();
}

void checkButton(int buttonPin, int buttonIndex, int delta) {
  int currentState = digitalRead(buttonPin);

  if (currentState != lastButtonState[buttonIndex]) {
    lastDebounceTime[buttonIndex] = millis();
    lastButtonState[buttonIndex] = currentState;
  }

  if ((millis() - lastDebounceTime[buttonIndex]) > debounceDelay) {
    if (currentState != stableButtonState[buttonIndex]) {
      stableButtonState[buttonIndex] = currentState;

      if (stableButtonState[buttonIndex] == LOW) {
        if (displayController.isInInsertMode()) {
          if (buttonIndex == 4) {
            String confirmationMessage = "C1 " + String(DEVICE_NAME) + " " + String(displayController.getScreenNumber());
            displayController.animateNumber(displayController.getScreenNumber());
            bool success = radio.sendMessage(confirmationMessage.c_str());
            while(!success){
              success = radio.sendMessage(confirmationMessage.c_str());
            }
            if(success){
              displayController.exitInsertMode();
            }
          } else {
            displayController.adjustInsertNumber(delta);
            Serial.println(displayController.getCurrentInsertNumber());
          }
        } else {
          sendButtonSignal(buttonIndex);
        }
      }
    }
  }
}

void loop() {
  radioCommandManager.processRadioInput();
  displayController.updateCountDown();

  checkButton(BUTTON_RED, 0, -10);
  checkButton(BUTTON_BLUE, 1, +10);
  checkButton(BUTTON_YELLOW, 2, -1);
  checkButton(BUTTON_GREEN, 3, +1);
  checkButton(BUTTON_WHITE, 4, 0);
}

void sendButtonSignal(int button) {
  String message = "ButtonPressed " + String(DEVICE_NAME) + " " + String(button) + "\n";
  radio.sendMessage(message.c_str());
  Serial.println("ButtonPressed " + String(DEVICE_NAME) + " " + String(button));
}