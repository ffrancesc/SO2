#!/bin/bash
docker build --rm -t so2-lab .

docker run \
    --rm -it \
    --name SO2-Lab \
    -e DISPLAY=host.docker.internal:0 \
    so2-lab
    #-v ${PWD}/zeos:/home/zeos \