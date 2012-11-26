CC = g++
ARCH = 
LIBS = -ljpeg
LIBS_PATH = -L/opt/local/lib
INCLUDE_PATH = -I/opt/local/include/

GOAL = m2mo_brummer_oboturov_project

all: $(GOAL)

$(GOAL): image.o main.o
	$(CC) $(LIBS_PATH) $(LIBS) -Wall image.o main.o -o $(GOAL)

main.o: main.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -O3 main.cpp

image.o: image.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -O3 image.cpp

clean:
	rm -rf *\.o $(GOAL) outputImages/*
