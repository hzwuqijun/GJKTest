SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp, bin/%.o, $(SRC))
OBJ+= bin/event_manager.o
EXE=partViewer

CC=g++
CFLAGS=-Wall -O3 -std=c++11 -I.
LDFLAGS= -lGLEW -lglut -lGL -lGLU -lAntTweakBar
RM=rm

vpath %.o bin/

bin/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
bin/event_manager.o: EventMgr/event_manager.cpp EventMgr/event_manager.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) bin/*
	@echo Clean Done!

