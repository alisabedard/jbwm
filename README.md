# JBWM
![Screenshot](screenshot.png)

## FEATURES of JBWM
* *Virtual desktops* with Meta-{1-0}
* Tiny binary size
* *Title bar* support if wanted
* *Xft* support if wanted
* *EWMH* support if wanted
* *Window shading* with title bar
* Code cleanups, simpler UI and key bindings
* Use of C99 features where possible.

## DIRECTION
Though jbwm features small size, its main design goal is practical usefulness.
If a change increases binary size but speeds execution, development, or user
interaction, then the change is more than welcome.  If you want to see a feature
or contribute code, let me know.  I use jbwm as my primary and sole window
manager, however I may not see problems that fall outside of my usage patterns,
so bug reports are also much appreciated.

## RELEASES
Releases are made when sufficient changes are made to the code base.  Releases
fixing crashes are made as soon as possible--such releases will be as tertiary
point releases, starting at x.x.1, if made soon after minor releases.  Such
releases may be delayed if a new minor release cycle has been started, unless a
bug is reported.  

## INSTALLATION
Edit configuration options in Makefile and config.h, then build:
```sh
	$ ./configure
	$ make
	# make install
```

## REQUIREMENTS
* pkgconf
* freetype2
* libX11

## ACKNOWLEDGMENTS

evilwm, by Ciaran Anscomb <evilwm@6809.org.uk>.  http://www.6809.org.uk/evilwm/

The original code base for this project was that of aewm by Decklin
Foster.  His code is very clean and, in later versions, very well
commented.  http://www.red-bean.com/~decklin/aewm/

