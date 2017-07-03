# Debug
CC=gcc
# Profile
jbwm_cflags+=-pg
# Enable all warnings
jbwm_cflags+=-W -Wall -Wextra -Werror
jbwm_cflags+=-Wuninitialized -Wstrict-overflow=5
jbwm_cflags+=-Wsuggest-attribute=pure
jbwm_cflags+=-Wsuggest-attribute=const
jbwm_cflags+=-Wsuggest-attribute=noreturn
jbwm_cflags+=-Wsuggest-attribute=format
jbwm_cflags+=-Wlarger-than=512
jbwm_cflags+=-Wframe-larger-than=512
jbwm_cflags+=-Wcast-qual -Wcast-align
jbwm_cflags+=-Wjump-misses-init
jbwm_cflags+=-Wlogical-op
jbwm_cflags+=-Wmissing-prototypes
jbwm_cflags+=-Wredundant-decls -Wpointer-sign
jbwm_cflags+=-fstrict-aliasing -Wstrict-aliasing=2
jbwm_cflags+=-Wformat=2
