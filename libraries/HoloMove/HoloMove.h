#ifndef HoloMove_h
#define HoloMove_h

#include "Arduino.h"
#include "RoboClaw.h"

class HoloMove
{
	public:
		HoloMove(const RoboClaw* rb);
		void move(float direction, float speed = 1, float rotation = 0, float *a = 0, float *b = 0, float *c = 0, float *d = 0);
	private:
		RoboClaw* _rb;

};	

#endif