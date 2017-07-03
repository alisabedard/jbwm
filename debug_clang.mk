# Debug
CC=clang
jbwm_cflags+=-W -Wall -Wextra -Werror
jbwm_cflags+=-Wuninitialized -Wstrict-overflow=5
jbwm_cflags+=-Wlarger-than=512
jbwm_cflags+=-Wframe-larger-than=512
jbwm_cflags+=-Wcast-qual -Wcast-align
jbwm_cflags+=-Wmissing-prototypes
jbwm_cflags+=-Wpadded
jbwm_cflags+=-Wredundant-decls -Wpointer-sign
jbwm_cflags+=-fstrict-aliasing -Wstrict-aliasing=2
jbwm_cflags+=-Wformat=2
