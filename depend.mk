-e button_event.o: button_event.c button_event.h client.h drag.h log.h title_bar.h wm_state.h
client.o: client.c client.h ewmh.h ewmh_state.h select.h title_bar.h util.h vdesk.h wm_state.h
command_line.o: command_line.c command_line.h config.h key_masks.h log.h
display.o: display.c display.h client.h config.h log.h util.h
drag.o: drag.c drag.h font.h JBWMClient.h move_resize.h snap.h
events.o: events.c events.h button_event.h client.h ewmh.h ewmh_client.h JBWMScreen.h key_event.h log.h move_resize.h new.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h JBWMClient.h JBWMScreen.h client.h config.h font.h geometry.h log.h macros.h PropertyData.h util.h
ewmh_client.o: ewmh_client.c ewmh_client.h client.h config.h drag.h ewmh.h ewmh_wm_state.h select.h util.h vdesk.h wm_state.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h drag.h ewmh.h jbwm.h log.h max.h select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.c ewmh_wm_state.h JBWMClient.h client.h ewmh.h ewmh_state.h log.h max.h util.h
exec.o: exec.c exec.h
font.o: font.c font.h config.h util.h
geometry.o: geometry.c geometry.h JBWMClient.h JBWMScreen.h JBWMSize.h log.h macros.h
jbwm.o: jbwm.c jbwm.h config.h ewmh.h font.h JBWMScreen.h keys.h log.h new.h util.h
key_event.o: key_event.c key_event.h JBWMKeys.h JBWMScreen.h client.h drag.h exec.h keys.h key_masks.h log.h max.h move_resize.h select.h snap.h title_bar.h vdesk.h wm_state.h
key_masks.o: key_masks.c key_masks.h config.h
keys.o: keys.c keys.h JBWMKeys.h key_masks.h
main.o: main.c command_line.h display.h events.h font.h JBWMClient.h jbwm.h vdesk.h
max.o: max.c max.h ewmh.h ewmh_state.h font.h JBWMClient.h move_resize.h title_bar.h
move_resize.o: move_resize.c move_resize.h JBWMClient.h font.h log.h mwm.h shape.h title_bar.h
mwm.o: mwm.c mwm.h JBWMClient.h log.h mwmproto.h util.h
new.o: new.c new.h JBWMClient.h client.h config.h ewmh.h geometry.h key_masks.h log.h mwm.h select.h shape.h util.h vdesk.h
select.o: select.c select.h client.h ewmh.h ewmh_state.h JBWMClient.h util.h
shape.o: shape.c shape.h JBWMClient.h log.h
snap.o: snap.c snap.h client.h config.h geometry.h JBWMClient.h JBWMPoint.h JBWMSize.h log.h
title_bar.o: title_bar.c title_bar.h config.h ewmh.h ewmh_state.h font.h move_resize.h util.h wm_state.h
util.o: util.c util.h
vdesk.o: vdesk.c vdesk.h JBWMClient.h JBWMScreen.h client.h config.h ewmh.h util.h
wm_state.o: wm_state.c wm_state.h ewmh.h ewmh_state.h log.h util.h

