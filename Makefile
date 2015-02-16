CXX = g++
LD = g++
STRIP = strip

OBJS = $(CFILES:.cc=.o)

CXXFLAGS = -Wall -g -Os -std=c++11 -DRELEASE
STRIPFLAGS = -s -R .comment -R .gnu.version
LIBS = -nostdlib -nostartfiles -lSDL -lGL

CFILES = \
	intro.cc \
	texture.cc \
	program.cc

TARGET = demo

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LIBS)
	$(STRIP) $(STRIPFLAGS) $(TARGET)

depend: .depend

.depend: $(CFILES)
	rm -f .depend
	$(CXX) $(CXXFLAGS) -MM $^ > .depend;

clean:
	rm -f *o $(TARGET)

include .depend

.PHONY: all clean depend
