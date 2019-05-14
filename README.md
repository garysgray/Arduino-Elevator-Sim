# Arduino-Elevator-Sim
Arduino Elevator Sim : This is mainly code for a work in progress project with some links and pdf to tutorials that helped get this together so far.

At this time the code is not finished, the elevator has a simple life cycle and if run the elevator will be given random floors and one can watch the elevator go "up and down" from the digiatal display and led's indicating floors reached/passed. one can also use the serial output to follow along.

***need to disable elevator to see button functionality at this time**

Buttons are working, and button presses will result in serial output stating which floor button was pressed, along with floor LED and display. The elevator simulation takes over the serial output from pushing buttons, so you need to disable it comment out this line in elevator_main.ino: #define ELEVATOR_LIVE.

Where to find diagrams:

BUTTONS : https://www.instructables.com/id/Driving-Dozens-of-Buttons/,

LED     : in included PDF lesson: 16 pg: 109,

DISPLAY : in included PDF lesson: 19 pg: 127,
