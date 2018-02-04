
.PHONY : all clean

all : inkscape-layers

inkscape-layers : inkscape-layers.cpp
	g++ -std=c++11 $^ `pkg-config libxml++-2.6 --cflags --libs` -o $@

clean :
	rm inkscape-layers
