DEPDIR := .d
$(shell mkdir -p $(DEPDIR) > /dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CC=gcc-7
CFLAGS=--std=c++1z -Os $(DEPFLAGS)
CDEFINES=-D__STDC_WANT_LIB_EXT1__=1
BOOSTLIB=$(BOOST)/stage/lib/libboost_serialization.a
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: zork_linux

clean:
	rm *.o
	rm zork_linux

%.o : %.cpp
%.o: %.cpp $(DEPDIR)/%.d
	$(CC) $(CFLAGS) $(CDEFINES) -c -o $@ $<
	$(POSTCOMPILE)

zork_linux: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lstdc++ $(BOOSTLIB)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))


