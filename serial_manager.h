#include "RadioManager.h"

class CommunicationManager {
private:
  DisplayController &displayController;
  LEDController &ledController;
  RadioManager &radio;
  String deviceName;

public:
  CommunicationManager(DisplayController &dispCtrl, LEDController &ledCtrl, RadioManager &radioManager, const String &devName)
    : displayController(dispCtrl), ledController(ledCtrl), radio(radioManager), deviceName(devName) {}

  void processRadioInput() {
    char buffer[32] = "";
    if (radio.receiveMessage(buffer, sizeof(buffer))) {
      String input = String(buffer);
      input.trim();
      Serial.println("Mensaje recibido por radio: " + input);

      String targetDevice = getValue(input, ' ', 0);
      String command = getValue(input, ' ', 1);
      String numberStr = getValue(input, ' ', 2);
      Serial.println(deviceName);
      if (targetDevice == deviceName || targetDevice == "ALL") {
        radio.clearMessage();
        Serial.println("Mensaje dirigido a este dispositivo: " + command);

        displayController.stopCountDown();

        if (command != "INSERT_NUMBER") {
          displayController.exitInsertMode();
        }

        if (command == "SHOW_NUMBER") {
          int targetNumber = numberStr.toInt();
          if (targetNumber > 0) {
            displayController.animateNumber(targetNumber);
            Serial.print("Número recibido: ");
            Serial.println(targetNumber);
          }
        } else if (command == "LED_ON") {
          ledController.turnOn();
          Serial.println("LED Encendido");
        } else if (command == "LED_OFF") {
          ledController.turnOff();
          Serial.println("LED Apagado");
        } else if (command == "countdown") {
          displayController.resetCountDown();
          displayController.startCountDown(numberStr.toInt());
        } else if (command == "INSERT_NUMBER") {
          displayController.enterInsertMode();
          Serial.println("Modo inserción activado. Usa los botones.");
        }
      } else {
        Serial.println("Mensaje ignorado. No es para este dispositivo.");
      }
    }
  }

  String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
      }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
  }
};