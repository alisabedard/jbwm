# Debug
CC=clang
jbwm_cflags+=-W -Wall -Wextra -Werror
jbwm_cflags+=-Wuninitialized -Wstrict-overflow=5
jbwm_cflags+=-Wlarger-than=512
jbwm_cflags+=-Wframe-larger-than=512
jbwm_cflags+=-Wcast-qual -Wcast-align
jbwm_cflags+=-Weverything
jbwm_cflags+=-Wno-conversion
jbwm_cflags+=-Wno-vla
jbwm_cflags+=-Wno-switch-enum
