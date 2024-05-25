/opt/rocm/bin/hipcc -o gameOfLifeHIP src/main.cpp src/kernels.hip\
    -Wall -Werror \
    -lGL -lGLU -lglut -Wall -Werror -O3

./gameOfLifeHIP
