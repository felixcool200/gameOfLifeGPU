docker build -t game_of_life_shader -f Dockerfile .
xhost + local:docker
docker run --rm --gpus all \
  --net=host \
  --env DISPLAY=$DISPLAY \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw"\
  game_of_life_shader 1920 1080