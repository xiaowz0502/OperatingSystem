CC=g++ -g -Wall -std=c++17 -D_XOPEN_SOURCE

# List of source files for your thread library
THREAD_SOURCES=cpu.cpp thread.cpp common.cpp mutex.cpp cv.cpp

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}

all: libthread.o app

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh
	ld -r -o $@ ${THREAD_OBJS}

# Compile an application program
app: test20.cpp libthread.o libcpu.o
	${CC} -o $@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o app
