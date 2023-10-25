#!/bin/sh

RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
NORM="\e[0m"

CC="cc"
CFLAGS="-pipe -Ofast -ansi -Werror -Wpedantic -Wall -std=c89 -Wno-overlength-strings"
LIBS="-lGL -ldl -lX11 -lXi -lm"

if [ "$1" = "clean" ]; then
  echo -e "${YELLOW}[CLEAN]${NORM} Removing binaries..."
  rm *.o
  rm *.bin
  echo -e "${YELLOW}[CLEAN]${GREEN} Done.${NORM}"
  exit 0
fi

function build_info {
  echo -e "${YELLOW}[BUILD]${NORM} $1"
}

if ! $(test -f glad.o); then
  build_info "Building glad..."
  ${CC} -Ofast -c ../glad/src/glad.c
fi
if ! $(test -f glfw-ez.o); then
  build_info "Building glfw-ez..."
  ${CC} -Ofast -w -c ./glfw-ez/glfw-ez.c
fi
build_info "Building the demo..."
if ! $(${CC} ${CFLAGS} -c demo.c); then
  echo -e "${RED}[ERROR]${NORM} Couldn't build demo."
  exit 1
fi
build_info "Linking the demo..."
if ! $(${CC} *.o ${LIBS} -o demo.bin); then
  echo -e "${RED}[ERROR]${NORM} Couldn't link the demo."
  exit 1
fi
build_info "Stripping the demo..."
strip demo.bin
build_info "${GREEN}Done.${NORM}"

