# Debug
jbwm_cflags=-O0 -ggdb -DDEBUG
# Profile
jbwm_cflags+=-pg
# Enable all warnings
jbwm_cflags+=-W -Wall -Wextra -Werror
# Select compiler
#include debug_clang.mk
include debug_gcc.mk
