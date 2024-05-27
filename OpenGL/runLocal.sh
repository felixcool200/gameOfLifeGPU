gcc \
    -o gameOfLife \
    -L/usr/lib \
    -Iinclude/ \
    src/main.c src/debug.c\
    -lglfw -lGLEW -lGL -lpthread \
    -Wall -Werror -O3 -lm

./gameOfLife