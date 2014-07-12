
.PHONY : all clean

all : inkscape-layers

inkscape-layers : inkscape-layers.cpp
	g++ $^ `pkg-config libxml++-2.6 --cflags --libs` -o $@

clean :
	rm inkscape-layers
