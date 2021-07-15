#ifndef StateMachine_h
#define StateMachine_h
#include <Arduino.h>

typedef void (*Action)();

class StateMachine
{        
    public:
        /**
         * Default constructor.
         * @see StateMachine
         */
        StateMachine();

        /**
         * Constructor for StateMachine with maximum states.
         * @param max max number of states.
         */
        StateMachine(int max);
        ~StateMachine();

        /**
         * Call the Loop function for the current state. Meant to called once every update.
         */ 
        void StateLoop();

        /**
         * Return the current state.
         * @return int representing the state.
         */ 
        int GetState();

        /**
         * Return the previous state.
         * @return the previous state.
         */
        int GetPreviousState();

        /**
         * Set the current state.
         * @param state the state to be set to.
         */ 
        void SetState(int state);

        /**
         * Set the following for a given state: begin, loop, and end.
         * @param state the state.
         * @param begin the function called when switching from another state to this one. Called before loop.
         * @param loop the function called on StateLoop.
         * @param end the function called when exiting this state. Called before begin on the new state.
         * @see StateLoop
         */
        void SetCallbacks(int state, Action begin = nullptr, Action loop = nullptr, Action end = nullptr);
        
        /**
         * Start the StateMachine. Called after SetCallbacks. Calls begin for the initial state.
         * @param initialState the initial state.
         */ 
        void Start(int initialState = 0);

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