# JBWM
![Screenshot](screenshot.png)

## FEATURES of JBWM
* Virtual desktops with Meta-[1-0]
* 15K binary size (with gcc 5.3.0 on linux/amd64) for minimal configuration
	- Takes only 13kb on a Raspberry PI, minimal configuration
	- Run 'make small'
* Titlebar support if wanted
* Xft support if wanted
* EWMH support if wanted
* Window shading with titlebar
* Code cleanups, simpler UI and key bindings
* Use of C11 features where possible.

## RELEASES
Releases are made when sufficient changes are made to the code base.  Releases
fixing crashes are made as soon as possible--such releases will be as tertiary
point releases, starting at x.x.1, if made soon after minor releases.  Such
releases may be delayed if a new minor release cycle has been started, unless a
bug is reported.  

## KNOWN ISSUES
* More EWMH support needed, particularly to enable pagers and window placement.

## INSTALLATION
Edit configuration options in Makefile and config.h, then build:
```sh
	$ make
	# make install
```

## ACKNOWLEDGEMENTS

evilwm, by Ciaran Anscomb <evilwm@6809.org.uk>.  http://www.6809.org.uk/evilwm/

The original code base for this project was that of aewm by Decklin
Foster.  His code is very clean and, in later versions, very well
commented.  http://www.red-bean.com/~decklin/aewm/

