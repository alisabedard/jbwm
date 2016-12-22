button_event.o: button_event.c button_event.h JBWMClient.h \
 JBWMClientOptions.h JBWMRectangle.h client.h drag.h log.h screen.h \
 jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h title_bar.h wm_state.h
client.o: client.c client.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h display.h ewmh.h \
 ewmh_state.h jbwm.h log.h screen.h select.h title_bar.h util.h
display.o: display.c display.h client.h config.h events.h font.h jbwm.h \
 log.h
drag.o: drag.c drag.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h display.h font.h screen.h jbwm.h \
 snap.h
events.o: events.c events.h JBWMScreen.h JBWMSize.h pixel_t.h \
 button_event.h client.h display.h ewmh_state.h ewmh.h jbwm.h keys.h \
 log.h new.h screen.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h \
 display.h font.h log.h macros.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMScreen.h JBWMSize.h pixel_t.h \
 client.h display.h drag.h ewmh.h jbwm.h log.h max.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h select.h util.h \
 wm_state.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBWMSize.h pixel_t.h config.h \
 display.h events.h ewmh.h keys.h log.h new.h signal.h util.h
keys.o: keys.c keys.h JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h \
 client.h config.h display.h drag.h jbwm.h log.h max.h screen.h \
 JBWMClient.h JBWMClientOptions.h JBWMRectangle.h select.h snap.h \
 title_bar.h util.h wm_state.h
max.o: max.c max.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMScreen.h JBWMSize.h pixel_t.h display.h ewmh.h ewmh_state.h font.h \
 log.h screen.h jbwm.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h util.h pixel_t.h
new.o: new.c new.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 JBWMDefaults.h JBWMScreen.h JBWMSize.h pixel_t.h client.h config.h \
 display.h ewmh.h jbwm.h keys.h log.h macros.h mwm.h screen.h select.h \
 shape.h util.h
screen.o: screen.c screen.h jbwm.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h JBWMDefaults.h \
 client.h display.h ewmh_state.h ewmh.h font.h log.h shape.h title_bar.h \
 util.h wm_state.h
select.o: select.c select.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMScreen.h JBWMSize.h pixel_t.h client.h display.h \
 ewmh.h ewmh_state.h screen.h jbwm.h util.h
shape.o: shape.c shape.h JBWMClient.h JBWMClientOptions.h JBWMRectangle.h \
 display.h log.h
signal.o: signal.c signal.h log.h
snap.o: snap.c snap.h JBWMSize.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h JBWMDefaults.h JBWMPoint.h JBWMScreen.h pixel_t.h \
 client.h font.h screen.h jbwm.h
util.o: util.c util.h pixel_t.h jbwm.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMClientOptions.h \
 JBWMRectangle.h display.h ewmh.h ewmh_state.h log.h util.h pixel_t.h
