#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include <ev3_tacho.h>
#include <unistd.h>
#include "beep.h"

#define HIT_MOTOR_PORT      OUTPUT_B
#define HIT_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define DRIVE_MOTOR_PORT      OUTPUT_D
#define DRIVE_MOTOR_EXT_PORT  EXT_PORT__NONE_

static int32_t max_speed;  /* Motor maximal speed */
enum { HIT_MOTOR, DRIVE_MOTOR, HIT_MOTOR_2 };
static uint8_t motor[ 3 ] = { DESC_LIMIT, DESC_LIMIT, DESC_LIMIT };  /* Sequence numbers of motors */
static uint8_t button_sn = 0;
static uint8_t distance_sn = 0;
static int32_t distance_playfield = 1750; // ~1 meter with big wheels directly connected to motor

static bool driveHold(void);
static bool hitBall(void);
static bool hitReset(void);
static bool init(void);
static int8_t buttonPressed(void);
static int16_t countBalls(void);
static int16_t returnToInitPosAndKick(uint16_t counter_balls);
static int32_t DEGREE_TO_COUNT(int32_t d);

// return 1 on pressed, 0 on not pressed, -1 on error
static int8_t buttonPressed(void){
    bool all_ok = (bool) true;
    int8_t ret_val = (int8_t) -1;
    int32_t val = 0;
    
    if(get_sensor_value( 0, button_sn, &val ) == (size_t) 0){
        all_ok = (bool) false;
    }

    if(!all_ok){
        ret_val = (int8_t) -1;
    }
    else{
        if(val != (int32_t) 0){
            ret_val = 1;
        }
        else{ ret_val = 0;
        }
    }

    return ret_val;
}

static int32_t DEGREE_TO_COUNT(int32_t d) {
    return ((d) * 360 / 90 );
}

//core init to get the robot ready for the game
// returns true for success, false for fail
static bool init(void){
    bool all_ok = (bool) true;
    int8_t s[ 256 ];
    
    if(ev3_sensor_init() == -1){
        all_ok = (bool) false;
    }

    if(ev3_search_sensor(LEGO_EV3_TOUCH, &button_sn, 0) == (bool) false){
        all_ok = (bool) false;
    }

    if(ev3_search_sensor(LEGO_EV3_US, &distance_sn, 0) == (bool) false){
        all_ok = (bool) false;
    }

    if(all_ok == (bool) true){
        if(ev3_tacho_init() == -1){
            all_ok = (bool) false;
        }
        if ( ev3_search_tacho_plugged_in(HIT_MOTOR_PORT, HIT_MOTOR_EXT_PORT, &(motor[HIT_MOTOR]), 0 )) {
            if(get_tacho_max_speed( motor[HIT_MOTOR], &max_speed ) == (size_t) 0){
                all_ok = (bool) false;
            }
            /* Reset the motor */
            if(set_tacho_command_inx( motor[HIT_MOTOR], TACHO_RESET ) == (size_t) 0){
                all_ok = (bool) false;
            }
        } else {
            /* Inoperative without left motor */
            all_ok = (bool) false;
        }
        if ( ev3_search_tacho_plugged_in(DRIVE_MOTOR_PORT, DRIVE_MOTOR_EXT_PORT, &(motor[DRIVE_MOTOR]), 0 )) {
            /* Reset the motor */
            if(set_tacho_command_inx( motor[DRIVE_MOTOR], TACHO_RESET ) == (size_t) 0){
                all_ok = (bool) false;
            }
        } else {
            /* Inoperative without right motor */
            all_ok = (bool) false;
        }
    }
    return all_ok;
}

//#region core movement
// returns rel_pos on success and -1 on failure
static int64_t driveToRelPos(int32_t rel_pos){
    bool all_ok = (bool) true;
    int32_t ret_val = 0;

    if(set_tacho_speed_sp( motor[DRIVE_MOTOR], max_speed / 8 ) == (size_t) 0) {
        all_ok = (bool) false;
    }
	if(set_tacho_position_sp( motor[DRIVE_MOTOR], rel_pos) == (size_t) 0){
        all_ok = (bool) false;
    }
	if(all_ok == (bool) true){
        if(set_tacho_command_inx( motor[DRIVE_MOTOR], TACHO_RUN_TO_REL_POS ) == (size_t) 0) {
            all_ok = (bool) false;
        }
    }

    if(all_ok == (bool) false) {
        ret_val = (int32_t) -1;
    }
    else{
        ret_val = rel_pos;
    }

    return ret_val;
}

// returns true if successful, false, if not
static bool driveHold(void){
    bool success = (bool) true;

    if(set_tacho_command_inx( motor[DRIVE_MOTOR], TACHO_STOP ) == (size_t) 0){
        success = (bool) false;
    }

    return success;
}

// returns true if successful, false, if not
static bool hitBall(void){
    bool all_ok = (bool)true;

    if(set_tacho_polarity_inx(motor[HIT_MOTOR], TACHO_INVERSED) == (size_t)0){
        all_ok = (bool)false;
    }
    if(set_tacho_speed_sp(motor[HIT_MOTOR], max_speed) == (size_t)0){
        all_ok = (bool)false;
    }
    if(set_tacho_position_sp(motor[HIT_MOTOR], DEGREE_TO_COUNT(180)) == (size_t)0){
        all_ok = (bool)false;
    }
    if(all_ok == (bool) true){
        if(set_tacho_command_inx(motor[HIT_MOTOR], TACHO_RUN_TO_REL_POS) == (size_t)0){
            all_ok = (bool)false;
        }
    }

    return all_ok;
}

// returns true if successful, false, if not
static bool hitReset(void){
    bool all_ok = (bool) true;

    if(set_tacho_speed_sp(motor[HIT_MOTOR], max_speed / 4) == (size_t)0){
        all_ok = (bool) false;
    }
    if(set_tacho_polarity_inx(motor[HIT_MOTOR], TACHO_NORMAL) == (size_t)0){
        all_ok = (bool) false;
    }
    if(set_tacho_position_sp(motor[HIT_MOTOR], DEGREE_TO_COUNT(180)) == (size_t)0){
        all_ok = (bool) false;
    }

    if(all_ok == (bool) true){
        if(set_tacho_command_inx(motor[HIT_MOTOR], TACHO_RUN_TO_REL_POS) == (size_t)0){
            all_ok = (bool)false;
        }
    }

    return all_ok;
}

//#endregion core movement

//#region handler functions
//robot needs to drive forward, count balls and get back to starting position, may need to be used before every round
static int16_t countBalls(void){
    int8_t s[ 256 ];
    int32_t val = 0; //distance val sensor
    uint8_t i = 0, ii = 0;
    uint8_t sn_touch, sn_color, sn_ir;
    uint16_t counter_balls = 0;
    bool isBall = (bool) false;
    bool all_ok = (bool) true;
    bool drive = (bool) true;
    int8_t sleep_state = 0;
    //avg distance to tabletop 80-90 val
    int32_t rel_pos = 0;
    int16_t ret_val = 0;

    if(driveToRelPos(distance_playfield) != distance_playfield){
        all_ok = (bool) false;
    }
    
    while((drive) && (all_ok) && (counter_balls < (uint16_t) 255)){
        if ( get_sensor_value( ii, distance_sn, &val ) != (size_t) 0) {
            //distance = get sensor reading
            if((val <= 80) && (!isBall))
            {
                isBall = (bool) true;
                counter_balls++;
                if(!ballFound()){
                    all_ok = (bool) false;
                }
                if(usleep(600000) != 0) {
                    all_ok = (bool) false;
                }
            }
            if((val > 80) && (isBall))
            {
                if(usleep(600000) != 0) {
                    all_ok = (bool) false;
                }
                isBall = (bool) false;
            }
            //can be triggered through touchsensor
            if(buttonPressed() == (int8_t) 1){
                drive = (bool) false;
                if(driveHold() == (bool) false){
                    all_ok = (bool) false;
                }
            }
        }
        
        if(get_tacho_position(motor[DRIVE_MOTOR], &rel_pos) == (size_t) 0){
            all_ok = (bool) false;
        }

        if((all_ok == (bool) true) && (rel_pos >= distance_playfield)){
            drive = (bool) false;
            
            if(driveHold() == (bool) false){
                all_ok = (bool) false;
            }
        }
    }

    if((counter_balls == (uint16_t) 255) || (all_ok == (bool) false)){
        ret_val = -1;
    }
    else{
        ret_val = (int16_t) counter_balls;
    }

    return ret_val;
}

// returns remaining balls on success or -1
static int16_t returnToInitPosAndKick(uint16_t counter_balls){
    int32_t val = 0; //distance val sensor
    int32_t rel_pos = 0;
    uint16_t counter_required_hits = 0;
    uint8_t i = 0, ii = 0;
    bool isBall = (bool) false;
    bool all_ok = (bool) true;
    bool drive = (bool) true;
    int8_t sleep_state = 0;
    int16_t ret_val = 0;
    uint16_t counter_remaining_balls = counter_balls;

    if(get_tacho_position(motor[DRIVE_MOTOR], &rel_pos) == (size_t) 0){
        all_ok = (bool) false;
    }
    if(driveToRelPos(-distance_playfield) != (-distance_playfield)){
        all_ok = (bool) false;
    }
    if(all_ok == (bool) true){
        // determine how many balls we kick
        if((counter_remaining_balls == (uint16_t) 2) || (counter_remaining_balls == (uint16_t) 5)){
            //hit 2
            counter_required_hits = 2;
        }
        else{
            // hit 1
            counter_required_hits = 1;
        }
        if(counter_remaining_balls == (uint16_t) 0){
            //return error?
            counter_required_hits = 0;
        }

        while((drive) && (all_ok) && (counter_required_hits > (uint16_t) 0) && (counter_remaining_balls > (uint16_t) 0)){
            if ( get_sensor_value( ii, distance_sn, &val ) != (size_t) 0) {
                //distance = get sensor reading
                //update display with ballcounter
                if((val <= 80) && (!isBall))
                {
                    isBall = (bool) true;
                    if(driveHold() != (bool) true){
                        all_ok = (bool) false;
                    }
                    if(all_ok == (bool) true){
                        if(hitBall()){
                            counter_required_hits--;
                            counter_remaining_balls--;
                        }
                        if(usleep(1000000) != 0){
                            all_ok = (bool) false;
                        }
                        if(hitReset() != (bool) true){
                            all_ok = (bool) false;
                        }
                        if(usleep(2000000) != 0){
                            all_ok = (bool) false;
                        }
                        
                        if(get_tacho_position(motor[DRIVE_MOTOR], &rel_pos) != (size_t) 0)
                        {
                            // intentionally left empty. if 0 then tacho has no new value for us
                            // is ok with the frequency we check this
                        }
                        if(all_ok == (bool) true) {
                            if(driveToRelPos(-rel_pos + 10) > 0){
                                all_ok = (bool) false;
                            }
                        }
                    }
                }
                if((val > 80) && (isBall))
                {
                    isBall = (bool) false;
                }
            }
        }

        bool waitToFinish = (bool) true;
        while(waitToFinish){
            if(get_tacho_position(motor[DRIVE_MOTOR], &rel_pos) != (size_t) 0){
                // intentionally left empty. if 0 then tacho has no new value for us
                // is ok with the frequency we check this
            }
            if(rel_pos <= 10){
                waitToFinish = (bool) false;
            }
            if(((int8_t) usleep(100000)) != (int8_t) 0){
                all_ok = (bool) false;
            }
        }
    }
    
    if(all_ok == (bool) false){
        ret_val = -1;
    }
    else{
        ret_val = (int16_t) counter_remaining_balls;
    }
    return (int16_t) ret_val;
}

//#endregion handler functions

int main(void)
{
    bool all_ok = (bool) true;
    int32_t rel_pos = 0;
    int16_t counter_balls = 0;
    int16_t response;
    int32_t ret_val = 0;

    int32_t result_init = ev3_init();
    if (result_init < 1 ) {
        all_ok = (bool) false;
    }
    
    if(all_ok == (bool) true){
        if(init() == (bool) false){
            all_ok = (bool) false;
            if(!errorBeep()){
                all_ok = (bool) false;
            }
        }
        if(all_ok == (bool) true){
            counter_balls = countBalls();
            if(counter_balls == -1){
                all_ok = (bool) false;
            }
            if(usleep(2000000) != 0){
                all_ok = (bool) false;
            }

            if(all_ok == (bool) true){
                response = returnToInitPosAndKick((uint16_t) counter_balls);
                if(response == -1){
                    all_ok = (bool) false;
                }
            }
            if(usleep(1000000) != 0){
                all_ok = (bool) false;
            }
        }
    }

    if(all_ok == (bool) true){
        if((counter_balls == 1) || (counter_balls == 2)){
            if(victoryBeep() == (bool) false){
                all_ok = (bool) false;
            }
        }
        if((counter_balls == 4) || (counter_balls == 3)){
            if(failureBeep() == (bool) false){
                all_ok = (bool) false;
            }
        }
    }

    if(!all_ok){
        if(errorBeep() == (bool) false){
            // if even error beep fails, then return -1
            ret_val = (int32_t) - 1;
        }
    }
    
    return ret_val;
}



