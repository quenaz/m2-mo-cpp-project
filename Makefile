CC = g++
ARCH = 
LIBS = -ljpeg
# Boost MUST be installed in order to use tests.
LIBS_TEST = -lboost_unit_test_framework-mt
# Both current directory and Port library directories supported.
LIBS_PATH = -L. -L/opt/local/lib
INCLUDE_PATH = -I. -I/opt/local/include/

GOAL = m2mo_brummer_oboturov_project
TEST_SUITE = tests

all: $(GOAL)

$(GOAL): image.o directory_listing.o  main.o
	$(CC) image.o directory_listing.o main.o -o $(GOAL) $(LIBS_PATH) $(LIBS) -Wall

test: image.o directory_listing.o image_test.o
	$(CC) image.o directory_listing.o image_test.o -o $(TEST_SUITE) $(LIBS_PATH) $(LIBS) $(LIBS_TEST) -Wall 

main.o: main.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -g main.cpp

image.o: image.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -g image.cpp

directory_listing.o: directory_listing.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -g directory_listing.cpp

image_test.o: image_test.cpp
	$(CC) $(INCLUDE_PATH) $(LIBS_PATH) -c -g image_test.cpp

clean:
	rm -rf *\.o $(GOAL) outputImages/*
