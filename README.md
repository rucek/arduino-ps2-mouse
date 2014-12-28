# arduino-ps2-mouse

A library to interact with a PS/2 mouse. The mouse works in the remote mode, i.e. you need to explicitly ask it to send the data.

The library provides information about the pressed buttons, mouse position and the wheel(s) rotation count(s), if any. The following data structure is used:

```c
typedef struct {
	int status;
	Position position;
	int wheel;
} MouseData;
```

where the `Position` is defined as:

```c
typedef struct {
	int x, y;
} Position;
```

The `position` and `wheel` values are relative to the previously retrieved data, i.e. the are all 0's if the mouse/wheel(s) positions haven't changed since last read.

If the mouse has two wheels, the `wheel` value is incremented/decremented by 1 when the vertical scroll wheel is moved and incremented/decremented by 2 when the horizontal scroll wheel is moved (that's the way Microsoft has implemented their IntelliMouse standard).

See http://computer-engineering.org/ps2mouse for details.

## Usage

1. Identify the power, clock and data lines in the PS/2 cable, see http://www.computer-engineering.org/ps2protocol for details.

2. Connect the power lines to Arduino's +5V and GND.

3. Connect the data and clock lines to two Arduino's digital outputs.

4. Include the header file in your sketch and create a `PS2Mouse` instance with the numbers of the clock and data pins as constructor parameters:

	```c
	#include "PS2Mouse.h"
	#define DATA_PIN 5
	#define CLOCK_PIN 6

	PS2Mouse mouse(CLOCK_PIN, DATA_PIN);
	```

5. Initialize the mouse in the `setup()`:

	```c
	mouse.initialize();
	```

6. In the `loop()` use `PS2Mouse::readData()` to retrieve current position and wheel data, with a small delay between subsequent reads:

	```c
	MouseData data = mouse.readData();
	delay(20);
	```

	See [example.ino](example.ino) for a complete example.

## Credits

- http://computer-engineering.org/ps2mouse
- http://www.computer-engineering.org/ps2protocol
- http://playground.arduino.cc/ComponentLib/Ps2mouse
- https://github.com/kristopher/PS2-Mouse-Arduino
