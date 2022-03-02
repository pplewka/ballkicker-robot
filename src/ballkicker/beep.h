#ifndef BEEP_H
#define BEEP_H

#include "ev3.h"
#include <unistd.h>


bool ballFound(void);

bool victoryBeep(void);

bool failureBeep(void);

bool errorBeep(void);

#endif // BEEP_H