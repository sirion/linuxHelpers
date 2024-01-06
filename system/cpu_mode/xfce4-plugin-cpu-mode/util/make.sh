#!/bin/bash
name=cpu_mode # Change this to your plugin's name

mkdir -p bin
gcc -Wall -Werror -shared -o bin/lib${name}.so -fPIC ${name}.c `pkg-config --cflags --libs libxfce4panel-2.0` `pkg-config --cflags --libs gtk+-3.0`
