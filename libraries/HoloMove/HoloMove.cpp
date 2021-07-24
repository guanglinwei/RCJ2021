#include "Arduino.h"
#include "HoloMove.h"
#include "RoboClaw.h"


HoloMove::HoloMove(RoboClaw* rb, HardwareSerial &ser) : _rb(rb), _ser(ser){

}

void HoloMove::move(float direction, float speed, float rotation, float *fl, float *fr, float *bl, float *br)
{
    // round to nearest 15
    float _m = 15;
    direction += _m/2;
    direction -= fmod(direction, _m);
    if(speed > 0) {
        _ser.print("move: "); _ser.print(direction); _ser.print(" dir: "); _ser.println(rotation);
    }

    float radians = 71 * direction / 4068;

    float frontRightOutput = 0,
        frontLeftOutput = 0,
        rearLeftOutput = 0,
        rearRightOutput = 0;
    // Please refer to README.txt for a full explanation of the formulas used.
    if (speed > 0)
    {
        _ser.println("calculating outputs");
        frontRightOutput = (-speed * cos((PI / 4) - radians)) + rotation;
        _ser.println("no problem");
        frontLeftOutput = (speed * cos((PI / 4) + radians)) + rotation;
        rearLeftOutput = (speed * cos((PI / 4) - radians)) + rotation;
        rearRightOutput = (-speed * cos((PI / 4) + radians)) + rotation;

        float values[4] = { frontRightOutput, rearRightOutput, rearLeftOutput, frontLeftOutput };
        _ser.println("aa");
        float maxValue = 0;
        for (int z = 0; z < 4; z++) {
            _ser.print(z); _ser.print(" : "); _ser.println(values[z]);
            if (abs(values[z]) > maxValue) maxValue = abs(values[z]);
        }

        frontLeftOutput *=  -speed / maxValue;
        frontRightOutput *= -speed / maxValue;
        rearLeftOutput *= -speed / maxValue;
        rearRightOutput *= -speed / maxValue;

        // _ser.println("\nok\n");

        // *fr = frontRightOutput;
        // *br = rearRightOutput;
        // *bl = rearLeftOutput;
        // *fl = frontLeftOutput;

        _ser.println("sending motors");
        uint8_t t1 = (uint8_t) map(frontRightOutput, -255, 255, 0, 127);
        uint8_t t2 = (uint8_t) map(rearRightOutput, -255, 255, 0, 127);
        uint8_t t3 = (uint8_t) map(rearLeftOutput, -255, 255, 0, 127);
        uint8_t t4 = (uint8_t) map(frontLeftOutput, -255, 255, 0, 127);

        float _min = 5;

        // gw

        if(abs(frontRightOutput) <= _min) {
            _rb->ForwardM1(0x80, 0);
        }
        else { 
            _rb->ForwardBackwardM1(0x80, t1);
        }

        if(abs(rearRightOutput) <= _min) {
            _rb->ForwardM1(0x81, 0);
        }
        else {
            _rb->ForwardBackwardM1(0x81, t2);
        }

        if(abs(rearLeftOutput) <= _min) {
            _rb->ForwardM2(0x81, 0);
        }
        else { 
            _rb->ForwardBackwardM2(0x81, t3);
        }

        if(abs(frontLeftOutput) <= _min) {
            _rb->ForwardM2(0x80, 0);
        }
        else { 
            _rb->ForwardBackwardM2(0x80, t4);
        }

        // peter
        
        // if(abs(frontRightOutput) <= _min) {
        //     // _rb->ForwardM1(0x80, 0);
        //     _rb->ForwardM2(0x80, 0);
        // }
        // else { 
        //     // _rb->ForwardBackwardM1(0x80, t1);
        //     _rb->ForwardBackwardM2(0x80, t1);
        // }

        // if(abs(rearRightOutput) <= _min) {
        //     // _rb->ForwardM1(0x81, 0);
        //     _rb->ForwardM1(0x81, 0);
        // }
        // else {
        //     // _rb->ForwardBackwardM1(0x81, t2);
        //     _rb->ForwardBackwardM1(0x81, -t2);
        // }

        // if(abs(rearLeftOutput) <= _min) {
        //     // _rb->ForwardM2(0x81, 0);
        //     _rb->ForwardM2(0x81, 0);
        // }
        // else { 
        //     // _rb->ForwardBackwardM2(0x81, t3);
        //     _rb->ForwardBackwardM2(0x81, t3);
        // }

        // if(abs(frontLeftOutput) <= _min) {
        //     // _rb->ForwardM2(0x80, 0);
        //     _rb->ForwardM1(0x80, 0);
        // }
        // else { 
        //     // _rb->ForwardBackwardM2(0x80, t4);
        //     _rb->ForwardBackwardM1(0x80, -t4);
        // }

        _ser.print(frontRightOutput); _ser.print(" | "); _ser.print(rearRightOutput); _ser.print(" | ");
        _ser.print(rearLeftOutput); _ser.print(" | "); _ser.println(frontLeftOutput);
        _ser.println("_");
        _ser.print(t1); _ser.print(" | "); _ser.print(t2); _ser.print(" | ");
        _ser.print(t3); _ser.print(" | "); _ser.println(t4);
    }

    else {
        // *fr = 0;
        // *br = 0;
        // *bl = 0;
        // *fl = 0;
    }

}

void HoloMove::setSpeeds(float a, float b, float c, float d) {
    _rb->ForwardBackwardM1(0x80, (uint8_t) map(a, -255, 255, 0, 127));
    _rb->ForwardBackwardM1(0x81, (uint8_t) map(b, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x81, (uint8_t) map(c, -255, 255, 0, 127));
    _rb->ForwardBackwardM2(0x80, (uint8_t) map(d, -255, 255, 0, 127));
}

void HoloMove::stop() {
    _rb->ForwardM1(0x80, 0);
    _rb->ForwardM2(0x80, 0);
    _rb->ForwardM1(0x81, 0);
    _rb->ForwardM2(0x81, 0);
}
