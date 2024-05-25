# Use the official ROCm development image
FROM rocm/dev-ubuntu-22.04:latest

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
#ENV ROCM_PATH=/opt/rocm
#ENV DEVICE_LIB_PATH ~/ROCm-Device-Libs/build/dist/amdgcn/bitcode/

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    mesa-utils \
    freeglut3-dev \
    && rm -rf /var/lib/apt/lists/*

# Create a working directory
WORKDIR /usr/local/src

# Copy the code to the container
COPY ./ /usr/local/src/gameOfLifeHIP

# Set the XDG_RUNTIME_DIR environment variable
ENV XDG_RUNTIME_DIR=/tmp/xdg
RUN mkdir -p /tmp/xdg && chmod 700 /tmp/xdg

# Compile the HIP program with OpenGL/GLUT support
RUN cd /usr/local/src/gameOfLifeHIP && \
    /opt/rocm/bin/hipcc \
    -o gameOfLifeHIP \
    -I /usr/local/src/gameOfLifeHIP/include \
    src/kernels.hip src/main.cpp \
    -lGL -lGLU -lglut -Wall -Werror -O3
#RUN apt-get update && apt-get install -y \
#    x11-apps

#ENTRYPOINT ["xeyes"]


# Set the entry point to run the compiled HIP program
ENTRYPOINT ["/usr/local/src/gameOfLifeHIP/gameOfLifeHIP"]
