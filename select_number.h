#include <TM1637Display.h>

TM1637Display display(CLK, DIO);
int valor = 2000;

unsigned long ultimaAccion = 0;
const unsigned long debounceTime = 50;
const unsigned long holdThreshold = 500;
const unsigned long repeatInterval = 100;

struct ButtonState {
  bool pressed;
  bool initialPress;
  unsigned long pressTime;
};

ButtonState buttons[] = {
  {false, false, 0},
  {false, false, 0},
  {false, false, 0},
  {false, false, 0}
};

void loop() {
  unsigned long currentMillis = millis();
  
  checkButton(BUTTON_1, 0, +10);
  checkButton(BUTTON_2, 1, -10);
  checkButton(BUTTON_3, 2, +1);
  checkButton(BUTTON_4, 3, -1);
}

void checkButton(int pin, int index, int increment) {
  bool estado = digitalRead(pin) == LOW;
  unsigned long currentMillis = millis();

  if (estado) {
    if (!buttons[index].pressed) {
      buttons[index].pressed = true;
      buttons[index].initialPress = true;
      buttons[index].pressTime = currentMillis;
      actualizarValor(increment, pin);
    }
    else {
      if (buttons[index].initialPress) {
        if (currentMillis - buttons[index].pressTime > holdThreshold) {
          buttons[index].initialPress = false;
          actualizarValor(increment, pin);
        }
      } else {
        if (currentMillis - ultimaAccion > repeatInterval) {
          actualizarValor(increment, pin);
        }
      }
    }
    ultimaAccion = currentMillis;
  } else {
    buttons[index].pressed = false;
    buttons[index].initialPress = false;
  }
}

void actualizarValor(int cambio, int pin) {
  valor += cambio;
  valor = constrain(valor, -999, 9999);
  
  display.showNumberDec(valor);
  
  String boton = "";
  if (pin == BUTTON_1) boton = "+10";
  else if (pin == BUTTON_2) boton = "-10";
  else if (pin == BUTTON_3) boton = "+1";
  else if (pin == BUTTON_4) boton = "-1";
  
  Serial.print("Botón ");
  Serial.print(boton);
  Serial.print(" | Valor: ");
  Serial.println(valor);
}