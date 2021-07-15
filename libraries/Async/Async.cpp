#include <Async.h>

Async::Async(HardwareSerial &serial) : _serial(&serial){
    m_CallbackHead = new m_CallbackInfo;
}

// Async::setup() {

// }

void Async::update() {
    m_CallbackInfo *current = m_CallbackHead->next;
    m_CallbackInfo *prev = m_CallbackHead;
    while(current != nullptr) {
        _serial->print("check "); _serial->println(current->endTime);
        if(millis() > current->endTime) {
            current->callback();
            if(!current->repeat) {
                prev->next = current->next;
                delete current;
            }
            else {
                current->endTime += current->delay;
            }
        }
        prev = current;
        current = current->next;
    }
}

void Async::setTimeout(Action callback, unsigned long delay) {
    char cringe[50];
    _serial->print("add "); _serial->println(delay);
    m_CallbackInfo *temp = m_CallbackHead->next;
    sprintf(cringe, "%lu", temp->delay);
    _serial->println((temp == nullptr) ? "null" : cringe);
    m_CallbackInfo add(callback, millis() + delay, delay, false, (temp == nullptr) ? nullptr : temp);
    
    sprintf(cringe, "%lu", add.next->delay);
    _serial->println((add.next == nullptr) ? "addnull" : cringe);
    _serial->println((temp == nullptr) ? "tempnull" : "tempok");
    m_CallbackHead->next = &add;
    
    list();
    // *m_CallbackHead = *(m_CallbackHead->next);
}

void Async::setInterval(Action callback, unsigned long delay) {
    m_CallbackInfo *temp = m_CallbackHead->next;
    m_CallbackInfo *add(callback, millis() + delay, delay, true, temp);
    m_CallbackHead->next = add;
}

void Async::list() {
    _serial->println("\n");
    m_CallbackInfo *current = m_CallbackHead->next;
    while(current != nullptr) {
        _serial->print(" -"); _serial->println((current->next == nullptr) ? "null" : "ok");
        current = current->next;
    }
}