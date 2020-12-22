#ifndef StateMachine_h
#define StateMachine_h
#include <Arduino.h>

typedef void (*Action)();

class StateMachine
{        

    public:
        StateMachine();
        StateMachine(int max);
        ~StateMachine();
        void StateLoop();
        int GetState();
        void SetState(int s);
        void SetCallbacks(int state, Action begin = nullptr, Action loop = nullptr, Action end = nullptr);

    private:
        int PreviousState;
        int State;
        Action *Begins;
        Action *Loops;
        Action *Ends;

        Action currentLoop;
        // HardwareSerial* serial;
};


#endif