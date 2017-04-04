if [ $# -eq 0 ]
    then
        output_name=main
    else
        output_name=$1
fi

g++  -std=c++11 -march=native -Wall -DPACKAGE=\"eo\" -DVERSION=\"0.9.3\" -I. -I src -c main.cpp 
g++  -std=c++11 -march=native -Wall -o $output_name main.o src/lib/libeo.a src/lib/libeoutils.a
rm main.o
