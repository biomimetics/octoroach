Last updated 8/3/2011


This directory will contain a stable version for the code for the OctoROACH robot,
intended to provide an "out of the box" working code base.



Instructions:
-------------
To be done later.



Python:
---------
Running the OctoRoACH code will require you to install a python interpreter, as well as several libraries.

For academic applications, a bundled product by Enthought (www.enthought.com) will contain most of these libraries.
You MUST install a 32bit version. 64bit python is known to not have sufficient library support to work.

Specifically, you will need:
* pySerial
* numPy
* xbee-python

- shared.py is used as a "global" module, to elide the need for long lists of "global" keywords in other scripts.
  *** IMPORTANT: Configuration of the COM port and baud rate of your Xbee/15.4 dongle is set by variables in shared.py
- callbackFunc.py contains the callback function used by the xbee-python library. It is kept in a separate file such
	that any additions or changes will be synchronized across all the user's python scripts.
	A dictionary of packet formats is also located in callbackFunc.py. If you are adding a command to the robot,
	you must add the callback handler here, as well as the packet format.
- command.py contains the command codes for the robot.
	*** Because the command set for the robot is not standardized or enforced in any way, you MUST manually synchronize
		the listings in command.py and cmd.c/cmd.h if any modifications are made.



Firmware:
-----------
The firmware for the OctoRoACH robot is implemented in plain C, to be run on the dsPIC33g microcontroller on the
ImageProc2 v2.4 board.
The code has been developed and tested with the MPLAB C30 compiler, under and academic use license.
A project file for use with the MPLAB IDE is provided; code was programmed to the board with a MicroChip ICD3 tool.

The source code is in two locations:
- lib/ contains code broken into modules, aligned to specific functions of the robot. See code comments
	    for more information.
- octoroach/ contains the MPLAB project files
- octoroach/source/ contians the main.c, which largely just setups up the board and other modules, and cmd.c and cmd.h,
	which describe the command set that the robot will repond to over the 15.4 radio.
	*** Because the command set for the robot is not standardized or enforced in any way, you MUST manually synchronize
		the listings in command.py and cmd.c/cmd.h if any modifications are made.

The project is delivered with a compiler directive error purposely inserted into a header file, where the radio settings
defined. It is REQUIRED that the user modifies these settings, and comments out the error directive.


There are several compile-time defines available:

__RADIO_HIGH_DATA_RATE : operates the 15.4 radio in 500Kbps mode and accordingly reduces some feedback delays. Only use this
			option if you are using the Fearing Lab 802.15.4 USB dongle at 230Kbaud.
__DFMEM_8MBIT / __DFMEM_16MBIT / __DFMEM_32MBIT:
			This option must correspond to the size of the AT45 serial flash chip installed on the boad. It is
			required to have at least one of these options defined.


Radio:
------------
The ImageProc2 v2.4 board uses an 802.15.4 radio for communication. Operation has been tested with an XBee module and the Fearing Lab 802.15.4 USB dongle.
The user is required to review and change the radio settings before the firmware project will compile. No binaries images with pre-set
radio addresses are provided at this time.