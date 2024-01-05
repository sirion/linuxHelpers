# Linux Helpers

A bunch of assorted programs and scripts that make my life easier.

## List of available Helpers

### System

* **cpu_mode**
 Change CPU governor for ll cores using the command line. Example: `cpu_mode powersave`. [cpu_mode](system/cpu_mode/README.md) can switch between lists of governors.

### Backup

* **backupDirectory**  
 Rsync wrapper script that creates incremental backups by hard-linking non-changed files to the ones in the previous backup folder.
 (This leads to a backup folder being reported much larger than it actually is in terms of used disk space. The Folder must be copied with `cp -a` or `cp --preserve=links`)
* **backupThis**  
 Simple backup utitly that creates either a 7z archive for single files or a tar archive compressed with 7z for directories in a predefined backup directoy. Also encrypts the backup in case a password file is present in the same directory as the script.

### Photo

* **alignGroups**  
 Aligns Photos taken in groups for HDR processing. Useful if many pictures have been taken using the same AEB setting. Warning: This script assumes that all pictures have been taken with the same AEB group size, are given as arguments in the right order and no pictures have been taken in between groups.
* **applyWatermark**  
 Adds a watermark picture stored as base64 inside the script to the given files.  
 *Warning: With default configuration options this overwrites the original file.*
* **createMultiDesktopWallpaper**  
 Searches a folder for usable pictures (based on a command line argument), resizes and concatenates them to one picture that fits the exact sizes of multiple monitors.
* **enfuseAligneRaw**  
 Creates an aligned and enfused TIFF file from the given arguments.
 Tries to use the additional dynamic range of raw picture files by under- and over-developing the raw files by two stops and combines the results before aligning.
* **enfuseAligneRawGroups**  
 Combines `alignGroups` and `enfuseAligneRaw` to automate the process for pictures taken with AEB.
* **hdr1**  
 Automates the creation of HDR pictures. Aligns and enfuses the given pictures into one.
* **makeIcon**  
 Automatically resizes a picture to the sizes 128x128, 64x64, 48x48 and 32x32 px.
* **rejectRaws**  
 Part of my photo post-processing workflow. After I ordered the pictures with `sortPhotographyOriginals`, I copy the "jpg" folder next to the "original" folder and rename it to "selected", then I delete all pictures inside that I do not want to work on any further. This script moves all files found in the "selected" folder afterwards into a new "selected" sub-directory of the "original" folder and all others in to "rejected".
* **resizeForGallery**  
 Resizes all given images (creating new files in the process) to the configured sizes. I use this for an online gallery in order to prepare the pictures on my local machine which is much faster than the vserver I started with.
* **sortPhotographyOriginals**  
 Sorts the pictures and videos in the given folder and renames the pictures. All files are renamed to only user lower case characters in their file name. Jpeg files and videos are moved into separate sub-directories.
 As last step all pictures are renamed so that the file name consists only of the date and time they were taken.
* **toLower**  
 Simply renames the given files so that only lower case characters are used.

### Logging

* **timeit**  
Enrich program output with timing information.  
Every line of a program's output is prefixed with the current timestamp when started via timeit.  
Usage: `timeit programm [arguments]`

### Misc

* **mipdflatext**  
Simple script that automates creating PDFs from my latex sources.
* **mkpass**  
Creates a random password

### Raspberry Pi

* **createRpiArch**  
 An automated version of the steps needed to create an SD-Card that boots arch linux on a raspberry pi. The original steps can be found at <http://archlinuxarm.org/platforms/armv6/raspberry-pi>  
 This script should to be run in an empty directory as it downloads an archive and creates two folders ("boot" and "root").  
 **Warning:** This script must be run as root makes no additional checks, if you give the wrong device argument to it, it will destroy whatever is on the given disk.

### Template

* **bashScriptTemplate**  
 The standard template I use to create my more complex bash script which makes things like displaying a help message, checking for needed programs and parsing command line options easier.
