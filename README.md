# libjb
This is a library of convenience code for X11/xcb, unix and C programming.
It provides a common logging and assertion framework, xcb convenience functions,
min and max macros, and more to come.
libjb provides both a shared and static library.
# NOTES
On OpenBSD, please build with CC=clang, since the default compiler is too old to compile C11 code.
