#ifndef MOUSE_H_

#define MOUSE_H_

typedef struct {
    int x, y;
} Position;

typedef struct {
    int status;
    Position position;
    int wheel;
} MouseData;

class PS2Mouse {
private:
    int _clockPin;
    int _dataPin;
    bool _supportsIntelliMouseExtensions;

    void high(int pin);

    void low(int pin);

    void writeAndReadAck(int data);

    void reset();

    void setSampleRate(int rate);

    void checkIntelliMouseExtensions();

    void setResolution(int resolution);

    char getDeviceId();

    void setScaling(int scaling);

    void setRemoteMode();

    void waitForClockState(int expectedState);

    void requestData();

    char readByte();

    int readBit();

    void writeByte(char data);

    void writeBit(int bit);

public:
    PS2Mouse(int clockPin, int dataPin);

    void initialize();

    MouseData readData();
};

#endif // MOUSE_H_