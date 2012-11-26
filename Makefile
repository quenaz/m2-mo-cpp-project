CC = g++
ARCH = 
LIBS = -ljpeg
LIBS_TEST = -lboost_unit_test_framework-mt
LIBS_PATH = -L/opt/local/lib
INCLUDE_PATH = -I/opt/local/include/

GOAL = m2mo_brummer_oboturov_project
TEST_SUITE = tests

all: $(GOAL)

$(GOAL): image.o main.o
	$(CC) $(LIBS_PATH) $(LIBS) -Wall image.o main.o -o $(GOAL)

test: image.o image_test.o
	$(CC) $(LIBS_PATH) $(LIBS) $(LIBS_TEST) -Wall image.o image_test.o -o $(TEST_SUITE)

main.o: main.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -O3 main.cpp

image.o: image.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -O3 image.cpp

image_test.o: image_test.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -O3 image_test.cpp

clean:
	rm -rf *\.o $(GOAL) outputImages/*
