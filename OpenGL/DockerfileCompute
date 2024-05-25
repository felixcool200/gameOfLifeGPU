# Use the official ROCm development image
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    mesa-utils \
    libglfw3-dev \
    libglew-dev \
    && rm -rf /var/lib/apt/lists/*

# Create a working directory
WORKDIR /usr/local/src

# Copy the code to the container
COPY ./ /usr/local/src/gameOfLife

# Set the XDG_RUNTIME_DIR environment variable
ENV XDG_RUNTIME_DIR=/tmp/xdg
RUN mkdir -p /tmp/xdg && chmod 700 /tmp/xdg

# Compile the program with OpenGL/GLUT support
RUN cd /usr/local/src/gameOfLife && \
    gcc \
    -o gameOfLife \
    -L/usr/lib \
    -I/usr/local/src/gameOfLife/include \
    src/main.c \
    -lglfw -lGLEW -lGL -lpthread \
    -Wall -Werror -O3 -lm
#-lm -lX11 -lpthread -lXrandr -lXi -ldl \
#RUN apt-get update && apt-get install -y \
#    x11-apps

#ENTRYPOINT ["xeyes"]


# Set the entry point to run the compiled HIP program
ENTRYPOINT ["/usr/local/src/gameOfLife/gameOfLife"]
