#ifndef JBWM_PROPERTYDATA_H
#define JBWM_PROPERTYDATA_H
#include "JBWMAtomIndex.h"
#include <X11/Xlib.h>
struct PropertyData {
    Display * display;
    void * data;
    Window target;
    Atom property;
    uint16_t size, type;
};
#endif//!JBWM_PROPERTYDATA_H
