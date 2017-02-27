g++ -DPACKAGE=\"eo\" -DVERSION=\"0.9.3\" -I. -I src -Wall -g -c main.cpp 
g++ -Wall -g -o main main.o src/lib/libeo.a src/lib/libeoutils.a
