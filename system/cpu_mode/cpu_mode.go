package main

import (
	"bytes"
	"fmt"
	"os"
	"slices"
	"strconv"
	"strings"
)

var governors = []string{
	"powersave",
	"schedutil",
	"performance",
}

func main() {
	if len(os.Args) == 1 {
		switchGovernor(governors)
	} else {
		// user the arguments as governor list
		switchGovernor(os.Args[1:])
	}

	fmt.Print(readGovernor() + "\n")
}

func switchGovernor(governors []string) {
	gov := readGovernor()

	// fmt.Printf("Cur: %s\n", gov)
	// If current in list, take next, else take first one
	newGov := governors[0]
	i := slices.Index[[]string](governors, gov)
	// fmt.Printf("I: %d\n", i)
	if i != -1 && i < len(governors)-1 {
		newGov = governors[i+1]
		// fmt.Printf("New: %s\n", newGov)
	}

	writeGovernor(newGov)
}

func readGovernor() string {
	data, err := os.ReadFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading CPU governor: %s\n", err.Error())
		return ""
	}

	gov, _ := bytes.CutSuffix(data, []byte("\n"))

	return string(gov)
}

func writeGovernor(governor string) {
	entries, err := os.ReadDir("/sys/devices/system/cpu/")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading CPU files in /sys/devices/system/cpu/: %s\n", err.Error())
		return
	}

	for _, entry := range entries {
		suffix, ok := strings.CutPrefix(entry.Name(), "cpu")
		if !ok {
			continue
		}
		_, err := strconv.ParseUint(suffix, 10, 64)
		if err != nil {
			continue
		}
		file := fmt.Sprintf("/sys/devices/system/cpu/%s/cpufreq/scaling_governor", entry.Name())

		err = os.WriteFile(file, []byte(governor+"\n"), 0644)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing CPU files in /sys/devices/system/cpu/: %s\n", err.Error())
			return
		}
	}
}

// TODO: set frequency (echo 3000000 > /sys/devices/system/cpu/cpu{0..9}*/cpufreq/scaling_max_freq)
