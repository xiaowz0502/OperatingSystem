CC=g++ -g -Wall -std=c++17

# List of source files for your file server
FS_SOURCES=fs_server.cpp

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

all: fs app test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13 test14 test15 test16 test17 test18 test19 test20

# Compile the file server and tag this compilation
fs: ${FS_OBJS} libfs_server_macos.o
	./autotag.sh
	${CC} -o $@ $^ -pthread -ldl

# Compile a client program
app: test1.cpp libfs_client_macos.o
	${CC} -o $@ $^ -pthread -ldl

test%: test%.cpp libfs_client_macos.o
	${CC} -o $@ $^ -pthread -ldl

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs app test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13 test14 test15 test16 test17 test18 test19 test20
