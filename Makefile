all: soccer videoWriter

soccer: soccer.cpp
	g++ soccer.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -o soccer.o

videoWriter: videoWriter.cpp
	g++ videoWriter.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -o videoWriter.o

