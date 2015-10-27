CC      := gcc
ECHO    := echo
RM      := rm
CD      := cd
TAR     := tar

BUILD_DIR := build/GNU

OUTDIR := $(BUILD_DIR)
CFLAGS  := -Wall -Wextra -std=c99
DEBUG   ?= 0
ifeq ($(DEBUG), 1)
	OUTDIR := $(OUTDIR)/Debug
	CFLAGS += -g -DDEBUG
else
	OUTDIR := $(OUTDIR)/Release
	CFLAGS += -O2
endif
OBJDIR := $(OUTDIR)/obj

LIBS   :=  -lm

PCE_SRC := arch/pce/opcodes.c arch/pce/labels.c arch/pce/disassembly.c

EXE_SRC := $(PCE_SRC) message.c pass.c decode.c cfg.c section.c labels.c labelsloader.c irq.c memory.c memorymap.c rom.c cd.c options.c etripator.c
OBJS    := $(EXE_SRC:.c=.o)
EXE_OBJ := $(addprefix $(OBJDIR)/, $(OBJS))
EXE     := $(OUTDIR)/etripator

DEPEND = .depend

all: $(EXE)

dep: $(DEPEND)

$(DEPEND):
	@$(ECHO) "  MKDEP"
	@$(CC) -MM -MG $(CFLAGS) $(EXE_SRC) > $(DEPEND)

$(EXE): $(EXE_OBJ)
	@$(ECHO) "  LD        $@"
	@$(CC) -o $(EXE) $^ $(LIBS)

$(OBJDIR)/%.o: %.c
	@$(ECHO) "  CC        $@"
	@mkdir -p `dirname $@` 
	@$(CC) $(CFLAGS) -c -o $@ $<

$(EXE_OBJ): | $(OBJDIR) $(OUTDIR)

$(OUTDIR):
	@mkdir  -p $(OUTDIR)

install:

clean: FORCE
	@$(ECHO) "  CLEAN     object files"
	@find $(BUILD_DIR) -name "*.o" -exec $(RM) -f {} \;

realclean: clean
	@$(ECHO) "  CLEAN     $(EXE)"
	@$(RM) -f $(EXE)
	@$(ECHO) "  CLEAN     noise files"
	@$(RM) -f `find . -name "*~" -o -name "\#*"`

c: clean

rc: realclean

archive: realclean
	@$(ECHO) "  TBZ2      `date +"%Y/%m/%d"`"
	@$(CD) ..; $(TAR) jcf etripator-`date +"%Y%m%d"`.tar.bz2 same

FORCE :
ifeq (.depend,$(wildcard .depend))
include .depend
endif
