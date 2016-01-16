// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com> 
#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct
{
        int x, y;
} Position;

typedef struct
{
        uint16_t w, h;
} Dim;

#ifndef USE_TBAR
#define TDIM 0
#endif//!USE_TBAR

#endif//!GEOMETRY_H
