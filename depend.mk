button_event.o: button_event.c button_event.h JBWMClient.h JBWMScreen.h \
 JBDim.h typedef.h client.h log.h screen.h title_bar.h wm_state.h
client.o: client.c client.h JBWMClient.h JBWMScreen.h JBDim.h typedef.h \
 display.h ewmh.h ewmh_state.h jbwm.h log.h screen.h select.h title_bar.h \
 util.h
display.o: display.c display.h client.h JBWMClient.h JBWMScreen.h JBDim.h \
 typedef.h config.h events.h font.h jbwm.h log.h
events.o: events.c events.h JBWMClient.h JBWMScreen.h JBDim.h typedef.h \
 button_event.h client.h display.h ewmh_state.h ewmh.h jbwm.h keys.h \
 log.h new.h screen.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h client.h JBWMClient.h JBWMScreen.h JBDim.h \
 typedef.h config.h display.h font.h log.h macros.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h JBWMClient.h \
 JBWMScreen.h JBDim.h typedef.h display.h ewmh.h jbwm.h log.h max.h \
 screen.h select.h util.h wm_state.h
font.o: font.c font.h config.h jbwm.h JBWMScreen.h JBDim.h typedef.h
jbwm.o: jbwm.c jbwm.h JBWMScreen.h JBDim.h typedef.h client.h \
 JBWMClient.h config.h display.h events.h ewmh.h font.h keys.h log.h \
 new.h util.h
keys.o: keys.c keys.h JBWMScreen.h JBDim.h typedef.h client.h \
 JBWMClient.h config.h display.h jbwm.h log.h max.h screen.h select.h \
 snap.h title_bar.h util.h wm_state.h
max.o: max.c max.h JBWMClient.h JBWMScreen.h JBDim.h typedef.h display.h \
 ewmh.h ewmh_state.h font.h jbwm.h log.h screen.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h JBWMScreen.h JBDim.h typedef.h display.h \
 log.h util.h
new.o: new.c new.h typedef.h client.h JBWMClient.h JBWMScreen.h JBDim.h \
 config.h display.h ewmh.h jbwm.h keys.h log.h macros.h mwm.h screen.h \
 select.h shape.h util.h
screen.o: screen.c screen.h typedef.h client.h JBWMClient.h JBWMScreen.h \
 JBDim.h config.h display.h ewmh_state.h ewmh.h font.h jbwm.h shape.h \
 snap.h title_bar.h util.h wm_state.h
select.o: select.c select.h client.h JBWMClient.h JBWMScreen.h JBDim.h \
 typedef.h display.h ewmh.h ewmh_state.h screen.h util.h
shape.o: shape.c shape.h display.h log.h
snap.o: snap.c snap.h JBDim.h JBWMClient.h JBWMScreen.h typedef.h \
 client.h config.h font.h screen.h
title_bar.o: title_bar.c title_bar.h display.h ewmh.h ewmh_state.h font.h \
 jbwm.h JBWMScreen.h JBDim.h typedef.h screen.h select.h util.h \
 wm_state.h JBWMClient.h
util.o: util.c util.h typedef.h jbwm.h JBWMScreen.h JBDim.h
wm_state.o: wm_state.c wm_state.h JBWMClient.h JBWMScreen.h JBDim.h \
 typedef.h display.h log.h util.h
