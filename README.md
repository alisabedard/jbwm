# JBWM
![Screenshot](screenshot.png)

## FEATURES of JBWM
* *Virtual desktops* with Super-{1-0} (Can be changed in config.h)
* Tiny binary size (~35k on x86-64 with -Os)
* *Title bar* support
* *Xft* support if wanted
* *EWMH* support
* *Window shading* with title bar
* Code cleanups, simple UI and key bindings
* Use of C99 features where appropriate.

## DIRECTION
Though jbwm features small size, its main design goal is practical usefulness.
If a change increases binary size but speeds execution, development, or user
interaction, then the change is more than welcome.  If you want to see a feature
or contribute code, let me know.  I use jbwm as my primary and sole window
manager, however I may not see problems that fall outside of my usage patterns,
so bug reports are also much appreciated.

## RELEASES
Releases are made when sufficient changes are made to the code base.  Releases
fixing critical bugs are made as soon as possible.

## INSTALLATION
Edit configuration options in Makefile and config.h, then build:
```sh
	$ ./configure
	$ make
	# make install
```

## REQUIREMENTS
* libX11

## ACKNOWLEDGMENTS

evilwm, by Ciaran Anscomb <evilwm@6809.org.uk>.  http://www.6809.org.uk/evilwm/
The original code base for this project was that of aewm by Decklin
Foster.  His code is very clean and, in later versions, very well
commented.  http://www.red-bean.com/~decklin/aewm/
