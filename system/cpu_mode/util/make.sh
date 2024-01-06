#!/bin/sh

mkdir -p bin && \
go build -o bin/cpu_mode cpu_mode.go