# CPU Power XFCE4-Plugin

Simple XFCE4-Panel-Plugin to switch CPU governors using the cpu_mode tool.

## Compile and Install

*The cpu_mode tool has to be installed for this plugin to work.*

You will need the following packages (may differ slightly depending on your distribution):

- xfce4-dev-tools
- pkgconf
- [build-essential] (if you are on debian/ubuntu)

Use the following command to compile:

```sh
    ./util/make.sh
```

Use the following command to install:

```sh
    ./util/install.sh
```

## Development

### Get Log Output from xfce4-panel

Stop the panel and start it in a terminal with debug output to see what happened

```sh
killall xfce4-panel 
PANEL_DEBUG=1 xfce4-panel
```

### TODOs

- Switches only between hardcoded governors "schedutil" and "powersave"
- Icons might not be available on system
- No configuration
- (Un-)Installation just copies/removes files in hardcoded xfce4 system directories
- No i18n
- Does not adhere to xfce4-styleguide
