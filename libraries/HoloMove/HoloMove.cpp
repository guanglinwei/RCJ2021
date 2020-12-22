#include "Arduino.h"
#include "HoloMove.h"
#include "RoboClaw.h"


HoloMove::HoloMove(const RoboClaw* rb) : _rb(rb){

}

void HoloMove::move(float direction, float speed, float rotation, float *a, float *b, float *c, float *d){

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

        frontLeftOutput *= speed / maxValue;
        frontRightOutput *= speed / maxValue;
        rearLeftOutput *= speed / maxValue;
        rearRightOutput *= speed / maxValue;

        *a = frontRightOutput;
        *b = rearRightOutput;
        *c = rearLeftOutput;
        *d = frontLeftOutput;

    }

    else {
        *a = 0;
        *b = 0;
        *c = 0;
        *d = 0;
    }

    _rb->ForwardBackwardM1(0x80, map(*a, -255, 255, 0, 127));
    _rb->ForwardBackwardM1(0x81, map(*c, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x81, map(*d, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x80, map(*b, -255, 255, 0, 127));

}