# CPU Power Switch Util

A tool that switches the CPU governor for all cores.

## Usage

This prgram can be used on the command line or with an [XFCE4-panel-plugin](xfce4-plugin-cpu-mode/) written specifically for this purpose.

### Switch to specific Governor

Switch CPU cores to power saving mode

```sh
cpu_mode powersave
```

Switch CPU to standard mode

```sh
cpu_mdoe schedutil
```

### Switch between Governors "powersave", "schedutil", "performance"

```sh
cpu_mode
```

### Switch between a user defined list of governors

```sh
cpu_mode schedutil powersave
```

## Compile and Install

You will need go installed on your system.

Use the following command to compile:

```sh
    ./util/make.sh
```

Use the following command to install:

```sh
    ./util/install.sh
```

## TODOs

- Currently installs directly to `/usr/bin`
