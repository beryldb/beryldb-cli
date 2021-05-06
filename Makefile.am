CC                  =   g++
CFLAGS              =   -std=c++14 -O3 -Wno-write-strings -Wall -Wshadow 
OBJS                =   server.o methods.o configprocess.o utils.o handlers.o nodes.o engine.o stackconf.o base.o filehandler.o match.o
LIBS                =   -Iemerald/include 

all:    ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LIBS}  emerald/src/emerald.cpp -o beryl-cli
	-rm *.o

base.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o base.o emerald/src/base.cpp

stackconf.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o stackconf.o emerald/src/stackconf.cpp

nodes.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o nodes.o emerald/src/nodes.cpp

handlers.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o handlers.o emerald/src/handlers.cpp

utils.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o utils.o emerald/src/utils.cpp

methods.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o methods.o emerald/src/methods.cpp

engine.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o engine.o emerald/src/engine.cpp

server.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o server.o emerald/src/server.cpp

configprocess.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o configprocess.o emerald/src/configprocess.cpp

filehandler.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o filehandler.o emerald/src/filehandler.cpp

match.o:
	${CC} ${CFLAGS} ${LIBS}  -c -o match.o emerald/src/match.cpp

clean:
	rm -vf *.o 
	rm -rf beryl-cli
	rm -rf config.log
	rm -rf Makefile
	rm -rf autom4te.cache
	rm -rf aclocal.m4
