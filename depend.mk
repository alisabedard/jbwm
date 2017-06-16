button_event.o: button_event.c button_event.h client.h drag.h log.h \
 title_bar.h wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h
client.o: client.c client.h JBWMAtomIndex.h ewmh.h ewmh_state.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h select.h title_bar.h vdesk.h \
 wm_state.h
command_line.o: command_line.c command_line.h config.h key_masks.h log.h
display.o: display.c display.h client.h config.h log.h util.h pixel_t.h
drag.o: drag.c drag.h font.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h snap.h
events.o: events.c events.h button_event.h client.h ewmh.h ewmh_client.h \
 key_event.h log.h move_resize.h new.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMAtomIndex.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 client.h config.h font.h log.h macros.h util.h ewmh_atoms.c
ewmh_atoms.o: ewmh_atoms.c
ewmh_client.o: ewmh_client.c ewmh_client.h JBWMAtomIndex.h client.h \
 config.h drag.h ewmh.h ewmh_wm_state.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h select.h util.h vdesk.h wm_state.h
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMAtomIndex.h client.h drag.h \
 ewmh.h jbwm.h log.h max.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.c ewmh_wm_state.h JBWMAtomIndex.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h \
 client.h ewmh.h ewmh_state.h log.h max.h util.h pixel_t.h
exec.o: exec.c exec.h config.h
font.o: font.c font.h config.h util.h pixel_t.h
geometry.o: geometry.c geometry.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h log.h macros.h screen.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMPixels.h pixel_t.h config.h ewmh.h \
 keys.h log.h new.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h util.h
key_event.o: key_event.c key_event.h JBWMKeys.h JBWMScreen.h JBWMPixels.h \
 pixel_t.h client.h config.h drag.h exec.h key_masks.h keys.h log.h max.h \
 move_resize.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h select.h snap.h title_bar.h vdesk.h \
 wm_state.h
key_masks.o: key_masks.c key_masks.h config.h
keys.o: keys.c keys.h JBWMKeys.h key_masks.h
main.o: main.c JBWMScreen.h JBWMPixels.h pixel_t.h command_line.h \
 display.h events.h font.h jbwm.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h
max.o: max.c max.h JBWMAtomIndex.h ewmh.h ewmh_state.h font.h \
 move_resize.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 title_bar.h
move_resize.o: move_resize.c move_resize.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h font.h log.h \
 mwm.h shape.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h \
 JBWMRectangle.h log.h mwmproto.h util.h pixel_t.h
new.o: new.c new.h JBWMAtomIndex.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h client.h config.h ewmh.h geometry.h \
 key_masks.h log.h mwm.h screen.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 select.h shape.h util.h
screen.o: screen.c screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h
select.o: select.c select.h JBWMAtomIndex.h client.h ewmh.h ewmh_state.h \
 screen.h JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h \
 JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h log.h
snap.o: snap.c snap.h JBWMPoint.h JBWMSize.h client.h config.h font.h \
 screen.h JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h \
 JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h
test.o: test.c
title_bar.o: title_bar.c title_bar.h JBWMAtomIndex.h config.h ewmh.h \
 ewmh_state.h font.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h util.h wm_state.h
util.o: util.c util.h pixel_t.h
vdesk.o: vdesk.c vdesk.h JBWMAtomIndex.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 client.h config.h ewmh.h log.h util.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMAtomIndex.h ewmh.h ewmh_state.h \
 log.h util.h pixel_t.h
