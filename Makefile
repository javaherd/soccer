all: soccer videoWriter

soccer: soccer.cpp
	g++ soccer.cpp -std=c++11 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -o soccer.o

videoWriter: videoWriter.cpp
	g++ videoWriter.cpp -std=c++11 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -o videoWriter.o

