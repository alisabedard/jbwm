// Copyright 2016, Jeffrey E. Bedard
#ifndef JBWM_JBWMCLIENTMANAGER_H
#define JBWM_JBWMCLIENTMANAGER_H
#include "JBWMClient.h"
struct JBWMClientManager {
	struct JBWMClient * current, * head;
};
#endif//!JBWM_JBWMCLIENTMANAGER_H
