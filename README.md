# Arduino-Elevator-Sim
Arduino Elevator Sim : This is mainly code for a work in progress project with some links and pdf to tutorials that helped get this together so far.

At this time the code is not finished, the elevator has a simple life cycle and if run, the elevator will be given random floors and one can watch the elevator go "up and down" from the digiatal display and LED's indicating floors reached/passed. One can also use the serial output to follow along. At this time button presses do not tell elevator which floor to go to, buttons will seem unresponsive.

Added code to help throttle the elevator update now one can see results of pushing buttons (in serial ouptu) while elevator is running 

5/15/19
update: life cycle is better and now user can press buttons and go to floors. its to simple and needs some adjustments to picking the right order of floors to target.
random floor s taken out no longer neede. button presses get added to "queue"

5/16/19
update: got the elevator working much more like a real elevator, just have to clean up code and then start fine tuning stuff

Where to find diagrams:

BUTTONS : https://www.instructables.com/id/Driving-Dozens-of-Buttons/,

LED     : in included PDF lesson: 16 pg: 109,

DISPLAY : in included PDF lesson: 19 pg: 127,
