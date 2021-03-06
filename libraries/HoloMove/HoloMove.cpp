#include "Arduino.h"
#include "HoloMove.h"
#include "RoboClaw.h"


HoloMove::HoloMove(RoboClaw* rb, HardwareSerial &ser) : _rb(rb), _ser(ser){

}

void HoloMove::move(float direction, float speed, float rotation, float *fl, float *fr, float *bl, float *br)
{

    // _ser.print("move: ");
    float radians = 71 * direction / 4068;
    // Please refer to README.txt for a full explanation of the formulas used.
    if (speed > 0)
    {
        float frontRightOutput = (-speed * cos((PI / 4) - radians)) + rotation,
            frontLeftOutput = (speed * cos((PI / 4) + radians)) + rotation,
            rearLeftOutput = (speed * cos((PI / 4) - radians)) + rotation,
            rearRightOutput = (-speed * cos((PI / 4) + radians)) + rotation;

        float values[] = { frontRightOutput, frontLeftOutput, rearLeftOutput, rearRightOutput };
        float maxValue = 0;
        for (int z = 0; z < 3; z++) {
            if (abs(values[z]) > maxValue) maxValue = abs(values[z]);
        }

        frontLeftOutput *=  speed / maxValue;
        frontRightOutput *= speed / maxValue;
        rearLeftOutput *= speed / maxValue;
        rearRightOutput *= speed / maxValue;

        *fr = -1 * frontRightOutput;
        *br = -1 *rearRightOutput;
        *bl = -1 *rearLeftOutput;
        *fl = -1 *frontLeftOutput;

    }

    else {
        *fr = 0;
        *br = 0;
        *bl = 0;
        *fl = 0;
    }

    _rb->ForwardBackwardM1(0x80, (uint8_t) map(*fr, -255, 255, 0, 127));
    _rb->ForwardBackwardM1(0x81, (uint8_t) map(*br, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x81, (uint8_t) map(*bl, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x80, (uint8_t) map(*fl, -255, 255, 0, 127));

    // _ser.println(*fr);

}
