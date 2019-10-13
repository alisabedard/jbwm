button_event.o: button_event.c button_event.h client.h drag.h log.h \
 title_bar.h wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h
client.o: client.c client.h JBWMAtomIndex.h ewmh.h ewmh_state.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h select.h title_bar.h vdesk.h \
 wm_state.h
command_line.o: command_line.c command_line.h config.h key_masks.h log.h
display.o: display.c display.h client.h config.h log.h util.h pixel_t.h
drag.o: drag.c drag.h font.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h snap.h
events.o: events.c events.h button_event.h client.h ewmh.h ewmh_client.h \
 key_event.h log.h move_resize.h new.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 client.h config.h font.h geometry.h JBWMSize.h log.h macros.h \
 PropertyData.h JBWMAtomIndex.h util.h ewmh_init.c ewmh_allowed.c
ewmh_allowed.o: ewmh_allowed.c
ewmh_client.o: ewmh_client.c ewmh_client.h JBWMAtomIndex.h client.h \
 config.h drag.h ewmh.h ewmh_wm_state.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h select.h util.h vdesk.h wm_state.h
ewmh_init.o: ewmh_init.c
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMAtomIndex.h client.h drag.h \
 ewmh.h jbwm.h log.h max.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.c ewmh_wm_state.h JBWMAtomIndex.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h \
 client.h ewmh.h ewmh_state.h log.h max.h util.h pixel_t.h
exec.o: exec.c exec.h config.h
font.o: font.c font.h config.h util.h pixel_t.h
geometry.o: geometry.c geometry.h JBWMSize.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h log.h macros.h screen.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMPixels.h pixel_t.h config.h ewmh.h \
 keys.h log.h new.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h util.h
key_event.o: key_event.c key_event.h JBWMKeys.h key_combos.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h client.h drag.h exec.h config.h keys.h \
 key_masks.h log.h max.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h select.h snap.h \
 title_bar.h vdesk.h wm_state.h
key_masks.o: key_masks.c key_masks.h config.h
keys.o: keys.c keys.h JBWMKeys.h key_combos.h key_masks.h
main.o: main.c command_line.h display.h events.h font.h jbwm.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h
max.o: max.c max.h JBWMAtomIndex.h ewmh.h ewmh_state.h font.h \
 move_resize.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 title_bar.h
move_resize.o: move_resize.c move_resize.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h font.h log.h \
 mwm.h shape.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h \
 JBWMRectangle.h log.h mwmproto.h util.h pixel_t.h
new.o: new.c new.h JBWMAtomIndex.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h client.h config.h ewmh.h geometry.h \
 JBWMSize.h key_masks.h log.h mwm.h screen.h JBWMScreen.h JBWMPixels.h \
 pixel_t.h select.h shape.h util.h
screen.o: screen.c screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h
select.o: select.c select.h JBWMAtomIndex.h client.h ewmh.h ewmh_state.h \
 screen.h JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h \
 JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h log.h
snap.o: snap.c snap.h JBWMPoint.h JBWMSize.h client.h config.h font.h \
 geometry.h log.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h
title_bar.o: title_bar.c title_bar.h JBWMAtomIndex.h config.h ewmh.h \
 ewmh_state.h font.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h util.h wm_state.h
util.o: util.c util.h pixel_t.h
vdesk.o: vdesk.c vdesk.h JBWMAtomIndex.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 client.h config.h ewmh.h util.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitleBar.h JBWMRectangle.h JBWMAtomIndex.h ewmh.h ewmh_state.h \
 log.h util.h pixel_t.h
# DO NOT DELETE

button_event.o: button_event.h /usr/include/X11/Xlib.h
button_event.o: /usr/include/sys/types.h /usr/include/features.h
button_event.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
button_event.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
button_event.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
button_event.o: /usr/include/time.h /usr/include/bits/pthreadtypes.h
button_event.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
button_event.o: /usr/include/X11/Xosdefs.h client.h /usr/include/stdint.h
button_event.o: /usr/include/bits/wchar.h drag.h log.h title_bar.h wm_state.h
button_event.o: JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h
button_event.o: JBWMRectangle.h
client.o: client.h /usr/include/stdint.h /usr/include/features.h
client.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
client.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
client.o: /usr/include/bits/wchar.h /usr/include/X11/Xlib.h
client.o: /usr/include/sys/types.h /usr/include/bits/types.h
client.o: /usr/include/bits/typesizes.h /usr/include/time.h
client.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
client.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
client.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
client.o: /usr/include/X11/keysymdef.h /usr/include/stdlib.h
client.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
client.o: /usr/include/bits/stdlib-float.h JBWMAtomIndex.h ewmh.h
client.o: ewmh_state.h screen.h JBWMClient.h JBWMClientOptions.h
client.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
client.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
client.o: /usr/include/freetype2/freetype/config/ftheader.h
client.o: /usr/include/freetype2/freetype/freetype.h
client.o: /usr/include/freetype2/freetype/config/ftconfig.h
client.o: /usr/include/freetype2/freetype/config/ftoption.h
client.o: /usr/include/freetype2/freetype/config/ftstdlib.h
client.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
client.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
client.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
client.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
client.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
client.o: /usr/include/_G_config.h /usr/include/wchar.h
client.o: /usr/include/bits/sys_errlist.h /usr/include/setjmp.h
client.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
client.o: /usr/include/freetype2/freetype/fttypes.h
client.o: /usr/include/freetype2/freetype/config/ftconfig.h
client.o: /usr/include/freetype2/freetype/ftsystem.h
client.o: /usr/include/freetype2/freetype/ftimage.h
client.o: /usr/include/freetype2/freetype/fterrors.h
client.o: /usr/include/freetype2/freetype/ftmoderr.h
client.o: /usr/include/freetype2/freetype/fterrdef.h
client.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
client.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
client.o: /usr/include/X11/extensions/render.h /usr/include/X11/Xdefs.h
client.o: /usr/include/X11/Xft/XftCompat.h JBWMPixels.h pixel_t.h select.h
client.o: title_bar.h vdesk.h wm_state.h
command_line.o: command_line.h /usr/include/stdio.h /usr/include/features.h
command_line.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
command_line.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
command_line.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
command_line.o: /usr/include/libio.h /usr/include/_G_config.h
command_line.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
command_line.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
command_line.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
command_line.o: /usr/include/sys/types.h /usr/include/time.h
command_line.o: /usr/include/bits/pthreadtypes.h
command_line.o: /usr/include/bits/stdlib-float.h /usr/include/string.h
command_line.o: /usr/include/xlocale.h /usr/include/unistd.h
command_line.o: /usr/include/bits/posix_opt.h
command_line.o: /usr/include/bits/environments.h /usr/include/bits/confname.h
command_line.o: /usr/include/getopt.h config.h /usr/include/X11/X.h
command_line.o: key_masks.h /usr/include/stdint.h /usr/include/bits/wchar.h
command_line.o: log.h
display.o: display.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
display.o: /usr/include/features.h /usr/include/stdc-predef.h
display.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
display.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
display.o: /usr/include/bits/typesizes.h /usr/include/time.h
display.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
display.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
display.o: /usr/include/X11/Xproto.h /usr/include/X11/Xmd.h
display.o: /usr/include/X11/Xprotostr.h /usr/include/errno.h
display.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
display.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
display.o: /usr/include/asm-generic/errno-base.h /usr/include/stdio.h
display.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
display.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
display.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
display.o: /usr/include/bits/waitstatus.h /usr/include/bits/stdlib-float.h
display.o: client.h /usr/include/stdint.h /usr/include/bits/wchar.h config.h
display.o: log.h util.h pixel_t.h
drag.o: drag.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
drag.o: /usr/include/features.h /usr/include/stdc-predef.h
drag.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
drag.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
drag.o: /usr/include/bits/typesizes.h /usr/include/time.h
drag.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
drag.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
drag.o: /usr/include/X11/cursorfont.h /usr/include/stdlib.h
drag.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
drag.o: /usr/include/bits/stdlib-float.h font.h /usr/include/stdint.h
drag.o: /usr/include/bits/wchar.h move_resize.h screen.h JBWMClient.h
drag.o: JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
drag.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
drag.o: /usr/include/freetype2/freetype/config/ftheader.h
drag.o: /usr/include/freetype2/freetype/freetype.h
drag.o: /usr/include/freetype2/freetype/config/ftconfig.h
drag.o: /usr/include/freetype2/freetype/config/ftoption.h
drag.o: /usr/include/freetype2/freetype/config/ftstdlib.h
drag.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
drag.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
drag.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
drag.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
drag.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
drag.o: /usr/include/_G_config.h /usr/include/wchar.h
drag.o: /usr/include/bits/sys_errlist.h /usr/include/setjmp.h
drag.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
drag.o: /usr/include/freetype2/freetype/fttypes.h
drag.o: /usr/include/freetype2/freetype/config/ftconfig.h
drag.o: /usr/include/freetype2/freetype/ftsystem.h
drag.o: /usr/include/freetype2/freetype/ftimage.h
drag.o: /usr/include/freetype2/freetype/fterrors.h
drag.o: /usr/include/freetype2/freetype/ftmoderr.h
drag.o: /usr/include/freetype2/freetype/fterrdef.h
drag.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
drag.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
drag.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
drag.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
drag.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
drag.o: JBWMPixels.h pixel_t.h snap.h
events.o: events.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
events.o: /usr/include/features.h /usr/include/stdc-predef.h
events.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
events.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
events.o: /usr/include/bits/typesizes.h /usr/include/time.h
events.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
events.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
events.o: /usr/include/X11/Xatom.h button_event.h client.h
events.o: /usr/include/stdint.h /usr/include/bits/wchar.h ewmh.h
events.o: ewmh_client.h key_event.h log.h move_resize.h new.h screen.h
events.o: JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h
events.o: JBWMRectangle.h JBWMScreen.h /usr/include/X11/Xft/Xft.h
events.o: /usr/include/freetype2/ft2build.h
events.o: /usr/include/freetype2/freetype/config/ftheader.h
events.o: /usr/include/freetype2/freetype/freetype.h
events.o: /usr/include/freetype2/freetype/config/ftconfig.h
events.o: /usr/include/freetype2/freetype/config/ftoption.h
events.o: /usr/include/freetype2/freetype/config/ftstdlib.h
events.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
events.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
events.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
events.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
events.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
events.o: /usr/include/_G_config.h /usr/include/wchar.h
events.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
events.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
events.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
events.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
events.o: /usr/include/freetype2/freetype/fttypes.h
events.o: /usr/include/freetype2/freetype/config/ftconfig.h
events.o: /usr/include/freetype2/freetype/ftsystem.h
events.o: /usr/include/freetype2/freetype/ftimage.h
events.o: /usr/include/freetype2/freetype/fterrors.h
events.o: /usr/include/freetype2/freetype/ftmoderr.h
events.o: /usr/include/freetype2/freetype/fterrdef.h
events.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
events.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
events.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
events.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
events.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
events.o: JBWMPixels.h pixel_t.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
ewmh.o: /usr/include/features.h /usr/include/stdc-predef.h
ewmh.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
ewmh.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
ewmh.o: /usr/include/bits/typesizes.h /usr/include/time.h
ewmh.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
ewmh.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
ewmh.o: /usr/include/stdint.h /usr/include/bits/wchar.h /usr/include/unistd.h
ewmh.o: /usr/include/bits/posix_opt.h /usr/include/bits/environments.h
ewmh.o: /usr/include/bits/confname.h /usr/include/getopt.h
ewmh.o: /usr/include/X11/Xatom.h JBWMClient.h JBWMClientOptions.h
ewmh.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
ewmh.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
ewmh.o: /usr/include/freetype2/freetype/config/ftheader.h
ewmh.o: /usr/include/freetype2/freetype/freetype.h
ewmh.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh.o: /usr/include/freetype2/freetype/config/ftoption.h
ewmh.o: /usr/include/freetype2/freetype/config/ftstdlib.h
ewmh.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
ewmh.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
ewmh.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
ewmh.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
ewmh.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
ewmh.o: /usr/include/_G_config.h /usr/include/wchar.h
ewmh.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
ewmh.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
ewmh.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
ewmh.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
ewmh.o: /usr/include/freetype2/freetype/fttypes.h
ewmh.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh.o: /usr/include/freetype2/freetype/ftsystem.h
ewmh.o: /usr/include/freetype2/freetype/ftimage.h
ewmh.o: /usr/include/freetype2/freetype/fterrors.h
ewmh.o: /usr/include/freetype2/freetype/ftmoderr.h
ewmh.o: /usr/include/freetype2/freetype/fterrdef.h
ewmh.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
ewmh.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
ewmh.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
ewmh.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
ewmh.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
ewmh.o: JBWMPixels.h pixel_t.h client.h config.h font.h geometry.h JBWMSize.h
ewmh.o: log.h macros.h PropertyData.h JBWMAtomIndex.h util.h ewmh_init.c
ewmh.o: ewmh_allowed.c
ewmh_client.o: ewmh_client.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
ewmh_client.o: /usr/include/features.h /usr/include/stdc-predef.h
ewmh_client.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
ewmh_client.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
ewmh_client.o: /usr/include/bits/typesizes.h /usr/include/time.h
ewmh_client.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
ewmh_client.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
ewmh_client.o: JBWMAtomIndex.h client.h /usr/include/stdint.h
ewmh_client.o: /usr/include/bits/wchar.h config.h drag.h ewmh.h
ewmh_client.o: ewmh_wm_state.h screen.h JBWMClient.h JBWMClientOptions.h
ewmh_client.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
ewmh_client.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
ewmh_client.o: /usr/include/freetype2/freetype/config/ftheader.h
ewmh_client.o: /usr/include/freetype2/freetype/freetype.h
ewmh_client.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh_client.o: /usr/include/freetype2/freetype/config/ftoption.h
ewmh_client.o: /usr/include/freetype2/freetype/config/ftstdlib.h
ewmh_client.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
ewmh_client.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
ewmh_client.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
ewmh_client.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
ewmh_client.o: /usr/include/xlocale.h /usr/include/stdio.h
ewmh_client.o: /usr/include/libio.h /usr/include/_G_config.h
ewmh_client.o: /usr/include/wchar.h /usr/include/bits/sys_errlist.h
ewmh_client.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
ewmh_client.o: /usr/include/bits/waitstatus.h
ewmh_client.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
ewmh_client.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
ewmh_client.o: /usr/include/freetype2/freetype/fttypes.h
ewmh_client.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh_client.o: /usr/include/freetype2/freetype/ftsystem.h
ewmh_client.o: /usr/include/freetype2/freetype/ftimage.h
ewmh_client.o: /usr/include/freetype2/freetype/fterrors.h
ewmh_client.o: /usr/include/freetype2/freetype/ftmoderr.h
ewmh_client.o: /usr/include/freetype2/freetype/fterrdef.h
ewmh_client.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
ewmh_client.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
ewmh_client.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
ewmh_client.o: /usr/include/X11/keysymdef.h
ewmh_client.o: /usr/include/X11/extensions/render.h /usr/include/X11/Xdefs.h
ewmh_client.o: /usr/include/X11/Xft/XftCompat.h JBWMPixels.h pixel_t.h
ewmh_client.o: select.h util.h vdesk.h wm_state.h
ewmh_state.o: ewmh_state.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
ewmh_state.o: /usr/include/features.h /usr/include/stdc-predef.h
ewmh_state.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
ewmh_state.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
ewmh_state.o: /usr/include/bits/typesizes.h /usr/include/time.h
ewmh_state.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
ewmh_state.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
ewmh_state.o: /usr/include/X11/Xatom.h JBWMAtomIndex.h client.h
ewmh_state.o: /usr/include/stdint.h /usr/include/bits/wchar.h drag.h ewmh.h
ewmh_state.o: jbwm.h log.h max.h screen.h JBWMClient.h JBWMClientOptions.h
ewmh_state.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
ewmh_state.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
ewmh_state.o: /usr/include/freetype2/freetype/config/ftheader.h
ewmh_state.o: /usr/include/freetype2/freetype/freetype.h
ewmh_state.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh_state.o: /usr/include/freetype2/freetype/config/ftoption.h
ewmh_state.o: /usr/include/freetype2/freetype/config/ftstdlib.h
ewmh_state.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
ewmh_state.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
ewmh_state.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
ewmh_state.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
ewmh_state.o: /usr/include/xlocale.h /usr/include/stdio.h
ewmh_state.o: /usr/include/libio.h /usr/include/_G_config.h
ewmh_state.o: /usr/include/wchar.h /usr/include/bits/sys_errlist.h
ewmh_state.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
ewmh_state.o: /usr/include/bits/waitstatus.h /usr/include/bits/stdlib-float.h
ewmh_state.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h
ewmh_state.o: /usr/include/bits/sigset.h
ewmh_state.o: /usr/include/freetype2/freetype/fttypes.h
ewmh_state.o: /usr/include/freetype2/freetype/config/ftconfig.h
ewmh_state.o: /usr/include/freetype2/freetype/ftsystem.h
ewmh_state.o: /usr/include/freetype2/freetype/ftimage.h
ewmh_state.o: /usr/include/freetype2/freetype/fterrors.h
ewmh_state.o: /usr/include/freetype2/freetype/ftmoderr.h
ewmh_state.o: /usr/include/freetype2/freetype/fterrdef.h
ewmh_state.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
ewmh_state.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
ewmh_state.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
ewmh_state.o: /usr/include/X11/keysymdef.h
ewmh_state.o: /usr/include/X11/extensions/render.h /usr/include/X11/Xdefs.h
ewmh_state.o: /usr/include/X11/Xft/XftCompat.h JBWMPixels.h pixel_t.h
ewmh_state.o: select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.h /usr/include/X11/Xlib.h
ewmh_wm_state.o: /usr/include/sys/types.h /usr/include/features.h
ewmh_wm_state.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
ewmh_wm_state.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
ewmh_wm_state.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
ewmh_wm_state.o: /usr/include/time.h /usr/include/bits/pthreadtypes.h
ewmh_wm_state.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
ewmh_wm_state.o: /usr/include/X11/Xosdefs.h JBWMAtomIndex.h JBWMClient.h
ewmh_wm_state.o: JBWMClientOptions.h /usr/include/stdint.h
ewmh_wm_state.o: /usr/include/bits/wchar.h JBWMClientTitleBar.h
ewmh_wm_state.o: JBWMRectangle.h client.h ewmh.h ewmh_state.h log.h max.h
ewmh_wm_state.o: util.h pixel_t.h
exec.o: exec.h /usr/include/stdlib.h /usr/include/features.h
exec.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
exec.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
exec.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
exec.o: /usr/include/sys/types.h /usr/include/bits/types.h
exec.o: /usr/include/bits/typesizes.h /usr/include/time.h
exec.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/stdlib-float.h
exec.o: config.h /usr/include/X11/X.h /usr/include/signal.h
exec.o: /usr/include/bits/sigset.h /usr/include/bits/signum.h
exec.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
exec.o: /usr/include/bits/sigstack.h /usr/include/sys/ucontext.h
exec.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigthread.h
exec.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
exec.o: /usr/include/bits/environments.h /usr/include/bits/confname.h
exec.o: /usr/include/getopt.h
font.o: font.h /usr/include/stdint.h /usr/include/features.h
font.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
font.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
font.o: /usr/include/bits/wchar.h /usr/include/X11/Xlib.h
font.o: /usr/include/sys/types.h /usr/include/bits/types.h
font.o: /usr/include/bits/typesizes.h /usr/include/time.h
font.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
font.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
font.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
font.o: /usr/include/freetype2/freetype/config/ftheader.h
font.o: /usr/include/freetype2/freetype/freetype.h
font.o: /usr/include/freetype2/freetype/config/ftconfig.h
font.o: /usr/include/freetype2/freetype/fttypes.h
font.o: /usr/include/freetype2/freetype/ftsystem.h
font.o: /usr/include/freetype2/freetype/ftimage.h
font.o: /usr/include/freetype2/freetype/fterrors.h
font.o: /usr/include/freetype2/freetype/ftmoderr.h
font.o: /usr/include/freetype2/freetype/fterrdef.h
font.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
font.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
font.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
font.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
font.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h config.h
font.o: util.h pixel_t.h
geometry.o: geometry.h /usr/include/stdint.h /usr/include/features.h
geometry.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
geometry.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
geometry.o: /usr/include/bits/wchar.h /usr/include/X11/Xlib.h
geometry.o: /usr/include/sys/types.h /usr/include/bits/types.h
geometry.o: /usr/include/bits/typesizes.h /usr/include/time.h
geometry.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
geometry.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
geometry.o: JBWMSize.h JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h
geometry.o: JBWMRectangle.h JBWMScreen.h /usr/include/X11/Xft/Xft.h
geometry.o: /usr/include/freetype2/ft2build.h
geometry.o: /usr/include/freetype2/freetype/config/ftheader.h
geometry.o: /usr/include/freetype2/freetype/freetype.h
geometry.o: /usr/include/freetype2/freetype/config/ftconfig.h
geometry.o: /usr/include/freetype2/freetype/config/ftoption.h
geometry.o: /usr/include/freetype2/freetype/config/ftstdlib.h
geometry.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
geometry.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
geometry.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
geometry.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
geometry.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
geometry.o: /usr/include/_G_config.h /usr/include/wchar.h
geometry.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
geometry.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
geometry.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
geometry.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
geometry.o: /usr/include/freetype2/freetype/fttypes.h
geometry.o: /usr/include/freetype2/freetype/config/ftconfig.h
geometry.o: /usr/include/freetype2/freetype/ftsystem.h
geometry.o: /usr/include/freetype2/freetype/ftimage.h
geometry.o: /usr/include/freetype2/freetype/fterrors.h
geometry.o: /usr/include/freetype2/freetype/ftmoderr.h
geometry.o: /usr/include/freetype2/freetype/fterrdef.h
geometry.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
geometry.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
geometry.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
geometry.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
geometry.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
geometry.o: JBWMPixels.h pixel_t.h log.h macros.h screen.h
jbwm.o: jbwm.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
jbwm.o: /usr/include/features.h /usr/include/stdc-predef.h
jbwm.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
jbwm.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
jbwm.o: /usr/include/bits/typesizes.h /usr/include/time.h
jbwm.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
jbwm.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h JBWMScreen.h
jbwm.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
jbwm.o: /usr/include/freetype2/freetype/config/ftheader.h
jbwm.o: /usr/include/freetype2/freetype/freetype.h
jbwm.o: /usr/include/freetype2/freetype/config/ftconfig.h
jbwm.o: /usr/include/freetype2/freetype/config/ftoption.h
jbwm.o: /usr/include/freetype2/freetype/config/ftstdlib.h
jbwm.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
jbwm.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
jbwm.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
jbwm.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
jbwm.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
jbwm.o: /usr/include/_G_config.h /usr/include/wchar.h
jbwm.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
jbwm.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
jbwm.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
jbwm.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
jbwm.o: /usr/include/freetype2/freetype/fttypes.h
jbwm.o: /usr/include/freetype2/freetype/config/ftconfig.h
jbwm.o: /usr/include/freetype2/freetype/ftsystem.h
jbwm.o: /usr/include/freetype2/freetype/ftimage.h
jbwm.o: /usr/include/freetype2/freetype/fterrors.h
jbwm.o: /usr/include/freetype2/freetype/ftmoderr.h
jbwm.o: /usr/include/freetype2/freetype/fterrdef.h
jbwm.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
jbwm.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
jbwm.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
jbwm.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
jbwm.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
jbwm.o: JBWMPixels.h pixel_t.h /usr/include/stdint.h
jbwm.o: /usr/include/bits/wchar.h config.h ewmh.h keys.h log.h new.h screen.h
jbwm.o: JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h
jbwm.o: util.h
key_event.o: key_event.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
key_event.o: /usr/include/features.h /usr/include/stdc-predef.h
key_event.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
key_event.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
key_event.o: /usr/include/bits/typesizes.h /usr/include/time.h
key_event.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
key_event.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
key_event.o: JBWMKeys.h /usr/include/X11/keysym.h
key_event.o: /usr/include/X11/keysymdef.h key_combos.h JBWMScreen.h
key_event.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
key_event.o: /usr/include/freetype2/freetype/config/ftheader.h
key_event.o: /usr/include/freetype2/freetype/freetype.h
key_event.o: /usr/include/freetype2/freetype/config/ftconfig.h
key_event.o: /usr/include/freetype2/freetype/config/ftoption.h
key_event.o: /usr/include/freetype2/freetype/config/ftstdlib.h
key_event.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
key_event.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
key_event.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
key_event.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
key_event.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
key_event.o: /usr/include/_G_config.h /usr/include/wchar.h
key_event.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
key_event.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
key_event.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
key_event.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
key_event.o: /usr/include/freetype2/freetype/fttypes.h
key_event.o: /usr/include/freetype2/freetype/config/ftconfig.h
key_event.o: /usr/include/freetype2/freetype/ftsystem.h
key_event.o: /usr/include/freetype2/freetype/ftimage.h
key_event.o: /usr/include/freetype2/freetype/fterrors.h
key_event.o: /usr/include/freetype2/freetype/ftmoderr.h
key_event.o: /usr/include/freetype2/freetype/fterrdef.h
key_event.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
key_event.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
key_event.o: /usr/include/X11/Xutil.h /usr/include/X11/extensions/render.h
key_event.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
key_event.o: JBWMPixels.h pixel_t.h /usr/include/stdint.h
key_event.o: /usr/include/bits/wchar.h client.h drag.h exec.h config.h keys.h
key_event.o: key_masks.h log.h max.h move_resize.h screen.h JBWMClient.h
key_event.o: JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h
key_event.o: select.h snap.h title_bar.h vdesk.h wm_state.h
key_masks.o: key_masks.h /usr/include/stdint.h /usr/include/features.h
key_masks.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
key_masks.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
key_masks.o: /usr/include/bits/wchar.h config.h /usr/include/X11/X.h
keys.o: keys.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
keys.o: /usr/include/features.h /usr/include/stdc-predef.h
keys.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
keys.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
keys.o: /usr/include/bits/typesizes.h /usr/include/time.h
keys.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
keys.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h JBWMKeys.h
keys.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h key_combos.h
keys.o: key_masks.h /usr/include/stdint.h /usr/include/bits/wchar.h
main.o: command_line.h display.h /usr/include/X11/Xlib.h
main.o: /usr/include/sys/types.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
main.o: /usr/include/time.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
main.o: /usr/include/X11/Xosdefs.h events.h font.h /usr/include/stdint.h
main.o: /usr/include/bits/wchar.h jbwm.h screen.h JBWMClient.h
main.o: JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
main.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
main.o: /usr/include/freetype2/freetype/config/ftheader.h
main.o: /usr/include/freetype2/freetype/freetype.h
main.o: /usr/include/freetype2/freetype/config/ftconfig.h
main.o: /usr/include/freetype2/freetype/config/ftoption.h
main.o: /usr/include/freetype2/freetype/config/ftstdlib.h
main.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
main.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
main.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
main.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
main.o: /usr/include/_G_config.h /usr/include/wchar.h
main.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
main.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
main.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
main.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
main.o: /usr/include/freetype2/freetype/fttypes.h
main.o: /usr/include/freetype2/freetype/config/ftconfig.h
main.o: /usr/include/freetype2/freetype/ftsystem.h
main.o: /usr/include/freetype2/freetype/ftimage.h
main.o: /usr/include/freetype2/freetype/fterrors.h
main.o: /usr/include/freetype2/freetype/ftmoderr.h
main.o: /usr/include/freetype2/freetype/fterrdef.h
main.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
main.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
main.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
main.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
main.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
main.o: JBWMPixels.h pixel_t.h
max.o: max.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
max.o: /usr/include/features.h /usr/include/stdc-predef.h
max.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
max.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
max.o: /usr/include/bits/typesizes.h /usr/include/time.h
max.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
max.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
max.o: JBWMAtomIndex.h ewmh.h /usr/include/stdint.h /usr/include/bits/wchar.h
max.o: ewmh_state.h font.h move_resize.h screen.h JBWMClient.h
max.o: JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
max.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
max.o: /usr/include/freetype2/freetype/config/ftheader.h
max.o: /usr/include/freetype2/freetype/freetype.h
max.o: /usr/include/freetype2/freetype/config/ftconfig.h
max.o: /usr/include/freetype2/freetype/config/ftoption.h
max.o: /usr/include/freetype2/freetype/config/ftstdlib.h
max.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
max.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
max.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
max.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
max.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
max.o: /usr/include/_G_config.h /usr/include/wchar.h
max.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
max.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
max.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
max.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
max.o: /usr/include/freetype2/freetype/fttypes.h
max.o: /usr/include/freetype2/freetype/config/ftconfig.h
max.o: /usr/include/freetype2/freetype/ftsystem.h
max.o: /usr/include/freetype2/freetype/ftimage.h
max.o: /usr/include/freetype2/freetype/fterrors.h
max.o: /usr/include/freetype2/freetype/ftmoderr.h
max.o: /usr/include/freetype2/freetype/fterrdef.h
max.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
max.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
max.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
max.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
max.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h JBWMPixels.h
max.o: pixel_t.h title_bar.h
move_resize.o: move_resize.h JBWMClient.h /usr/include/X11/Xlib.h
move_resize.o: /usr/include/sys/types.h /usr/include/features.h
move_resize.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
move_resize.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
move_resize.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
move_resize.o: /usr/include/time.h /usr/include/bits/pthreadtypes.h
move_resize.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
move_resize.o: /usr/include/X11/Xosdefs.h JBWMClientOptions.h
move_resize.o: /usr/include/stdint.h /usr/include/bits/wchar.h
move_resize.o: JBWMClientTitleBar.h JBWMRectangle.h font.h log.h mwm.h
move_resize.o: shape.h title_bar.h
mwm.o: mwm.h JBWMClient.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
mwm.o: /usr/include/features.h /usr/include/stdc-predef.h
mwm.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
mwm.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
mwm.o: /usr/include/bits/typesizes.h /usr/include/time.h
mwm.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
mwm.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
mwm.o: JBWMClientOptions.h /usr/include/stdint.h /usr/include/bits/wchar.h
mwm.o: JBWMClientTitleBar.h JBWMRectangle.h log.h mwmproto.h util.h pixel_t.h
new.o: new.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
new.o: /usr/include/features.h /usr/include/stdc-predef.h
new.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
new.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
new.o: /usr/include/bits/typesizes.h /usr/include/time.h
new.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
new.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
new.o: /usr/include/X11/Xatom.h /usr/include/stdlib.h
new.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
new.o: /usr/include/bits/stdlib-float.h JBWMAtomIndex.h JBWMClient.h
new.o: JBWMClientOptions.h /usr/include/stdint.h /usr/include/bits/wchar.h
new.o: JBWMClientTitleBar.h JBWMRectangle.h client.h config.h ewmh.h
new.o: geometry.h JBWMSize.h key_masks.h log.h mwm.h screen.h JBWMScreen.h
new.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
new.o: /usr/include/freetype2/freetype/config/ftheader.h
new.o: /usr/include/freetype2/freetype/freetype.h
new.o: /usr/include/freetype2/freetype/config/ftconfig.h
new.o: /usr/include/freetype2/freetype/config/ftoption.h
new.o: /usr/include/freetype2/freetype/config/ftstdlib.h
new.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
new.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
new.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
new.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
new.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
new.o: /usr/include/_G_config.h /usr/include/wchar.h
new.o: /usr/include/bits/sys_errlist.h /usr/include/setjmp.h
new.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
new.o: /usr/include/freetype2/freetype/fttypes.h
new.o: /usr/include/freetype2/freetype/config/ftconfig.h
new.o: /usr/include/freetype2/freetype/ftsystem.h
new.o: /usr/include/freetype2/freetype/ftimage.h
new.o: /usr/include/freetype2/freetype/fterrors.h
new.o: /usr/include/freetype2/freetype/ftmoderr.h
new.o: /usr/include/freetype2/freetype/fterrdef.h
new.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
new.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
new.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
new.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
new.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h JBWMPixels.h
new.o: pixel_t.h select.h shape.h util.h
screen.o: screen.h JBWMClient.h /usr/include/X11/Xlib.h
screen.o: /usr/include/sys/types.h /usr/include/features.h
screen.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
screen.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
screen.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
screen.o: /usr/include/time.h /usr/include/bits/pthreadtypes.h
screen.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
screen.o: /usr/include/X11/Xosdefs.h JBWMClientOptions.h
screen.o: /usr/include/stdint.h /usr/include/bits/wchar.h
screen.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
screen.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
screen.o: /usr/include/freetype2/freetype/config/ftheader.h
screen.o: /usr/include/freetype2/freetype/freetype.h
screen.o: /usr/include/freetype2/freetype/config/ftconfig.h
screen.o: /usr/include/freetype2/freetype/config/ftoption.h
screen.o: /usr/include/freetype2/freetype/config/ftstdlib.h
screen.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
screen.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
screen.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
screen.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
screen.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
screen.o: /usr/include/_G_config.h /usr/include/wchar.h
screen.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
screen.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
screen.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
screen.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
screen.o: /usr/include/freetype2/freetype/fttypes.h
screen.o: /usr/include/freetype2/freetype/config/ftconfig.h
screen.o: /usr/include/freetype2/freetype/ftsystem.h
screen.o: /usr/include/freetype2/freetype/ftimage.h
screen.o: /usr/include/freetype2/freetype/fterrors.h
screen.o: /usr/include/freetype2/freetype/ftmoderr.h
screen.o: /usr/include/freetype2/freetype/fterrdef.h
screen.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
screen.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
screen.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
screen.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
screen.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
screen.o: JBWMPixels.h pixel_t.h
select.o: select.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
select.o: /usr/include/features.h /usr/include/stdc-predef.h
select.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
select.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
select.o: /usr/include/bits/typesizes.h /usr/include/time.h
select.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
select.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
select.o: /usr/include/X11/Xatom.h JBWMAtomIndex.h client.h
select.o: /usr/include/stdint.h /usr/include/bits/wchar.h ewmh.h ewmh_state.h
select.o: screen.h JBWMClient.h JBWMClientOptions.h JBWMClientTitleBar.h
select.o: JBWMRectangle.h JBWMScreen.h /usr/include/X11/Xft/Xft.h
select.o: /usr/include/freetype2/ft2build.h
select.o: /usr/include/freetype2/freetype/config/ftheader.h
select.o: /usr/include/freetype2/freetype/freetype.h
select.o: /usr/include/freetype2/freetype/config/ftconfig.h
select.o: /usr/include/freetype2/freetype/config/ftoption.h
select.o: /usr/include/freetype2/freetype/config/ftstdlib.h
select.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
select.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
select.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
select.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
select.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
select.o: /usr/include/_G_config.h /usr/include/wchar.h
select.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
select.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
select.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
select.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
select.o: /usr/include/freetype2/freetype/fttypes.h
select.o: /usr/include/freetype2/freetype/config/ftconfig.h
select.o: /usr/include/freetype2/freetype/ftsystem.h
select.o: /usr/include/freetype2/freetype/ftimage.h
select.o: /usr/include/freetype2/freetype/fterrors.h
select.o: /usr/include/freetype2/freetype/ftmoderr.h
select.o: /usr/include/freetype2/freetype/fterrdef.h
select.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
select.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
select.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
select.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
select.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
select.o: JBWMPixels.h pixel_t.h util.h
shape.o: shape.h /usr/include/X11/extensions/shape.h
shape.o: /usr/include/X11/Xfuncproto.h
shape.o: /usr/include/X11/extensions/shapeconst.h /usr/include/X11/Xutil.h
shape.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
shape.o: /usr/include/features.h /usr/include/stdc-predef.h
shape.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
shape.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
shape.o: /usr/include/bits/typesizes.h /usr/include/time.h
shape.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
shape.o: /usr/include/X11/Xosdefs.h /usr/include/X11/keysym.h
shape.o: /usr/include/X11/keysymdef.h JBWMClient.h JBWMClientOptions.h
shape.o: /usr/include/stdint.h /usr/include/bits/wchar.h JBWMClientTitleBar.h
shape.o: JBWMRectangle.h log.h
snap.o: snap.h /usr/include/stdlib.h /usr/include/features.h
snap.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
snap.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
snap.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
snap.o: /usr/include/sys/types.h /usr/include/bits/types.h
snap.o: /usr/include/bits/typesizes.h /usr/include/time.h
snap.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/stdlib-float.h
snap.o: JBWMPoint.h /usr/include/stdint.h /usr/include/bits/wchar.h
snap.o: JBWMSize.h client.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
snap.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h config.h
snap.o: font.h geometry.h log.h screen.h JBWMClient.h JBWMClientOptions.h
snap.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
snap.o: /usr/include/X11/Xft/Xft.h /usr/include/freetype2/ft2build.h
snap.o: /usr/include/freetype2/freetype/config/ftheader.h
snap.o: /usr/include/freetype2/freetype/freetype.h
snap.o: /usr/include/freetype2/freetype/config/ftconfig.h
snap.o: /usr/include/freetype2/freetype/config/ftoption.h
snap.o: /usr/include/freetype2/freetype/config/ftstdlib.h
snap.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
snap.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
snap.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
snap.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
snap.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
snap.o: /usr/include/_G_config.h /usr/include/wchar.h
snap.o: /usr/include/bits/sys_errlist.h /usr/include/setjmp.h
snap.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
snap.o: /usr/include/freetype2/freetype/fttypes.h
snap.o: /usr/include/freetype2/freetype/config/ftconfig.h
snap.o: /usr/include/freetype2/freetype/ftsystem.h
snap.o: /usr/include/freetype2/freetype/ftimage.h
snap.o: /usr/include/freetype2/freetype/fterrors.h
snap.o: /usr/include/freetype2/freetype/ftmoderr.h
snap.o: /usr/include/freetype2/freetype/fterrdef.h
snap.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
snap.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
snap.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
snap.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
snap.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
snap.o: JBWMPixels.h pixel_t.h
title_bar.o: title_bar.h /usr/include/X11/Xatom.h /usr/include/X11/Xft/Xft.h
title_bar.o: /usr/include/freetype2/ft2build.h
title_bar.o: /usr/include/freetype2/freetype/config/ftheader.h
title_bar.o: /usr/include/freetype2/freetype/freetype.h
title_bar.o: /usr/include/freetype2/freetype/config/ftconfig.h
title_bar.o: /usr/include/freetype2/freetype/fttypes.h
title_bar.o: /usr/include/freetype2/freetype/ftsystem.h
title_bar.o: /usr/include/freetype2/freetype/ftimage.h
title_bar.o: /usr/include/freetype2/freetype/fterrors.h
title_bar.o: /usr/include/freetype2/freetype/ftmoderr.h
title_bar.o: /usr/include/freetype2/freetype/fterrdef.h
title_bar.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/types.h
title_bar.o: /usr/include/features.h /usr/include/stdc-predef.h
title_bar.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
title_bar.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
title_bar.o: /usr/include/bits/typesizes.h /usr/include/time.h
title_bar.o: /usr/include/bits/pthreadtypes.h /usr/include/sys/stat.h
title_bar.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
title_bar.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
title_bar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
title_bar.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
title_bar.o: /usr/include/X11/keysymdef.h
title_bar.o: /usr/include/X11/extensions/render.h /usr/include/X11/Xdefs.h
title_bar.o: /usr/include/X11/Xft/XftCompat.h JBWMAtomIndex.h config.h ewmh.h
title_bar.o: /usr/include/stdint.h /usr/include/bits/wchar.h ewmh_state.h
title_bar.o: font.h move_resize.h screen.h JBWMClient.h JBWMClientOptions.h
title_bar.o: JBWMClientTitleBar.h JBWMRectangle.h JBWMScreen.h
title_bar.o: /usr/include/X11/Xft/Xft.h
title_bar.o: /usr/include/freetype2/freetype/freetype.h
title_bar.o: /usr/include/freetype2/freetype/config/ftconfig.h
title_bar.o: /usr/include/freetype2/freetype/config/ftoption.h
title_bar.o: /usr/include/freetype2/freetype/config/ftstdlib.h
title_bar.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
title_bar.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
title_bar.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
title_bar.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
title_bar.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
title_bar.o: /usr/include/_G_config.h /usr/include/wchar.h
title_bar.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
title_bar.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
title_bar.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
title_bar.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
title_bar.o: /usr/include/freetype2/freetype/fttypes.h
title_bar.o: /usr/include/freetype2/freetype/fterrors.h JBWMPixels.h
title_bar.o: pixel_t.h util.h wm_state.h
util.o: util.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
util.o: /usr/include/features.h /usr/include/stdc-predef.h
util.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
util.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
util.o: /usr/include/bits/typesizes.h /usr/include/time.h
util.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
util.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
util.o: /usr/include/stdint.h /usr/include/bits/wchar.h pixel_t.h
util.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
util.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
util.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
util.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
util.o: /usr/include/bits/stdlib-float.h
vdesk.o: vdesk.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
vdesk.o: /usr/include/features.h /usr/include/stdc-predef.h
vdesk.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
vdesk.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vdesk.o: /usr/include/bits/typesizes.h /usr/include/time.h
vdesk.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
vdesk.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vdesk.o: /usr/include/stdint.h /usr/include/bits/wchar.h
vdesk.o: /usr/include/X11/Xatom.h JBWMAtomIndex.h JBWMClient.h
vdesk.o: JBWMClientOptions.h JBWMClientTitleBar.h JBWMRectangle.h
vdesk.o: JBWMScreen.h /usr/include/X11/Xft/Xft.h
vdesk.o: /usr/include/freetype2/ft2build.h
vdesk.o: /usr/include/freetype2/freetype/config/ftheader.h
vdesk.o: /usr/include/freetype2/freetype/freetype.h
vdesk.o: /usr/include/freetype2/freetype/config/ftconfig.h
vdesk.o: /usr/include/freetype2/freetype/config/ftoption.h
vdesk.o: /usr/include/freetype2/freetype/config/ftstdlib.h
vdesk.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
vdesk.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vdesk.o: /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h
vdesk.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
vdesk.o: /usr/include/xlocale.h /usr/include/stdio.h /usr/include/libio.h
vdesk.o: /usr/include/_G_config.h /usr/include/wchar.h
vdesk.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
vdesk.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vdesk.o: /usr/include/bits/stdlib-float.h /usr/include/setjmp.h
vdesk.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
vdesk.o: /usr/include/freetype2/freetype/fttypes.h
vdesk.o: /usr/include/freetype2/freetype/config/ftconfig.h
vdesk.o: /usr/include/freetype2/freetype/ftsystem.h
vdesk.o: /usr/include/freetype2/freetype/ftimage.h
vdesk.o: /usr/include/freetype2/freetype/fterrors.h
vdesk.o: /usr/include/freetype2/freetype/ftmoderr.h
vdesk.o: /usr/include/freetype2/freetype/fterrdef.h
vdesk.o: /usr/include/fontconfig/fontconfig.h /usr/include/sys/stat.h
vdesk.o: /usr/include/bits/stat.h /usr/include/X11/extensions/Xrender.h
vdesk.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
vdesk.o: /usr/include/X11/keysymdef.h /usr/include/X11/extensions/render.h
vdesk.o: /usr/include/X11/Xdefs.h /usr/include/X11/Xft/XftCompat.h
vdesk.o: JBWMPixels.h pixel_t.h client.h config.h ewmh.h util.h
wm_state.o: wm_state.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
wm_state.o: /usr/include/features.h /usr/include/stdc-predef.h
wm_state.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
wm_state.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
wm_state.o: /usr/include/bits/typesizes.h /usr/include/time.h
wm_state.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
wm_state.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
wm_state.o: JBWMClient.h JBWMClientOptions.h /usr/include/stdint.h
wm_state.o: /usr/include/bits/wchar.h JBWMClientTitleBar.h JBWMRectangle.h
wm_state.o: /usr/include/X11/Xatom.h /usr/include/X11/Xutil.h
wm_state.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
wm_state.o: JBWMAtomIndex.h ewmh.h ewmh_state.h log.h util.h pixel_t.h
