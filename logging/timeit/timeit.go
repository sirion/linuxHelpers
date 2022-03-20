package main

import (
	"bytes"
	"errors"
	"fmt"
	"io"
	"os"
	"os/exec"
	"time"
)

func main() {
	command := os.Args[1]
	arguments := os.Args[2:]

	cmd := exec.Command(command, arguments...)

	cmd.Stdin = os.Stdin

	readOut, err := cmd.StdoutPipe()
	if err != nil {
		panic(err)
		os.Exit(999)
	}

	readErr, err := cmd.StderrPipe()
	if err != nil {
		panic(err)
		os.Exit(999)
	}

	doneErr := prefixTime(readErr, os.Stderr)
	doneStd := prefixTime(readOut, os.Stdout)

	err = cmd.Start()
	if err != nil {
		panic(err)
		os.Exit(999)
	}


	<-doneErr
	<-doneStd
	err = cmd.Wait()

	if err != nil {
		exitErr, isExitError := err.(*exec.ExitError)
		if isExitError {
			os.Exit(exitErr.ExitCode())
		} else {
			panic(err)
			os.Exit(999)
		}
	}
}

func prefixTime(r io.ReadCloser, target *os.File) chan bool {
	done := make(chan bool, 1)
	go copyPrefixedTime(r, target, done)
	return done
}

func copyPrefixedTime(r io.ReadCloser, target *os.File, done chan bool) {
	buf := make([]byte, 1024)
	defer r.Close()
	defer target.Sync()

	continued := false

	for {
		read, err := r.Read(buf)

		if errors.Is(err, os.ErrClosed) || err == io.EOF {
			target.Sync()
			done <- true
			return
		} else if err != nil && err != io.EOF {
			target.Sync()
			panic(err)
			done <- true
			os.Exit(999)
		} else if read == 0 {
			time.Sleep(10 * time.Millisecond)
			continue;
		}

		output := buf[0:read]
		var data []byte
		var i int
		for len(output) > 0 {
			data = output

			ts := time.Now().Format("[2006-01-02T15:04:05] ")
			ln := "\n"

			if i = bytes.Index(output, []byte("\n")); i >= 0 {
				data = output[0:i]
				output = output[i + 1:]
			} else {
				data = output
				output = output[0:0]
				ln = ""
			}

			if continued {
				ts = ""
			}

			fmt.Fprintf(target, "%s%s%s", ts, data, ln)

			continued = ln == ""
		}

		if err == io.EOF {
			target.Sync()
			done <- true
			return
		}
	}

}