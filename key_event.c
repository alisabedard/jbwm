/* Copyright 2020, Jeffrey E. Bedard */
/* Module: */
#include "key_event.h"
/* System: */
#include <stdbool.h>
#include <stdlib.h>
/* Local: */
#include "JBWMClient.h"
#include "JBWMKeys.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "drag.h"
#include "exec.h"
#include "key_masks.h"
#include "log.h"
#include "max.h"
#include "move_resize.h"
#include "select.h"
#include "snap.h"
#include "title_bar.h"
#include "vdesk.h"
#include "wm_state.h"
/* Program: */
__attribute__((nonnull))
static void point(struct JBWMClient * c,
  const int16_t x, const int16_t y)
{
  Display * d = c->screen->display;
  XRaiseWindow(d, c->parent);
  jbwm_warp(d, c->window, x, y);
}
__attribute__((nonnull))
static void commit_key_move(struct JBWMClient * c)
{
  jbwm_snap_border(c);
  jbwm_move_resize(c);
  point(c, 1, 1);
}
enum KeyMoveFlags {
  KEY_MOVE_HORIZONTAL = 1,
  KEY_MOVE_POSITIVE = 2,
  KEY_MOVE_MODIFIER = 4
};
static inline bool can_resize(struct JBWMClientOptions * opt)
{
  return !opt->shaped && !opt->no_resize;
}
static inline bool has_sufficient_size(const int value)
{
  return value > JBWM_RESIZE_INCREMENT << 1;
}
static int16_t * get_antecedent(struct JBWMClient * c, const
  uint8_t flags)
{
  int16_t * ret;
  union JBWMRectangle * s = &c->size;
  int16_t * wh = flags & KEY_MOVE_HORIZONTAL ? &s->width : &s->height;
  if((flags & KEY_MOVE_MODIFIER) && has_sufficient_size(*wh)
    && can_resize(&c->opt))
    ret = wh;
  else
    ret = (flags & KEY_MOVE_HORIZONTAL ? &s->x : &s->y);
  return ret;
}
__attribute__((nonnull))
static void key_move(struct JBWMClient * c, uint8_t const flags)
{
  int8_t const d = flags & KEY_MOVE_POSITIVE
      ? JBWM_RESIZE_INCREMENT : - JBWM_RESIZE_INCREMENT;
  *get_antecedent(c, flags) += d;
  commit_key_move(c);
}
static uint8_t get_move_flags(const KeySym k, const bool mod)
{
  uint8_t flags = (mod ? KEY_MOVE_MODIFIER : 0) | KEY_MOVE_POSITIVE;
  switch (k) {
  case JBWM_KEY_LEFT:
    flags &= ~KEY_MOVE_POSITIVE;
    /* FALLTHROUGH */
  case JBWM_KEY_RIGHT:
    flags |= KEY_MOVE_HORIZONTAL;
    break;
  case JBWM_KEY_UP:
    flags &= ~KEY_MOVE_POSITIVE;
  }
  return flags;
}
static void handle_key_move(struct JBWMClient * c,
  const KeySym k, const bool mod)
{
  /* These operations invalid when fullscreen. */
  if (c->opt.fullscreen)
    return;
  key_move(c, get_move_flags(k, mod));
}
static void toggle_maximize(struct JBWMClient * c)
{
  const struct JBWMClientOptions o = c->opt;
  /* Honor mwm hints. Do not maximize shaped windows. */
  /* Ignore fullscreen windows. Let the fullscreen code handle them. */
  if (!o.no_max && !o.fullscreen && !o.shaped) {
    if (o.max_horz && o.max_vert) {
      jbwm_set_not_horz(c);
      jbwm_set_not_vert(c);
    } else {
      jbwm_set_horz(c);
      jbwm_set_vert(c);
    }
  }
}
__attribute__((nonnull))
static void handle_client_key_event(struct JBWMClient ** head_client,
  struct JBWMClient ** current_client,
  const bool mod, const KeySym key)
{
  struct JBWMClient * c;
  JBWM_LOG("handle_client_key_event: %d", (int)key);
  c=*current_client;
  if (c) {
    if (c->opt.fullscreen) {
      /* only allow exiting from fullscreen */
      if (key == JBWM_KEY_FS)
        jbwm_set_not_fullscreen(c);
      return; /* prevent other operations while fullscreen */
    }
    switch (key) {
    case JBWM_KEY_LEFT:
    case JBWM_KEY_RIGHT:
    case JBWM_KEY_UP:
    case JBWM_KEY_DOWN:
      handle_key_move(c, key, mod);
      break;
    case JBWM_KEY_KILL:
      jbwm_send_wm_delete(c);
      break;
    case JBWM_KEY_LOWER:
    case JBWM_KEY_ALTLOWER:
      XLowerWindow(c->screen->display, c->parent);
      break;
    case JBWM_KEY_RAISE:
      XRaiseWindow(c->screen->display, c->parent);
      break;
    case JBWM_KEY_FS:
      jbwm_set_fullscreen(c);
      break;
    case JBWM_KEY_MAX:
      toggle_maximize(c);
      break;
    case JBWM_KEY_MAX_H:
      (c->opt.max_horz ? jbwm_set_not_horz : jbwm_set_horz)(c);
      break;
    case JBWM_KEY_MAX_V:
      (c->opt.max_vert ? jbwm_set_not_vert : jbwm_set_vert)(c);
      break;
    case JBWM_KEY_STICK:
      jbwm_toggle_sticky(c, current_client);
      break;
    case JBWM_KEY_MOVE:
      jbwm_drag(c, head_client, false);
      break;
    case JBWM_KEY_SHADE:
      jbwm_toggle_shade(c);
      break;
    }
  }
}
static void next(struct JBWMClient * c,
  struct JBWMClient ** current_client, struct JBWMClient ** head_client,
  uint8_t const v){
  if(!c->next)
    c=*(head_client);
  else
    c=c->next;
  if (c!=*current_client) { /* prevent infinite recursion with dessktop */
    if(c->vdesk != v) {
      next(c, current_client, head_client, v);
    } else {
      point(c, 0, 0);
      point(c, c->size.width-1, c->size.height-1);
      jbwm_select_client(c, current_client);
    }
  }
}
static void cond_set_vdesk(struct JBWMClient * c,
  struct JBWMClient * head_client,
  struct JBWMScreen * s, const uint8_t desktop, const bool mod)
{
  if (mod && c)
    jbwm_set_client_vdesk(c, desktop);
  else
    jbwm_set_vdesk(s, head_client, desktop);
}
void jbwm_handle_key_event(struct JBWMScreen * s, struct JBWMClient * target,
  struct JBWMClient ** head_client, struct JBWMClient ** current_client,
  XKeyEvent * e)
{
  const KeySym key = XLookupKeysym(e, 0);
  struct {
    uint8_t vdesk:6;
    bool mod:1;
    bool zero:1;
  } opt = {s->vdesk, e->state & jbwm_get_mod_mask(), 0};
  JBWM_LOG("jbwm_handle_key_event");
  switch (key) {
  case JBWM_KEY_NEW:
    jbwm_exec(JBWM_TERM);
    break;
  case JBWM_KEY_QUIT:
    exit(0);
  case JBWM_KEY_NEXT:
    if(*current_client)
      next(*current_client, current_client, head_client, s->vdesk);
    break;
  case XK_0:
    opt.zero = true;
    /* FALLTHROUGH */
  case XK_1: case XK_2: case XK_3: case XK_4: case XK_5:
  case XK_6: case XK_7: case XK_8: case XK_9:
    /* First desktop 0, per wm-spec */
    cond_set_vdesk(target, *head_client, s, opt.zero
      ? 10 : key - XK_1, opt.mod);
    break;
  case JBWM_KEY_PREVDESK:
    cond_set_vdesk(target, *head_client, s, s->vdesk - 1, opt.mod);
    break;
  case JBWM_KEY_NEXTDESK:
    cond_set_vdesk(target, *head_client, s, s->vdesk + 1, opt.mod);
    break;
  default:
    if (target)
      handle_client_key_event(head_client, current_client, opt.mod, key);
  }
}
