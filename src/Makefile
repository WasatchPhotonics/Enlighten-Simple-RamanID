APP = identify

SRCS_C   = $(wildcard *.c)
SRCS_CPP = $(wildcard *.cpp)

SRCS     = $(SRCS_C) $(SRCS_CPP)
OBJS     = $(SRCS_C:.c=.o) $(SRCS_CPP:.cpp=.o)

CXXFLAGS += --std=c++11 -O3
CFLAGS   += -std=c99 -O3

# added for MinGW, which we're no longer using
# CC       = gcc
# LFLAGS  += -static-libgcc -static-libstdc++

all: $(APP)
	mkdir -p ../bin && cp -f $(APP) ../bin

clean:
	rm -f $(APP) $(APP).exe *.o

new: clean all

$(APP): $(OBJS)
	$(CXX) -o $(APP) $(CXXFLAGS) $(LFLAGS) $(OBJS)
