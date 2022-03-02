#include "beep.h"

// Including stdio is against MISRA guidelines.
// We need stdio because on our current system beep is not possible without printf / stdio
// include it in this c file only so scope of stdio stays in this c file
#include <stdio.h>

static bool doBeep(void);

static bool doBeep(void)
{
    return (printf("\a\n") == 2);
}

bool ballFound(void){
    return doBeep();
}

bool victoryBeep(void){
    bool all_ok = (bool) true;

    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(500000)) != (int8_t) 0){
        all_ok = (bool) false;
    }
    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(1000000)) != (int8_t) 0){
        all_ok = (bool) false;
    }
    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(1000000)) != (int8_t) 0){
        all_ok = (bool) false;
    }
    return all_ok;
}

bool failureBeep(void){
    bool all_ok = (bool) true;

    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(1000000)) != (int8_t) 0){
        all_ok = (bool) false;
    }
    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(500000)) != (int8_t) 0){
        all_ok = (bool) false;
    }
    if(!doBeep()){
        all_ok = (bool) false;
    }
    if(((int8_t)usleep(1000000)) != (int8_t) 0){
        all_ok = (bool) false;
    }

    return all_ok;
}

bool errorBeep(void)
{
    bool all_ok = (bool) true;

    for(uint8_t i = 0; i < (uint8_t) 4; i++){
        if(!doBeep()){
            all_ok = (bool) false;
        }
        if(((int8_t)usleep(500000)) != (int8_t) 0){
            all_ok = (bool) false;
        }
    }

    if(((int8_t)usleep(1000000)) != (int8_t) 0){
        all_ok = (bool) false;
    }

    return all_ok;
}
