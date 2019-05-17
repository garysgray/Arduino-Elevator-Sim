# Arduino-Elevator-Sim
Arduino Elevator Sim : This is mainly code for a work in progress project with some links and pdf to tutorials that helped get this together so far.

At this time the code is really coming along, the elevator has a life cycle and if run, one can push a floor button and watch the elevator go "up and down" from the digiatal display and LED's indicating floors reached/passed. Included a LED indicating when elevator stops and opens doors. One can also use the serial output to follow along  while it prints out the elevators differrent states and button presses. 

5/14/19
Update: Added code to help throttle the elevator update. Now one can see results of pushing buttons (in serial ouptu) while elevator is running 

5/15/19
Update: Life cycle is better and now user can press buttons and see elevator go to requested floors. Its still to simple and needs some adjustments for targeting best floor.
Random floor genarator taken out becuse it is no longer needed. Button presses now get added to "queue" and then checked during elevator life cycle.

5/16/19
Update: Got the elevator life cycle working much more like a real elevator with buttons. Know I have to clean up experimental hacky code and some rearrange some things. Next major change will be to give elevator different modes of operation, e.g user hits a fire button.

Where to find diagrams:

BUTTONS : https://www.instructables.com/id/Driving-Dozens-of-Buttons/,

LED     : in included PDF lesson: 16 pg: 109,

DISPLAY : in included PDF lesson: 19 pg: 127,
