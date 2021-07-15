#ifndef ASYNC_H
#define ASYNC_H

#include <Arduino.h>

// template<typename... Args> typedef void (*Action)(Args...);
typedef void (*Action)();

class Async {
    private:
        struct m_CallbackInfo {
            Action callback;
            unsigned long endTime;
            unsigned long delay;
            bool repeat;
            m_CallbackInfo *next;

            m_CallbackInfo() : next(nullptr) {};

            m_CallbackInfo(Action callback, unsigned long endTime, unsigned long delay, bool repeat, m_CallbackInfo *next)
            : callback(callback), endTime(endTime), delay(delay), repeat(repeat), next(next) {};
        };
        m_CallbackInfo *m_CallbackHead;
        HardwareSerial *_serial;

    public:
        Async(HardwareSerial &serial);
        // void setup();
        void update();
        void setTimeout(Action callback, unsigned long delay);
        void setInterval(Action callback, unsigned long delay);
        void list();
};

#endif