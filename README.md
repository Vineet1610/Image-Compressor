# Image-Compressor
This is a simple, graceful and a well-built image compressor which uses black and white images of type pgm as its input file. In this program the grayscale value of each pixel ranges from 0 to 255 and the image is compressed using the following inputs: input image file, maximum depth variable and maximum grayscale difference. This program was built using C language and was run in Linux Operating System using a Makefile.

This program was given as a coding assignment to us by the professor of CSCB09(Software Tools and Systems Programming) at University of Toronto. This program uses quad trees type data structure to subdivide the image into 4 subdivisions on the basis of maximum grayscale difference and maximum depth variable. The program works properly in the memory as well that is there are no memory leaks when the quad tree is constructed or deleted. The memory leaks are checked using valgrind feature on the Linux Terminal.

When you run the program and use test.pgm as the input file, you get output.pgm as your output.
