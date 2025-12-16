

## Docker 18.04 build
```bash
docker build -f .devcontainer/Dockerfile.9 -t gazebo:v9 .

docker run --rm -it \
--name gazebo_banch \
--hostname gazebo_banch \
--user user \
--net host \
-w /home/user \
-e DISPLAY=$DISPLAY \
-e QT_X11_NO_MITSHM \
-v /tmp/.X11-unix:/tmp/.X11-unix \
-v /home/user/projects/workspace:/workspace \
--device /dev/dri:/dev/dri \
gazebo:v9 \
/bin/bash

```
