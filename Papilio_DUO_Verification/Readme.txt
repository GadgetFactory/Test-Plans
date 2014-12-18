Papilio DUO Test Plan - Current Version 1.4

Test Plan Home Page:
http://www.gadgetfactory.net/opmanuals/index.php?n=Main.PapilioPro

CHANGELOG
12/17/2014 Version 1.4
	Changed the drive strength of the FT2232H to 8mA due to some boards failing the test plan.
	Fixed the serial number for the FT2232H
	Increased the timeout between FT2232H EEPROM writes.

11/24/2014 Version 1.3
	Updated I/O test to operate correctly with Seeeduino ADK as the stimulus board. All disconnected pins are reported in an easier to understand way.
	Changed the Quickstart sketch so the FPGA side does not interfere with AVR side.

11/17/2014 Version 1.2
	Updated the Mem I/O test to change the default value of pin 13 in order to work with Seeed's stimulus board.
	http://www.seeedstudio.com/depot/Seeeduino-ADK-Main-Board-p-846.html

11/3/2014 Version 1.1
	Fix for missing avrdude dependencies
	Added Papilio DUO drivers.

9/4/2014 Version 1.0
	Converted from the Papilio Pro test plan for the Papilio DUO.