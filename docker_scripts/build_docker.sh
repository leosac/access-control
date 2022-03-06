#!/bin/bash

IMAGE_BUILD="maxhy/leosac-buildsystem:latest"
IMAGE_RUN="maxhy/leosac:snapshot"
PLATFORMS="linux/amd64,linux/arm64,linux/arm/v7"

# Build system
read -p "Do you want to re-build the Docker buildsystem for Leosac first?" bs
case $bs in
  [YyOo]* )
      docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
      docker buildx create --name multiarch --driver docker-container --use
      docker buildx build --push --tag $IMAGE_BUILD --platform $PLATFORMS -f docker/Dockerfile.buildsystem .
      break;;
  * ) echo "Buildsystem skipped";;
esac

for platform in ${PLATFORMS//,/ } ; do
  docker run --rm --entrypoint=/bin/bash --platform $platform -v "${PWD}:/tmp/leosac" $IMAGE_BUILD -c "TARGETPLATFORM=${platform} && VERSION=snapshot && BUILD_DATE=$(date -u +'%Y-%m-%dT%H:%M:%SZ') && VCS_REF=HEAD && export TARGETPLATFORM VERSION BUILD_DATE VCS_REF && /tmp/leosac/docker_scripts/build_leosac.sh"
done
docker buildx build --push --tag $IMAGE_RUN --platform $PLATFORMS --file docker/Dockerfile.main build/packages/debian
