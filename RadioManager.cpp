#include "RadioManager.h"
byte broadcastAddress[5] = { 'A', 'L', 'L', '_', 'B' };
RadioManager::RadioManager(uint8_t cePin, uint8_t csnPin, const byte* addr)
  : radio(cePin, csnPin) {
  memcpy(address, addr, sizeof(this->address));
}
char* lastMessage = "";
bool RadioManager::begin() {
  if (!radio.begin()) {
    Serial.println("⚠️ NRF24L01 no detectado.");
    return false;
  }
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.openReadingPipe(2, broadcastAddress);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(100);
  radio.startListening();
  Serial.println("✅ NRF24L01 inicializado correctamente.");
  return true;
}

void RadioManager::clearMessage() {
  lastMessage = "";
}

bool RadioManager::sendMessage(const char* message) {
  if (message != lastMessage) {
    radio.stopListening();
    Serial.println(message);
    bool success = radio.write(message, strlen(message) + 1);
    if (success) {
      lastMessage = message;
    }
    radio.startListening();
    return success;
  } else {
    return true;
  }
}

bool RadioManager::receiveMessage(char* buffer, uint8_t bufferSize) {
  memset(buffer, 0, bufferSize);
  if (radio.available()) {
    radio.read(buffer, bufferSize);
    return true;
  }
  return false;
}
