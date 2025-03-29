#ifndef RADIOMANAGER_H
#define RADIOMANAGER_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

class RadioManager {
public:
    RadioManager(uint8_t cePin, uint8_t csnPin, const byte* address);
    bool begin();
    bool sendMessage(const char* message);
    bool receiveMessage(char* buffer, uint8_t bufferSize);
    void clearMessage();
    
private:
    RF24 radio;
    byte address[6];
};

#endif
