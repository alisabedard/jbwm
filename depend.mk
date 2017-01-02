button_event.o: button_event.c button_event.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h client.h drag.h log.h screen.h \
 JBWMScreen.h JBWMSize.h pixel_t.h jbwm.h title_bar.h wm_state.h
client.o: client.c client.h display.h ewmh.h ewmh_state.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h log.h screen.h JBWMScreen.h \
 JBWMSize.h pixel_t.h jbwm.h select.h title_bar.h
display.o: display.c display.h client.h config.h events.h font.h jbwm.h \
 log.h
drag.o: drag.c drag.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h display.h font.h screen.h jbwm.h \
 snap.h
events.o: events.c events.h JBWMScreen.h JBWMSize.h pixel_t.h \
 button_event.h client.h display.h ewmh_state.h ewmh.h jbwm.h keys.h \
 log.h new.h screen.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 select.h title_bar.h util.h wm_state.h
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMScreen.h JBWMSize.h pixel_t.h \
 client.h display.h drag.h ewmh.h jbwm.h log.h max.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h select.h util.h \
 wm_state.h
font.o: font.c font.h config.h jbwm.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h config.h \
 display.h events.h ewmh.h keys.h log.h new.h signal.h util.h
max.o: max.c max.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h display.h ewmh.h ewmh_state.h font.h \
 log.h screen.h jbwm.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h util.h pixel_t.h
screen.o: screen.c screen.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h jbwm.h config.h \
 display.h ewmh_state.h ewmh.h font.h log.h title_bar.h util.h wm_state.h
select.o: select.c select.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h client.h display.h \
 ewmh.h ewmh_state.h screen.h jbwm.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h
signal.o: signal.c signal.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h client.h display.h log.h title_bar.h
title_bar.o: title_bar.c title_bar.h JBWMScreen.h JBWMSize.h pixel_t.h \
 config.h display.h ewmh.h ewmh_state.h font.h jbwm.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h select.h util.h \
 wm_state.h
util.o: util.c util.h pixel_t.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h display.h ewmh.h ewmh_state.h log.h util.h pixel_t.h
