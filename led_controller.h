#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H
#endif

class LEDController {
  private:
    int pin;

  public:
    LEDController(int ledPin) : pin(ledPin) {
      pinMode(pin, OUTPUT);
    }

    void turnOn() {
      digitalWrite(pin, HIGH);
    }

    void turnOff() {
      digitalWrite(pin, LOW);
    }
};
