#!/bin/bash

# Check if SDL2 is installed
if ! sdl2-config --version &> /dev/null
then
    echo "SDL2 is not installed. Please install SDL2 to proceed."
    exit 1
fi

command="gcc snake.c -Wall -Werror -o snake -I/usr/local/include/SDL2 -L/opt/homebrew/lib -lSDL2"

echo "Running build command"
echo $command
$command
