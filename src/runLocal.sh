/opt/rocm/bin/hipcc -o gameOfLifeHIP main.cpp kernels.hip\
    -Wall -Werror \
    -lGL -lGLU -lglut -Wall -Werror -O3

./gameOfLifeHIP
