all: run

run: soccer
	./soccer.o

soccer: soccer.cpp
	g++ soccer.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -o soccer.o
