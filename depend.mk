atom.o: atom.c atom.h
button_event.o: button_event.c button_event.h client.h drag.h log.h title_bar.h wm_state.h
client.o: client.c client.h ewmh.h ewmh_state.h atom.h select.h title_bar.h util.h vdesk.h wm_state.h
command_line.o: command_line.c command_line.h config.h key_masks.h log.h
display.o: display.c display.h client.h config.h log.h util.h
drag.o: drag.c drag.h font.h JBWMClient.h move_resize.h snap.h
events.o: events.c events.h button_event.h client.h ewmh.h ewmh_client.h ewmh_client_list.h JBWMScreen.h key_event.h log.h move_resize.h new.h select.h title_bar.h util.h wm_state.h
ewmh.o: ewmh.c ewmh.h atom.h config.h JBWMClient.h log.h macros.h
ewmh_client.o: ewmh_client.c ewmh_client.h client.h config.h drag.h ewmh.h ewmh_wm_state.h atom.h select.h util.h vdesk.h wm_state.h
ewmh_client_list.o: ewmh_client_list.c ewmh_client_list.h JBWMClient.h atom.h config.h log.h macros.h
ewmh_state.o: ewmh_state.c ewmh_state.h client.h drag.h ewmh.h jbwm.h atom.h log.h max.h select.h util.h wm_state.h
ewmh_wm_state.o: ewmh_wm_state.c ewmh_wm_state.h atom.h JBWMClient.h client.h ewmh.h ewmh_state.h log.h max.h util.h
exec.o: exec.c exec.h config.h
font.o: font.c font.h config.h util.h
geometry.o: geometry.c geometry.h JBWMClient.h JBWMScreen.h JBWMSize.h log.h macros.h
jbwm.o: jbwm.c jbwm.h config.h ewmh.h font.h atom.h JBWMScreen.h keys.h log.h new.h util.h
key_event.o: key_event.c key_event.h JBWMClient.h JBWMKeys.h JBWMScreen.h client.h config.h drag.h exec.h key_masks.h log.h max.h move_resize.h select.h snap.h title_bar.h vdesk.h wm_state.h
key_masks.o: key_masks.c key_masks.h config.h
keys.o: keys.c keys.h JBWMKeys.h config.h key_masks.h
main.o: main.c JBWMScreen.h atom.h command_line.h display.h events.h font.h jbwm.h
max.o: max.c max.h ewmh.h ewmh_state.h font.h atom.h JBWMClient.h move_resize.h title_bar.h
move_resize.o: move_resize.c move_resize.h JBWMClient.h font.h log.h mwm.h shape.h title_bar.h
mwm.o: mwm.c mwm.h ewmh.h atom.h JBWMClient.h log.h mwmproto.h util.h
new.o: new.c new.h client.h config.h ewmh.h geometry.h atom.h JBWMClient.h key_masks.h log.h mwm.h select.h shape.h snap.h util.h vdesk.h
select.o: select.c select.h client.h ewmh.h ewmh_state.h atom.h JBWMClient.h util.h
shape.o: shape.c shape.h JBWMClient.h log.h
snap.o: snap.c snap.h config.h JBWMClient.h JBWMPoint.h JBWMSize.h
title_bar.o: title_bar.c title_bar.h config.h ewmh.h ewmh_state.h font.h atom.h move_resize.h util.h wm_state.h
util.o: util.c util.h
vdesk.o: vdesk.c vdesk.h client.h config.h ewmh.h atom.h JBWMClient.h JBWMScreen.h util.h
wm_state.o: wm_state.c wm_state.h ewmh.h ewmh_state.h atom.h log.h util.h
