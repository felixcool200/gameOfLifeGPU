docker build -t game_of_life_hip -f Dockerfile .
xhost + local:docker
docker run --rm --device=/dev/kfd --device=/dev/dri --group-add video \
  --net=host \
  --env DISPLAY=$DISPLAY \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw"\
  game_of_life_hip 1920 1080