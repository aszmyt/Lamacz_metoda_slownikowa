#!/bin/bash

g++ -g -c lab10.cpp -o prog.o
sleep 0.0001
g++ -o lamanie prog.o -lncurses -lpthread -lcrypto