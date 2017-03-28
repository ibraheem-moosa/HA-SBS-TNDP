g++  -DPACKAGE=\"eo\" -DVERSION=\"0.9.3\" -I. -I src  -g -c main.cpp 
g++   -g -o main main.o src/lib/libeo.a src/lib/libeoutils.a
