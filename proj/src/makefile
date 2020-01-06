# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = game.c proj.c video.c keyboard.c utils.c mouse.c xpm_includes.c board_tile.c timer.c player.c mouse_trigger.c sp.c queue.c rtc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
