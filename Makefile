#Konstantinos Eirinaios Tsiampouris 7/15/2017


CXX      = clang++
LDFLAGS  = -Wall -Wextra -std=c++11 -O0 #-O3

all: snake

snake:	snake.cpp #interface.h interface.cpp
	${CXX} ${LDFLAGS} -o	snake	snake.cpp #interface.cpp

clean:
	rm -f snake
