## Requirements
------------
In order to build dmenu you need the Xlib header files.


## Installation
------------
Edit config.mk to match your local setup (dmenu is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dmenu
(if necessary as root):

```sh
sudo make clean install
```

## Patches
- dmenu-center-4.8.diff
- dmenu-fuzzymatch-4.9.diff
    - this includes **case insensitive**
- dmenu-prefixcompletion-flag-4.9.diff
