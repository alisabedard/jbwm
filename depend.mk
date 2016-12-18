button_event.o: button_event.c button_event.h JBWMClient.h client.h log.h \
 screen.h title_bar.h wm_state.h
client.o: client.c client.h JBWMClient.h JBWMScreen.h JBDim.h pixel_t.h \
 display.h ewmh.h ewmh_state.h jbwm.h log.h screen.h select.h title_bar.h \
 util.h
display.o: display.c display.h client.h config.h events.h font.h jbwm.h \
 log.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMDefaults.h JBWMScreen.h JBDim.h \
 pixel_t.h client.h config.h display.h font.h log.h macros.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h JBWMScreen.h JBDim.h pixel_t.h \
 client.h display.h ewmh.h jbwm.h log.h max.h screen.h select.h util.h \
 wm_state.h JBWMClient.h
font.o: font.c font.h config.h jbwm.h
max.o: max.c max.h JBWMClient.h JBWMScreen.h JBDim.h pixel_t.h display.h \
 ewmh.h ewmh_state.h font.h log.h screen.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h display.h log.h util.h pixel_t.h
screen.o: screen.c screen.h JBWMDefaults.h JBWMScreen.h JBDim.h pixel_t.h \
 client.h config.h display.h ewmh_state.h ewmh.h font.h jbwm.h shape.h \
 snap.h title_bar.h util.h wm_state.h JBWMClient.h
select.o: select.c select.h JBWMClient.h JBWMScreen.h JBDim.h pixel_t.h \
 client.h display.h ewmh.h ewmh_state.h screen.h util.h
shape.o: shape.c shape.h JBWMClient.h display.h log.h
snap.o: snap.c snap.h JBDim.h JBWMClient.h JBWMDefaults.h JBWMScreen.h \
 pixel_t.h client.h font.h screen.h
title_bar.o: title_bar.c title_bar.h JBWMScreen.h JBDim.h pixel_t.h \
 display.h ewmh.h ewmh_state.h font.h jbwm.h screen.h select.h util.h \
 wm_state.h JBWMClient.h
util.o: util.c util.h pixel_t.h jbwm.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h display.h log.h util.h \
 pixel_t.h
