# JBWM

[Screenshot](/images/screensot.png?raw=true "Screenshot")

## FEATURES of JBWM
* 16K binary size (with gcc 5.3.0 on linux/amd64) for minimal configuration
	- Takes only 13kb on a Raspberry PI, minimal configuration
* Titlebar support if wanted
* Xft support if wanted
* Xpm support if wanted
* Window shading with titlebar
* Code cleanups, simpler UI and key bindings
* Use of C99 features where possible.

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

