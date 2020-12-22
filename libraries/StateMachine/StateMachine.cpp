#include "StateMachine.h"
#include "Arduino.h"

StateMachine::StateMachine(){

}
StateMachine::StateMachine(int max)
{
    Begins = new Action[max];
    Loops = new Action[max];
    Ends = new Action[max];
    State = -1;
    PreviousState = -1;
}

StateMachine::~StateMachine()
{
    delete [] Begins;
    delete [] Loops;
    delete [] Ends;
}

void StateMachine::SetCallbacks(int state, Action begin, Action loop, Action end)
{
    Begins[state] = begin;
    Loops[state] = loop;
    Ends[state] = end;
}

int StateMachine::GetState()
{
    return State;
}

void StateMachine::SetState(int state)
{
    State = state;
    if(PreviousState != -1){
        if(Ends[PreviousState])
            Ends[PreviousState]();
    }
    if(Begins[PreviousState])
        Begins[PreviousState]();
    currentLoop = Loops[State];
}

void StateMachine::StateLoop()
{
    if(currentLoop) currentLoop();
}