// Include the RoboClaw library
#include "RoboClaw.h"

// Create the RoboClaw object, passing the pointer to the hardware serial object
// and the serial timeout value
RoboClaw roboclaw(&Serial2, 10000);

void setup() {
// Begin serial communication at the given baudrate
roboclaw.begin(38400);
}

void loop()
{
// Call a method of RoboClaw object to control the motor controller



roboclaw.ForwardM1(0x80, 200);



} 
