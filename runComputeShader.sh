#docker build -t game_of_life_glut -f Dockerfile .
#xhost + local:docker
#docker run --rm --device=/dev/kfd --device=/dev/dri --group-add video \
#  --net=host \
#  --env DISPLAY=$DISPLAY \
#  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
#  --volume="$HOME/.Xauthority:/root/.Xauthority:rw"\
#  game_of_life_glut 1920 1080

docker build -t game_of_life_glut_nvidia -f Dockerfile .
xhost + local:docker
docker run --rm --gpus all \
  --net=host \
  --env DISPLAY=$DISPLAY \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw"\
  game_of_life_glut_nvidia 1920 1080