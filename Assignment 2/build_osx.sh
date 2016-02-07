LIB_DIR=./lib_osx
g++ -o a2 assignment2.cpp -F/Library/Frameworks -framework SDL2 -I. -L${LIB_DIR} -lSDL2_gfx
