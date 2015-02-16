CXX = g++
LD = g++
STRIP = strip

OBJS = $(CFILES:.cc=.o)

CXXFLAGS = -Wall -g -Os -std=c++11 -DRELEASE # -DDUMP_FRAMES
STRIPFLAGS = -s -R .comment -R .gnu.version
LIBS = -nostdlib -nostartfiles -lSDL -lGL # -lc

CFILES = \
	intro.cc \
	program.cc

TARGET = intro

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LIBS)
	$(STRIP) $(STRIPFLAGS) $(TARGET)

pack: $(TARGET)
	gzip -c $(TARGET) | cat stub.sh - > packed-intro

depend: .depend

.depend: $(CFILES)
	rm -f .depend
	$(CXX) $(CXXFLAGS) -MM $^ > .depend;

clean:
	rm -f *o $(TARGET)

include .depend

.PHONY: all clean depend
