#!/bin/bash
docker build --rm -t so2-lab .

xhost + ${hostname}

docker run \
    --rm -it \
    --name SO2-Lab \
    -e DISPLAY=host.docker.internal:0 \
    -v ${PWD}/../..:/workspaces \
    so2-lab "/bin/bash"
