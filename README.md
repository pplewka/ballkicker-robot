# Ballkicker Robot

A LEGO Mindstorm robot capable of playing a simple ballkicking game.

Student project aiming to implement MISRA conforming embedded systems in cooperation with [AbsInt](https://www.absint.com/).

## Game

Two parties (robot or human) play against each other.
On the field are any number of balls.
Each round the player can kick a number of balls from the field.
The player kicking the last ball wins.

If there are a prime amount of balls on the field, the player can choose between kicking one or two balls.
If the amount is not prime, they must kick exactly one ball.

## Implementation

The project uses [ev3dev-c](https://github.com/in4lio/ev3dev-c) for interfacing with the robot hardware.

MISRA conformity and general runtime was checked by AbsInt's [Astrée](https://www.absint.com/astree/index.htm).

## Robot

Robot consists of 2 motors, 1 IR sensor and an emergency off button.

### Drive Motor

Port: D

Single motor for driving forwards and backwards a distance of ~1m.


### Hit Motor

Port: B

Motor for hitting the ball directly under the sensor.


### IR Sensor

Sensor for scanning a single ball under it.
Height must be ~15cm and balls should have a diameter of ~3cm with at least 3cm distance between them.

Because of single drive motor, all balls must be in a straight line.

### Emergency Off Button

Button directed at the front of the robot.

If the robot drives forward and button is pressed by an obstacle, the robot will stop driving forward.

## Game

Starting the program will result in the robot playing a single round of the game:

- Drive forward and scan the amount of balls
- Drive backwards and kick balls
- Park and beep, if game was won or lost.

## Building

After installing ev3dev-c, building can be done with the `CMakeLists.txt`.

`reset_brick.c` can be compiled (by hand) to get an executable, that stops all motors in case of errors in `main`.
