#!/bin/bash

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker buildx create --name multiarch --driver docker-container --use
docker buildx build --tag leosac/leosac:snapshot --platform linux/amd64,linux/arm64/v8,linux/arm/v7 -f docker/Dockerfile.main .
