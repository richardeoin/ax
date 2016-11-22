#
# makefile
#

# Directories
#
#
OUTPUT_PATH := out/

# Shell Commands
#
# Listed here for portability.
#
CAT		:= cat
ECHO	:= echo
FIND	:= find
GREP	:= grep
MKDIR	:= mkdir -p
RM		:= rm -r
SED		:= sed
TR		:= tr

# Compilation Flags
#
# Display all warnings
#
CFLAGS  += -g3 -ggdb -std=c99 -Wall -Wextra -pthread -I. -I..
LDFLAGS += -g3 -ggdb -Wall -Wextra

# Source files
SOURCES		= $(shell $(FIND) . -name '*.c')

# Header files
#
INCLUDES	= $(shell $(FIND) . -name '*.h')

# Object files
objects		= ax/ax.o ax/ax_hw.o ax/ax_modes.o ax/ax_params.o ax_test.o
OBJECTS		= $(addprefix $(OUTPUT_PATH),$(objects))

# Assemble a list of c and h files that are used in this project
#
TAGFILES	= $(SOURCES) $(INCLUDES)


# Default target
#
#
all: ax_test etags

# ax_test
#
#
ax_test: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm -lwiringPi -lwiringPiDev -pthread -lpigpio -lrt


# Compile objects
#
$(OUTPUT_PATH)%.o: %.c $(INCLUDES)
	@$(ECHO)
	@$(ECHO) 'Compiling $<...'
	@$(MKDIR) $(OUTPUT_PATH)$(dir $<)
	$(CC) -c $(CFLAGS) -o $@ $<


# Generates an etags file for the project
#
#
etags: $(TAGFILES)
	@$(ECHO) "Generating ETAGS..."
	@etags $^

# Launches emacs with all the files used for this project
#
.PHONY: emacs
emacs:
	@emacs $(TAGFILES) Makefile README.md
