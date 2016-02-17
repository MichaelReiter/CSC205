LIB_DIR=./lib_linux
g++ -g -Wall -o LSViewer LSViewer.cpp LSystem.cpp -lSDL2 -I. -L${LIB_DIR} -lSDL2_gfx

