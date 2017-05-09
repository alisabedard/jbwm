button_event.o: button_event.c button_event.h client.h drag.h log.h \
 title_bar.h wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h
client.o: client.c client.h ewmh.h ewmh_state.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h JBWMSize.h select.h title_bar.h wm_state.h
display.o: display.c display.h client.h config.h log.h util.h pixel_t.h
drag.o: drag.c drag.h font.h move_resize.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h JBWMSize.h snap.h
events.o: events.c events.h button_event.h client.h ewmh.h ewmh_client.h \
 key_event.h log.h move_resize.h new.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h \
 JBWMPixels.h pixel_t.h JBWMSize.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h client.h config.h font.h log.h macros.h util.h
ewmh_client.o: ewmh_client.c ewmh_client.h client.h config.h drag.h \
 ewmh.h ewmh_wm_state.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h select.h util.h wm_state.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h drag.h ewmh.h jbwm.h \
 log.h max.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.c ewmh_wm_state.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h client.h ewmh.h \
 ewmh_state.h log.h max.h util.h pixel_t.h
font.o: font.c font.h config.h util.h pixel_t.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h \
 config.h ewmh.h key_masks.h keys.h log.h new.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h util.h
key_event.o: key_event.c key_event.h JBWMKeys.h JBWMScreen.h JBWMPixels.h \
 pixel_t.h JBWMSize.h client.h config.h drag.h key_masks.h keys.h log.h \
 max.h move_resize.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h select.h signal.h snap.h \
 title_bar.h wm_state.h
key_masks.o: key_masks.c key_masks.h config.h
keys.o: keys.c keys.h JBWMKeys.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h key_masks.h util.h
main.o: main.c JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h events.h \
 display.h font.h jbwm.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h
max.o: max.c max.h ewmh.h ewmh_state.h font.h move_resize.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h title_bar.h
move_resize.o: move_resize.c move_resize.h JBWMClient.h \
 JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h font.h log.h \
 mwm.h shape.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h \
 JBWMRectangle.h log.h mwmproto.h util.h pixel_t.h
new.o: new.c new.h client.h config.h ewmh.h keys.h macros.h mwm.h \
 screen.h JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h \
 JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h select.h \
 shape.h util.h
screen.o: screen.c screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h client.h config.h ewmh.h log.h util.h
select.o: select.c select.h client.h ewmh.h ewmh_state.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h log.h
signal.o: signal.c signal.h
snap.o: snap.c snap.h JBWMPoint.h client.h config.h font.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMClientTitlebar.h JBWMRectangle.h \
 JBWMScreen.h JBWMPixels.h pixel_t.h JBWMSize.h
title_bar.o: title_bar.c title_bar.h config.h ewmh.h ewmh_state.h font.h \
 move_resize.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h JBWMScreen.h JBWMPixels.h pixel_t.h \
 JBWMSize.h util.h wm_state.h
util.o: util.c util.h pixel_t.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMClientTitlebar.h JBWMRectangle.h ewmh.h ewmh_state.h log.h util.h \
 pixel_t.h
