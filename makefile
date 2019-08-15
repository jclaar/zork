DEPDIR := .d
$(shell mkdir -p $(DEPDIR) > /dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
UNAME=$(shell uname -s)

ifeq ($(UNAME),Darwin)
	CC=clang
	OUTFILE=zork_mac
else
	CC=gcc-7
	OUTFILE=zork_linux
endif

CFLAGS=--std=c++17 -Os $(DEPFLAGS)
CDEFINES=-D__STDC_WANT_LIB_EXT1__=1
BOOSTLIB=$(BOOST)/stage/lib/libboost_serialization.a
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(OUTFILE)

clean:
	rm -f *.o
	rm -f $(OUTFILE)
	rm -f *.gch

%.o : %.cpp
%.o: %.cpp $(DEPDIR)/%.d stdafx.h.gch
	$(CC) $(CFLAGS) $(CDEFINES) -c -o $@ $<
	$(POSTCOMPILE)

stdafx.h.gch: stdafx.h
	$(CC) $(CFLAGS) $(CDEFINES) -x c++-header stdafx.h -c -o stdafx.h.gch

$(OUTFILE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lstdc++ $(BOOSTLIB)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))


