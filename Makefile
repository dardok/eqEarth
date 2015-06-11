CC = g++

#EXTRA_CFLAGS = `sdl-config --cflags`
#EXTRA_LIBS = `sdl-config --libs`

CC = gcc
CXX = g++
OPT = -O -DNDEBUG
#OPT = -g -Wall

D =
#D = d

OBJS = channel.o config.o configEvent.o error.o frameData.o initData.o main.o node.o eqEarth.o pipe.o view.o window.o renderer.o sceneView.o viewer.o controls.o earthManipulator.o
CFLAGS = ${OPT} ${EXTRA_CFLAGS} -I.
#CFLAGS = -DEQ_IGNORE_GLEW ${OPT} -I/var/tmp/dkleiner/dev/Buildyard/Build/install/include -isystem /afs/cmf/project/dc/sys/include -I/afs/cmf/project/dc/sys/include -I/afs/cmf/project/gis/include ${EXTRA_CFLAGS} -I.
#LIBS = -lboost_serialization -lboost_system -lboost_date_time -losg${D} -losgViewer${D} -losgUtil${D} -lEqualizer -losgEarth${D} -losgEarthUtil${D} ${EXTRA_LIBS}
LIBS = -losg -losgViewer -losgUtil -lEqualizer -losgEarth -losgEarthUtil -lGL -losgGA -losgText -losgDB -lLunchbox -lEqualizerFabric -lCollage ${EXTRA_LIBS}
#LIBS = -Wl,-rpath -Wl,/var/tmp/dkleiner/dev/Buildyard/Build/install/lib -L/var/tmp/dkleiner/dev/Buildyard/Build/install/lib -L/afs/cmf/project/dc/sys/lib -losg${D} -losgViewer${D} -losgUtil${D} -lEqualizer -L/afs/cmf/project/gis/lib -losgEarth${D} -losgEarthUtil${D} ${EXTRA_LIBS}

all: eqEarth

eqEarth: ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

clean:
	/bin/rm -f *.o eqEarth

.SUFFIXES: 
.SUFFIXES: .o .cpp

.cpp.o:
	${CC} ${CFLAGS} -c $<
