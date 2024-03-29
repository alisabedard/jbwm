/* Mixtures of Ctrl, Alt an Escape are used for things like VMWare and
 * XFree86/Cygwin, so the KILL key is an option in the Makefile */
#ifndef JBWM_KEY_COMBOS_H
#define JBWM_KEY_COMBOS_H

#define JBWM_NUM_KEYS JBWM_KEY_0, JBWM_KEY_1, JBWM_KEY_2, \
  JBWM_KEY_3, JBWM_KEY_4, JBWM_KEY_5, JBWM_KEY_6, JBWM_KEY_7, \
  JBWM_KEY_8, JBWM_KEY_9

#define JBWM_VWM_KEYS JBWM_KEY_PREVDESK, JBWM_KEY_NEXTDESK, \
JBWM_NUM_KEYS, JBWM_KEY_VDESK_NEXT_ROW, JBWM_KEY_VDESK_PREV_ROW
#define JBWM_DIRECTION_KEYS JBWM_KEY_LEFT, JBWM_KEY_RIGHT, \
JBWM_KEY_DOWN, JBWM_KEY_UP
#define JBWM_KEYS_TO_GRAB JBWM_KEY_NEXT, JBWM_KEY_NEW, JBWM_KEY_QUIT,\
JBWM_KEY_KILL, JBWM_DIRECTION_KEYS, JBWM_KEY_LOWER,\
JBWM_KEY_ALTLOWER, JBWM_KEY_MAX, JBWM_KEY_STICK,\
JBWM_VWM_KEYS, JBWM_KEY_MOVE, JBWM_KEY_RAISE, JBWM_KEY_SHADE,\
JBWM_KEY_MAX_H, JBWM_KEY_MAX_V, JBWM_KEY_FS, 0
#define JBWM_ALT_KEYS_TO_GRAB JBWM_KEY_KILL, JBWM_DIRECTION_KEYS,\
JBWM_VWM_KEYS, 0
#endif//!JBWM_KEY_COMBOS_H
