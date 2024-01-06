#!/bin/bash
sudo cp bin/libcpu_mode.so /usr/lib/xfce4/panel/plugins/
sudo cp cpu_mode.desktop /usr/share/xfce4/panel/plugins/
xfce4-panel -r