Physics-sim-and-concurrency
==============

This was the Project for a course I have done in the University of Hull, when I was studying in Simulation and Concurrency.
This project consists of 2 programs: a Server, and a client. 

The server’s job is to handle a simulation of the balls in an enclosed and sending the data to the clients(s).
The clients job is to communicate with the server and render the data to the user.
Both program are design to be multi-threaded  to ensure the simulation/rendering will not conflict with
any network communication between the server and client.

Currently I am trying to improve the program by improving the way the server and client send information to each other, 
and also adding in proper rigid body physics and implement fourth order runge-kutta integration method for 
the simulation.

You can run both the server and the client on the same computer or on separate computers, 
however it may not work in separate computer since the coursework require us to use a specific port to communicate, 
which could be blocked in your network. The program is also currently set to run in 1980*1080,
a resize feature will be implemented in the future.

When using the client, you can create new balls by clicking in the enclosure,
pressing “B” to toggle rope simulation, 
WASD keys to move around and +,- to zoom in and out, 
and the “C” key will reset the simulation.
