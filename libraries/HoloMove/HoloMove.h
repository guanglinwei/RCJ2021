#ifndef HoloMove_h
#define HoloMove_h

#include "Arduino.h"
#include "RoboClaw.h"

class HoloMove
{
	public:
		/**
		 * The constructor for HoloMove.
		 * @param rb pointer to RoboClaw object.
		 * @param ser reference to Arduino Serial.
		 */ 
		HoloMove(RoboClaw* rb, HardwareSerial &ser);

		/**
		 * Move the robot using 4 omniwheels.
		 * @param direction the angle to move in degrees (0-360). 90 is left, 270 is right.
		 * @param speed the speed of the motors (0-255).
		 * @param rotation the offset for the speed of each motor. Positive values rotate the robot clockwise, the opposite for negative values.
		 * @param a pointer that will contain the speed for motor A.
		 * @param b pointer that will contain the speed for motor B.
		 * @param c pointer that will contain the speed for motor C.
		 * @param d pointer that will contain the speed for motor D. 
		 */ 
		void move(float direction, float speed = 1, float rotation = 0, float *a = 0, float *b = 0, float *c = 0, float *d = 0);

		/**
		 * Set the speeds of the motors.
		 * @param a speed of motor A
		 * @param b speed of motor B
		 * @param c speed of motor C
		 * @param d speed of motor D
		 */
		void setSpeeds(float a = 0, float b = 0, float c = 0, float d = 0);

		/**
		 * Stop the robot from moving.
		 */ 
		void stop();
	private:
		RoboClaw* _rb;
		HardwareSerial &_ser;
};	

#endif