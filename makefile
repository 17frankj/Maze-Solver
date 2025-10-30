LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

shapes: make_shapes.c initShader.o myLib.o 
	gcc -o shapes make_shapes.c initShader.o myLib.o $(LIBS)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c

myLib.o: myLib.c myLib.h
	gcc -c myLib.c

