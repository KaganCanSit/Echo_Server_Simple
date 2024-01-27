# Simple Echo Server
An echo server example in which multiple clients can be connected. Created with C ++. It can be used in a simple way with the terminal. For this;

## Addictions
C++11 or higher standart

## Compile
Go to the directory where your project is located to compile and use the server.
To use the server;
> g++ server.cpp -o server
> ./server <port number>

After compiling the server with the above operations, perform the following steps through a new BASH window. You must connect to the server with the client to the port address you have given as a parameter.
To use the client;
>nc localhost <port number>

And write your message!

## Simples
> ./server localhost 8080

> nc localhost 8080
> Hello World!

## Information Sources:
https://www.youtube.com/watch?v=CFe5LQOPdfk

https://www.yusufsezer.com.tr/c-cpp-soket/

https://my.eng.utah.edu/~cs4400/concurrency.pdf

## Licence
This project is licensed under the MIT License. See LICENSE file for details.