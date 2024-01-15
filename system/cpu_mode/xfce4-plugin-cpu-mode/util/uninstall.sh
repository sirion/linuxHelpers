#!/bin/bash
sudo rm /usr/lib/xfce4/panel/plugins/libcpu_power.so
sudo rm /usr/share/xfce4/panel/plugins/cpu_power.desktop
. bin/uninstall_icons.sh
xfce4-panel -r
## TODO: Remove icons