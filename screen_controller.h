#ifndef SCREEN_CONTROLLER_H
#define SCREEN_CONTROLLER_H
#endif

#include "led_controller.h"
class DisplayController {
private:
  TM1637Display display;
  LEDController &ledController;
  bool shouldStop;
  unsigned long startTime;       // Tiempo de inicio del contador
  unsigned long lastDisplayUpdate; // Última actualización del display
  unsigned long lastBlinkTime;
  int currentBlinkInterval;
  bool ledBlinkState;
  unsigned long totalCentiseconds; // Duración total en centésimas
  bool isInsertMode;
  int currentInsertNumber;

public:
  DisplayController(int clkPin, int dioPin, LEDController &ledCtrl)
    : display(clkPin, dioPin),
      ledController(ledCtrl),
      shouldStop(false),
      startTime(0),
      lastDisplayUpdate(0),
      lastBlinkTime(0),
      currentBlinkInterval(500),
      ledBlinkState(false),
      totalCentiseconds(0),
      isInsertMode(false),
      currentInsertNumber(0) {}

  void enterInsertMode() {
    isInsertMode = true;
    currentInsertNumber = 2000;
    animateNumber(currentInsertNumber);
  }

  void exitInsertMode() {
    isInsertMode = false;
  }

  bool isInInsertMode() {
    return isInsertMode;
  }

  int getScreenNumber() {
    return currentInsertNumber;
  }

  void adjustInsertNumber(int delta) {
    currentInsertNumber += delta;
    currentInsertNumber = constrain(currentInsertNumber, 0, 9999);
    display.showNumberDec(currentInsertNumber, false);
  }

  int getCurrentInsertNumber() {
    return currentInsertNumber;
  }


  void init() {
    display.setBrightness(0x0f);
    animateNumber(9999);
    display.showNumberDecEx(0, 0b01000000, true);
  }
  void updateCountDown() {
    if (shouldStop) return;

    unsigned long currentMillis = millis();
    unsigned long elapsedCentis = (currentMillis - startTime) / 10;
    int remaining = totalCentiseconds - elapsedCentis;

    if (remaining <= 0) {
      shouldStop = true;
      display.showNumberDecEx(0, 0b01000000, true);
      ledController.turnOff();
      return;
    }

    if (currentMillis - lastDisplayUpdate >= 100) {
      lastDisplayUpdate = currentMillis;
      display.showNumberDecEx(remaining, 0b01000000, true);
    }

    currentBlinkInterval = (remaining <= 500 && remaining > 0) ? 250 : 500;

    if (currentMillis - lastBlinkTime >= currentBlinkInterval) {
      ledBlinkState = !ledBlinkState;
      ledBlinkState ? ledController.turnOn() : ledController.turnOff();
      lastBlinkTime = currentMillis;
    }
  }
  void startCountDown(int timeInSecs) {
    totalCentiseconds = timeInSecs * 100;
    startTime = millis();
    lastDisplayUpdate = 0;
    shouldStop = false;
    display.showNumberDecEx(totalCentiseconds, 0b01000000, true);
  }

  void stopCountDown() {
    shouldStop = true;
  }

  void resetCountDown() {
    shouldStop = false;
  }

  void animateNumber(int targetNumber) {
    int currentNumber = 0;
    int stepDelay = 50;

    int thousands = targetNumber / 1000;
    int hundreds = (targetNumber / 100) % 10;
    int tens = (targetNumber / 10) % 10;
    int ones = targetNumber % 10;

    for (int i = 0; i <= 9; i++) {
      display.showNumberDec(currentNumber * 1000 + currentNumber * 100 + currentNumber * 10 + currentNumber, false);
      delay(stepDelay);
      currentNumber++;
    }

    display.showNumberDec(targetNumber, false);
  }
};
