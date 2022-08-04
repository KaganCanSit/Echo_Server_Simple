# Echo Server Simple

An echo server example in which multiple clients can be connected. Created with C ++. It can be used in a simple way with the terminal. For this;

Go to the directory where your project is located to compile and use the server.
To use the server;
> g++ -std=c++11 echo_server.cpp -o server

> ./server 

<hr>

After compiling the server with the above operations, perform the following steps through a new BASH window. You must connect to the server with the client to the port address you have given as a parameter.
To use the client;
>nc localhost 

And write your message!

When you send the message, the server will appear in Bash. 
