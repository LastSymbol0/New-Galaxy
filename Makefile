SRC = src/*.cpp dist/jsoncpp.cpp

INCL = -I./incl -I./dist

STD = -std=c++11

all:
	g++ -O2 -g $(STD) $(SRC) $(INCL)
