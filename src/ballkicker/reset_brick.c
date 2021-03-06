#include <stdio.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include <ev3_tacho.h>

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif


#define HIT_MOTOR_PORT      OUTPUT_D
#define HIT_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define DRIVE_MOTOR_PORT      OUTPUT_B
#define DRIVE_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define IR_CHANNEL        0

#define SPEED_LINEAR      75  /* Motor speed for linear motion, in percents */
#define SPEED_CIRCULAR    50  /* ... for circular motion */

int max_speed;  /* Motor maximal speed */
enum { L, R };
uint8_t motor[ 3 ] = { DESC_LIMIT, DESC_LIMIT, DESC_LIMIT };  /* Sequence numbers of motors */

int main( void )
{
    printf( "Hello, LEGO World!\n" );
    printf("\a ");
    Sleep(1000);
    if ( ev3_init() < 1 ) return ( 1 );

    ev3_sensor_init();
    printf( "Found sensors:\n" );
    uint32_t n, i,  ii, val;
    char s[ 256 ];
    for ( i = 0; i < DESC_LIMIT; i++ ) {
        if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
            printf( "  type = %s\n", ev3_sensor_type( ev3_sensor[ i ].type_inx ));
            printf( "  port = %s\n", ev3_sensor_port_name( i, s ));
            if ( get_sensor_mode( i, s, sizeof( s ))) {
                printf( "  mode = %s\n", s );
            }
            if ( get_sensor_num_values( i, &n )) {
                for ( ii = 0; ii < n; ii++ ) {
                    if ( get_sensor_value( ii, i, &val )) {
                        printf( "  value%d = %d\n", ii, val );
                    }
                }
            }
        }
    }
    ev3_tacho_init();
    if ( ev3_search_tacho_plugged_in(HIT_MOTOR_PORT, HIT_MOTOR_EXT_PORT, motor + L, 0 )) {
        get_tacho_max_speed( motor[ L ], &max_speed );
        /* Reset the motor */
        set_tacho_command_inx( motor[ L ], TACHO_RESET );
        printf("Found motor at port %c\n", HIT_MOTOR_PORT);
    } else {
        printf( "LEFT motor (%s) is NOT found.\n", ev3_port_name(HIT_MOTOR_PORT, HIT_MOTOR_EXT_PORT, 0, s ));
        /* Inoperative without left motor */
        return ( 0 );
    }
    if ( ev3_search_tacho_plugged_in(DRIVE_MOTOR_PORT, DRIVE_MOTOR_EXT_PORT, motor + R, 0 )) {
        /* Reset the motor */
        set_tacho_command_inx( motor[ R ], TACHO_RESET );
        printf("Found motor at port %c\n", DRIVE_MOTOR_PORT);
    } else {
        printf( "RIGHT motor (%s) is NOT found.\n", ev3_port_name(DRIVE_MOTOR_PORT, DRIVE_MOTOR_EXT_PORT, 0, s ));
        /* Inoperative without right motor */
        return ( 0 );
    }
    multi_set_tacho_command_inx( motor, TACHO_STOP );
    ev3_uninit();

    return 0;
}
