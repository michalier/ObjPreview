UNAMES := $(shell uname -o)
ifeq      (${UNAMES},GNU/Linux)
COPTS = 
CLIBS = -lGL -lglfw -lGLEW -ldl
EXE = 
else ifeq (${UNAMES},Msys)
COPTS = -I /msys64/mingw64/include/
CLIBS = -L /msys64/mingw64/lib/ -lopengl32 -lglfw3 -lglew32
EXE = .exe
COPTS += -DARCH_MSYS=${UNAMES}
else
TARGET = unknown-target
defaulterror:
	echo Unknown target architecture: uname -s 
endif

SRC = $(shell find . -type f -name '*.cpp')
OBJS = $(SRC:%.cpp=%.o)

default: $(OBJS)
	g++ -std=c++0x -I. $(COPTS) -o prac $(OBJS) $(CLIBS)

%.o: %.c
	g++ -std=c++17 -c $< -o $@

clean:
	@-rm -f prac$(EXE)
	@-find . -name '*.o' -delete
