#!/bin/bash

# This script allows to easily recreate docker images manually
# It shouldn't be required as it is normally triggered by the CI

IMAGE_BUILD_1="maxhy/leosac-buildsystem:debian-bullseye"
IMAGE_BUILD_2="maxhy/leosac-buildsystem:debian-buster"
IMAGE_RUN="maxhy/leosac:snapshot"
PLATFORMS="linux/amd64,linux/arm64,linux/arm/v7,linux/arm/v6"

# Build system
read -p "Do you want to re-build the Docker Buildsystem for Leosac first?" bs
case $bs in
  [Yy]* )
      docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
      docker buildx create --name multiarch --driver docker-container --use
      docker buildx build --push --tag $IMAGE_BUILD_1 --platform $PLATFORMS -f docker/buildsystem/debian/Dockerfile.bullseye .
      docker buildx build --push --tag $IMAGE_BUILD_2 --platform $PLATFORMS -f docker/buildsystem/debian/Dockerfile.buster .
  * ) echo "Docker Buildsystem skipped";;
esac

read -p "Do you want to re-build the Docker for Leosac?" ds
case $ds in
  [Yy]* )
      for platform in ${PLATFORMS//,/ } ; do
        rm ./build -rf
        rm ./obj-* -rf
        docker run --rm --entrypoint=/bin/bash --platform $platform -v "${PWD}:/tmp/leosac" $IMAGE_BUILD -c "TARGETPLATFORM=${platform} && VERSION=snapshot && BUILD_DATE=$(date -u +'%Y-%m-%dT%H:%M:%SZ') && VCS_REF=HEAD && export TARGETPLATFORM VERSION BUILD_DATE VCS_REF && /tmp/leosac/docker_scripts/build_leosac.sh"
      done
      docker buildx build --push --tag $IMAGE_RUN --platform $PLATFORMS --file docker/Dockerfile.main build/packages/debian
  * ) echo "Docker skipped";;
esac
