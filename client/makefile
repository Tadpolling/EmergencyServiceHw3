CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
# TODO change the makefile for StompEMIlient
all: StompEMIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompEMIClient: bin/ConnectionHandler.o bin/event.o bin/StompClient.o bin/StompMessageCreator.o bin/utils.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/StompMessageCreator.o bin/utils.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompMessageCreator.o: src/StompMessageCreator.cpp
	g++ $(CFLAGS) -o bin/StompMessageCreator.o src/StompMessageCreator.cpp
	
bin/utils.o: src/utils.cpp
	g++ $(CFLAGS) -o bin/utils.o src/utils.cpp

.PHONY: clean
clean:
	rm -f bin/*
	