SHELL   = /bin/sh
CC      = g++
INSTALL = install
EXEC    = robots 

# Sources
SRCS_0 = bot3.cpp outstream.cpp state.cpp output.cpp
SRCS = $(SRCS_0) main.cpp
HDRS = $(SRCS_0:.cpp=.h) 
OBJS = $(SRCS:.cpp=.o)

EXECS = $(EXEC)

CFLAGS += -Wall -O2
LDLIBS += -lm -lncurses


.PHONY: all clean

# Build
all: $(EXECS)

clean:
	-rm -f $(OBJS) $(EXECS)

%.o: %.cpp $(HDRS)  
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

$(EXEC): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(EXEC)

