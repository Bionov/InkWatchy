#ifndef BUTTONS_H
#define BUTTONS_H

#include "defines/defines.h"

typedef enum 
{
    None,
    Back,
    Menu,
    Up,
    Down,
} buttonState;

extern buttonState buttonPressed;
extern int RTC_DATA_ATTR UP_PIN;
extern uint64_t RTC_DATA_ATTR UP_MASK;

buttonState useButton();
void initButtons();
void buttonsLoop();
void setButton(buttonState button);

#if DEBUG
void dumpButtons();
String getButtonString(buttonState state);
#endif

#endif
