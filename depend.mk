button_event.o: button_event.c button_event.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h client.h drag.h log.h title_bar.h \
 wm_state.h
client.o: client.c client.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h display.h events.h ewmh.h ewmh_state.h log.h screen.h \
 JBWMScreen.h JBWMSize.h pixel_t.h jbwm.h select.h title_bar.h
display.o: display.c display.h client.h config.h font.h jbwm.h log.h
drag.o: drag.c drag.h display.h font.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h \
 jbwm.h snap.h
events.o: events.c events.h button_event.h client.h display.h \
 ewmh_state.h ewmh.h keys.h log.h new.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h \
 jbwm.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h display.h font.h \
 log.h macros.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h display.h drag.h ewmh.h \
 jbwm.h log.h max.h screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h select.h util.h \
 wm_state.h
font.o: font.c font.h config.h jbwm.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h config.h \
 display.h ewmh.h events.h keys.h log.h new.h signal.h util.h
keys.o: keys.c keys.h JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h \
 display.h drag.h jbwm.h log.h max.h screen.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h select.h signal.h snap.h title_bar.h \
 util.h wm_state.h
main.o: main.c jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h display.h \
 events.h signal.h
max.o: max.c max.h display.h ewmh.h ewmh_state.h font.h log.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h JBWMScreen.h JBWMSize.h \
 pixel_t.h jbwm.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h mwmproto.h util.h pixel_t.h
new.o: new.c new.h client.h config.h display.h ewmh.h keys.h macros.h \
 mwm.h screen.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h jbwm.h select.h shape.h util.h
screen.o: screen.c screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h jbwm.h client.h \
 config.h display.h ewmh_state.h ewmh.h font.h log.h mwm.h shape.h \
 title_bar.h util.h wm_state.h
select.o: select.c select.h client.h display.h ewmh.h ewmh_state.h \
 screen.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h JBWMScreen.h \
 JBWMSize.h pixel_t.h jbwm.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h
signal.o: signal.c signal.h client.h display.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h log.h title_bar.h
snap.o: snap.c snap.h JBWMPoint.h client.h config.h font.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h JBWMScreen.h JBWMSize.h \
 pixel_t.h jbwm.h
title_bar.o: title_bar.c title_bar.h JBWMScreen.h JBWMSize.h pixel_t.h \
 config.h display.h ewmh.h ewmh_state.h font.h jbwm.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h select.h util.h \
 wm_state.h
util.o: util.c util.h pixel_t.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h display.h ewmh.h ewmh_state.h log.h util.h pixel_t.h
