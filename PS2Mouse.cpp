#include "PS2Mouse.h"
#include "Arduino.h"

#define INTELLI_MOUSE 3
#define SCALING_1_TO_1 0xE6
#define RESOLUTION_8_COUNTS_PER_MM 3

enum Commands {
    SET_RESOLUTION = 0xE8,
    REQUEST_DATA = 0xEB,
    SET_REMOTE_MODE = 0xF0,
    GET_DEVICE_ID = 0xF2,
    SET_SAMPLE_RATE = 0xF3,
    RESET = 0xFF,
};

PS2Mouse::PS2Mouse(int clockPin, int dataPin) {
    _clockPin = clockPin;
    _dataPin = dataPin;
    _supportsIntelliMouseExtensions = false;
}

void PS2Mouse::high(int pin) {
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

void PS2Mouse::low(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void PS2Mouse::initialize() {
    high(_clockPin);
    high(_dataPin);
    reset();
    checkIntelliMouseExtensions();
    setResolution(RESOLUTION_8_COUNTS_PER_MM);
    setScaling(SCALING_1_TO_1);
    setSampleRate(40);
    setRemoteMode();
    delayMicroseconds(100);
}

void PS2Mouse::writeByte(char data) {
    int parityBit = 1;

    high(_dataPin);
    high(_clockPin);
    delayMicroseconds(300);
    low(_clockPin);
    delayMicroseconds(300);
    low(_dataPin);
    delayMicroseconds(10);

    // start bit
    high(_clockPin);

    waitForClockState(LOW);

    // data
    for (int i = 0; i < 8; i++) {
        int dataBit = bitRead(data, i);
        writeBit(dataBit);
        parityBit = parityBit ^ dataBit;
    }

    // parity bit
    writeBit(parityBit);

    // stop bit
    high(_dataPin);
    delayMicroseconds(50);
    waitForClockState(LOW);

    // wait for mouse to switch modes
    while ((digitalRead(_clockPin) == LOW) || (digitalRead(_dataPin) == LOW))
        ;

    // put a hold on the incoming data
    low(_clockPin);
}

void PS2Mouse::writeBit(int bit) {
    if (bit == HIGH) {
        high(_dataPin);
    } else {
        low(_dataPin);
    }

    waitForClockState(HIGH);
    waitForClockState(LOW);
}

char PS2Mouse::readByte() {
    char data = 0;

    high(_clockPin);
    high(_dataPin);
    delayMicroseconds(50);
    waitForClockState(LOW);
    delayMicroseconds(5);

    // consume the start bit
    waitForClockState(HIGH);

    // consume 8 bits of data
    for (int i = 0; i < 8; i++) {
        bitWrite(data, i, readBit());
    }

    // consume parity bit (ignored)
    readBit();

    // consume stop bit
    readBit();

    // put a hold on the incoming data
    low(_clockPin);

    return data;
}

int PS2Mouse::readBit() {
    waitForClockState(LOW);
    int bit = digitalRead(_dataPin);
    waitForClockState(HIGH);
    return bit;
}

void PS2Mouse::setSampleRate(int rate) {
    writeAndReadAck(SET_SAMPLE_RATE);
    writeAndReadAck(rate);
}

void PS2Mouse::writeAndReadAck(int data) {
    writeByte((char) data);
    readByte();
}

void PS2Mouse::reset() {
    writeAndReadAck(RESET);
    readByte();  // self-test status
    readByte();  // mouse ID
}

void PS2Mouse::checkIntelliMouseExtensions() {
    // IntelliMouse detection sequence
    setSampleRate(200);
    setSampleRate(100);
    setSampleRate(80);

    char deviceId = getDeviceId();
    _supportsIntelliMouseExtensions = (deviceId == INTELLI_MOUSE);
}

char PS2Mouse::getDeviceId() {
    writeAndReadAck(GET_DEVICE_ID);
    return readByte();
}

void PS2Mouse::setScaling(int scaling) {
    writeAndReadAck(scaling);
}

void PS2Mouse::setRemoteMode() {
    writeAndReadAck(SET_REMOTE_MODE);
}

void PS2Mouse::setResolution(int resolution) {
    writeAndReadAck(SET_RESOLUTION);
    writeAndReadAck(resolution);
}

void PS2Mouse::waitForClockState(int expectedState) {
    while (digitalRead(_clockPin) != expectedState)
        ;
}

MouseData PS2Mouse::readData() {
    MouseData data;

    requestData();
    data.status = readByte();
    data.position.x = readByte();
    data.position.y = readByte();

    if (_supportsIntelliMouseExtensions) {
        data.wheel = readByte();
    }

    return data;
};

void PS2Mouse::requestData() {
    writeAndReadAck(REQUEST_DATA);
}
