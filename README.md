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

5/19/19
Update: really just a ton of clean up, finally made controller a class and basicly made it so it holds all the key elements. Now updateElevator gets called as a controller object called from main loop().
Everything else is a class now as well. Cleaned up controllers elevator update a lot. Its a lot easier to follow now. Still a little bit of repative/long code but untill life cycle is completly figured out its easier to follow. Then later wrap repative code in functions. Also added the function calls to have a LED light up for elevator direction while its moving along with Floor LED. pretty cool

5/20/19
Update: Cleaned up some more code. also added in the different mode buttons functionality. so now non-floor button presses can change the mode of the controller. For example
pushing the FIRE button will make it so elevator will automaticlly start going to floor 1 and stop and open doors on each floor. Elevator no longer takes floor requests and once its
on floor 1 it stays there untill FIRE button is pressed again, then it goes into NORMAL mode again.

5/25/19
Update: Biggest change is in the way the LED's work now. They originally where used to indicate which floor elevator was on. Now its used to represent floors in the queue from button presses, ha which makes sense. Also added a passive buzzer which makes a chirp when floor stops and more of a buzz when mode/service buttons pushed. Anorther change is that modes are working  as my frined had requested. Upon mode button presses the elevator goes into FIRE_1 or FIRE_2 mode which forces eleveator to go to designated floors and stay there. As always a little more code clean up as well. Mainly fixing parts where I not passing inn whole objects, just need values.  

Where to find diagrams:

BUTTONS : https://www.instructables.com/id/Driving-Dozens-of-Buttons/,

LED     : in included PDF lesson: 16 pg: 109,

DISPLAY : in included PDF lesson: 19 pg: 127,
