#!/bin/bash

### Configuration

# Required programs to build the plugin
requirements=("gcc" "pkg-config" "basename" "sed" "convert" "find")

# Icon versions to create
icon_steps=("0" "10" "25" "50" "75" "90" "100")
icon_fills=("#000000" "#00ff00" "#22ff00" "#66ff00" "#999900" "#ff6600" "#ff0000")

# Icons sizes to create
# icon_sizes=("8" "16" "22" "24" "32" "36" "42" "44" "48" "64" "72" "96" "128" "150" "192" "256" "310" "512")
# icon_sizes=("16" "22" "24" "44" "48" "64" "72" "96" "128" "256" "512")
icon_sizes=("24" "48")

### Functions

# Globals
tmpDir=$(mktemp -d -q)

# Checks
if [[ "${#icon_steps[@]}" != "${#icon_fills[@]}" ]]; then
	echo "Configuration error: icon_steps and icon_fills must have the same length" >&2
	exit 2
fi

function check_prerequisites() {
	# Check for required programs before starting 
	requiredOk=1
	for prg in "${requirements[@]}"; do
		path=$(which "$prg")
		if [[ "" == "$path" ]]; then
			requiredOk=0
			echo "ERROR: Required program '$prg' not found" >&2
		fi
	done

	if [[ $requiredOk -eq 0 ]]; then
		echo "Please install missing requirements" >&2
		return 1
	fi
}

function compile() {
	mkdir -p bin
	# gcc -Wall -Werror -shared -o bin/libcpu_mode.so -fPIC cpu_mode.c $(pkg-config --cflags --libs libxfce4panel-2.0) $(pkg-config --cflags --libs gtk+-3.0)
	options="-Wall -Werror -shared"
	if [[ "$DEBUG" == "1" ]]; then
		# No errors for warnings when testing
		options="-Wall -shared"
		export CFLAGS=""
	else
		export CFLAGS="-O2 -pipe"
	fi
	# shellcheck disable=SC2046
	gcc $options -o bin/libcpu_mode.so -fPIC cpu_mode.c cpu_mode_config_dialog.c $(pkg-config --cflags --libs libxfce4panel-2.0) $(pkg-config --cflags --libs gtk+-3.0) $(pkg-config --cflags --libs libxfce4ui-2)
	return $?
}

function create_svgs() {
	for img in img/*.svg; do
		echo ""
	done

	i=0
	for step in "${icon_steps[@]}"; do
		new="$tmpDir/cpu_mode_$step.svg"
		height=$((445 * $step / 100))
		y=$((33 + 455 * (100 - $step) / 100))
		fill="${icon_fills[$i]}"

		cp img/cpu_mode.svg.tpl "$new"
		sed -i "s/__H__/$height/g" "$new"
		sed -i "s/__Y__/$y/g" "$new"
		sed -i "s/__F__/$fill/g" "$new"

		i=$((i+1))
	done

	return $?
}

function create_pixmaps() {
	for img in "$tmpDir"/*.svg; do
		for step in "${icon_sizes[@]}"; do
			targetDir="bin/icons/hicolor/${step}x${step}/apps"
			iconName=$(basename "${img%.*}")

			mkdir -p "$targetDir"
			cp "$img" "$targetDir/$iconName.svg"
			sed -i "s/__HEIGHT__/$step/g" "$targetDir/$iconName.svg"
			sed -i "s/__WIDTH__/$step/g" "$targetDir/$iconName.svg"
			convert -background none "$targetDir/$iconName.svg" -resize "${step}x${step}" "$targetDir/$iconName.png" &
		done

		# scalable svg version
		targetDir="bin/icons/hicolor/scalable/apps"
		mkdir -p "$targetDir"
		cp "$img" "$targetDir/$iconName.svg" &
	done

	wait
	# delete "resized" svg versions
	for img in "$tmpDir"/*.svg; do
		for step in "${icon_sizes[@]}"; do
			targetDir="bin/icons/hicolor/${step}x${step}/apps"
			iconName=$(basename "${img%.*}")

			rm "$targetDir/$iconName.svg"
		done
	done

	return $?
}

function create_icon_list() {
	echo "#!/bin/bash" > util/uninstall_icons.sh
	find bin/icons/ -type f -print | sed 's,bin/icons/,rm /usr/share/icons/,' >> util/uninstall_icons.sh
	chmod +x util/uninstall_icons.sh
}

function create_icons() {
	create_svgs && create_pixmaps && create_icon_list
	return $?
}



### Main

check_prerequisites && \
create_icons && \
compile
