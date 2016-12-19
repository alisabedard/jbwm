button_event.o: button_event.c button_event.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h client.h log.h screen.h title_bar.h \
 wm_state.h
client.o: client.c client.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h display.h ewmh.h \
 ewmh_state.h jbwm.h log.h screen.h select.h title_bar.h util.h
display.o: display.c display.h client.h config.h events.h font.h jbwm.h \
 log.h
events.o: events.c events.h JBWMScreen.h JBWMSize.h pixel_t.h \
 button_event.h client.h display.h ewmh_state.h ewmh.h jbwm.h keys.h \
 log.h new.h screen.h select.h title_bar.h util.h wm_state.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h \
 display.h font.h log.h macros.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMScreen.h JBWMSize.h pixel_t.h \
 client.h display.h ewmh.h jbwm.h log.h max.h screen.h select.h util.h \
 wm_state.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h
font.o: font.c font.h config.h jbwm.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h config.h \
 display.h events.h ewmh.h keys.h log.h new.h util.h
keys.o: keys.c keys.h JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h \
 client.h config.h display.h jbwm.h log.h max.h screen.h select.h snap.h \
 title_bar.h util.h wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h
max.o: max.c max.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h display.h ewmh.h ewmh_state.h font.h \
 log.h screen.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h util.h pixel_t.h
new.o: new.c new.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h \
 display.h ewmh.h jbwm.h keys.h log.h macros.h mwm.h screen.h select.h \
 shape.h util.h
screen.o: screen.c screen.h JBWMDefaults.h JBWMScreen.h JBWMSize.h \
 pixel_t.h client.h config.h display.h ewmh_state.h ewmh.h font.h jbwm.h \
 log.h shape.h snap.h title_bar.h util.h wm_state.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h
select.o: select.c select.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h client.h display.h \
 ewmh.h ewmh_state.h screen.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h
snap.o: snap.c snap.h JBWMSize.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMDefaults.h JBWMPoint.h JBWMScreen.h pixel_t.h \
 client.h font.h screen.h
title_bar.o: title_bar.c title_bar.h JBWMScreen.h JBWMSize.h pixel_t.h \
 display.h ewmh.h ewmh_state.h font.h jbwm.h screen.h select.h util.h \
 wm_state.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h
util.o: util.c util.h pixel_t.h jbwm.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h display.h log.h util.h pixel_t.h
