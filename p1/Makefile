CC=g++ -g -Wall -std=c++17 -D_XOPEN_SOURCE

# List of source files for your disk scheduler
SCHEDULER_SOURCES=disk.cpp #file2.cpp

# Generate the names of the disk scheduler's object files
SCHEDULER_OBJS=${SCHEDULER_SOURCES:.cpp=.o}

all: scheduler

# Compile the disk scheduler and tag this compilation
scheduler: ${SCHEDULER_OBJS} libthread.o
	./autotag.sh
	${CC} -o $@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${SCHEDULER_OBJS} scheduler
