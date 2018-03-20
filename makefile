CC=gcc-7
CFLAGS=--std=c++1z -Os
CDEFINES=-D__STDC_WANT_LIB_EXT1__=1
BOOSTLIB=$(BOOST)/stage/lib/libboost_serialization.a


OBJS=act1.o act2.o act3.o act4.o adv.o cevent.o defs.o dung.o\
	funcs.o globals.o info.o makstr.o mdlfun.o melee.o memq.o object.o\
	parser.o room.o roomfns.o rooms.o sr.o strings.o util.o zstring.o

all: zork_cpp

clean:
	rm *.o
	rm zork_cpp

%.o: %.cpp
	$(CC) $(CFLAGS) $(CDEFINES) -c -o $@ $<

zork_cpp: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lstdc++ $(BOOSTLIB)

