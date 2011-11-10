CC = g++

#EXTRA_CFLAGS = `sdl-config --cflags`
#EXTRA_LIBS = `sdl-config --libs`

OPT = -O -DNDEBUG
#OPT = -g -Wall

D =
#D = d

OBJS = channel.o config.o configEvent.o error.o frameData.o initData.o main.o node.o eqEarth.o pipe.o view.o window.o renderer.o sceneView.o viewer.o controls.o earthManipulator.o
CFLAGS = ${OPT} -isystem /afs/cmf/project/dc/sys/include -I/afs/cmf/project/dc/sys/include -I/afs/cmf/project/gis/include ${EXTRA_CFLAGS} -I.
LIBS = -L/afs/cmf/project/dc/sys/lib -losg${D} -losgViewer${D} -losgUtil${D} -lEqualizer -L/afs/cmf/project/gis/lib -losgEarth${D} -losgEarthUtil${D} ${EXTRA_LIBS}

all: eqEarth

eqEarth: ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

clean:
	/bin/rm -f *.o eqEarth

.SUFFIXES: 
.SUFFIXES: .o .cpp

.cpp.o:
	${CC} ${CFLAGS} -c $<
