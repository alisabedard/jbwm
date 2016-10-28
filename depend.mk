button_event.o: button_event.c button_event.h JBWMClient.h JBWMScreen.h \
 typedef.h client.h jbwm.h log.h screen.h title_bar.h
client.o: client.c client.h JBWMClient.h JBWMScreen.h typedef.h ewmh.h \
 ewmh_state.h jbwm.h log.h screen.h title_bar.h util.h
events.o: events.c events.h JBWMClient.h JBWMScreen.h typedef.h \
 button_event.h client.h ewmh_state.h ewmh.h jbwm.h keys.h log.h new.h \
 screen.h title_bar.h util.h
ewmh.o: ewmh.c ewmh.h client.h JBWMClient.h JBWMScreen.h typedef.h jbwm.h \
 log.h screen.h util.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h JBWMClient.h \
 JBWMScreen.h typedef.h ewmh.h jbwm.h log.h max.h screen.h util.h
jbwm.o: jbwm.c jbwm.h JBWMClient.h JBWMScreen.h typedef.h client.h \
 config.h events.h ewmh.h keys.h log.h new.h util.h
keys.o: keys.c keys.h JBWMScreen.h typedef.h client.h JBWMClient.h \
 config.h jbwm.h log.h max.h screen.h snap.h title_bar.h util.h
max.o: max.c max.h JBWMClient.h JBWMScreen.h typedef.h ewmh.h \
 ewmh_state.h jbwm.h log.h screen.h title_bar.h
mwm.o: mwm.c mwm.h log.h util.h typedef.h
new.o: new.c new.h JBWMScreen.h typedef.h client.h JBWMClient.h ewmh.h \
 jbwm.h keys.h log.h mwm.h screen.h shape.h title_bar.h util.h
screen.o: screen.c screen.h JBWMClient.h JBWMScreen.h typedef.h client.h \
 ewmh.h ewmh_state.h jbwm.h log.h shape.h snap.h title_bar.h util.h
shape.o: shape.c shape.h log.h
snap.o: snap.c snap.h client.h JBWMClient.h JBWMScreen.h typedef.h \
 config.h jbwm.h
title_bar.o: title_bar.c title_bar.h client.h JBWMClient.h JBWMScreen.h \
 typedef.h ewmh.h ewmh_state.h jbwm.h screen.h util.h
util.o: util.c util.h typedef.h jbwm.h JBWMClient.h JBWMScreen.h
