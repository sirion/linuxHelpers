#!/bin/sh

go build -o cpu_mode cpu_mode.go && \
sudo cp cpu_mode /usr/bin/cpu_mode && \
sudo chown root:root /usr/bin/cpu_mode && \
sudo chmod +s /usr/bin/cpu_mode