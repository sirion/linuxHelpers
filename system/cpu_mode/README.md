# CPU Power Switch Util

A toll that switches the CPU governor for all cores.

## Switch to specific Governor

Switch CPU cores to power saving mode

```sh
cpu_mode powersave
```

Switch CPU to standard mode

```sh
cpu_mdoe schedutil
```

## Switch between Governors "powersave", "schedutil", "performance",

```sh
cpu_mode
```

## Switch between a user defined list of governors

```sh
cpu_mode performance schedutil powersave
```

## TODOs

- Currently installs directly to `/usr/bin`
