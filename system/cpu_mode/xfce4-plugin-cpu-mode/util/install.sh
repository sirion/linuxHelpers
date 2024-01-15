#!/bin/bash
sudo cp bin/libcpu_mode.so /usr/lib/xfce4/panel/plugins/
sudo cp cpu_mode.desktop /usr/share/xfce4/panel/plugins/

# See https://stackoverflow.com/questions/71847278/gtk-4-and-applications-icons-how-to-include-an-application-icon-in-a-portable
sudo cp -r bin/icons/hicolor /usr/share/icons/

sudo gtk-update-icon-cache -f -t /usr/share/icons/hicolor

# for i in /usr/share/icons/*; do
#     if [[ ! -d "$i" ]]; then
#         continue
#     fi
#     echo "Update icon cache $i..."
#     sudo gtk-update-icon-cache -f -t "$i"
# done

xfce4-panel -r