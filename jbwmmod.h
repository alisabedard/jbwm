/* Copyright 2007-2012, Jeffrey E. Bedard */

#ifndef _JBWMMOD_H
#define _JBWMMOD_H

#include "config.h"
#include "keys.h"
#include "screen.h"
#include "client.h"
#include "new.h"
#include "events.h"
#include "graphics.h"
#include "button_event.h"

// Optional modules:
#ifdef USE_TBAR
#include "titlebar.h"
#endif//USE_TBAR 

#ifdef EWMH
#include "ewmh.h"
#endif//EWMH

#endif//!_JBWMMOD_H
