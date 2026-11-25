# build.mk - common build helpers.
#
#   $(call log,TAG,WHAT)
#       print "  TAG      WHAT" (nothing with SHOW_FLAGS=1).
#   $(AT)
#       prefix of a command line: hidden unless SHOW_FLAGS=1.
#   $(call lc_tick,WHAT[,LINE])
#       one step of the build progress bar (scripts/progress.sh): on a
#       terminal, redraw "building [###...] n/total WHAT"; in a log, print
#       LINE if given. Counted in $(LC_PROGRESS), out of $(LC_TOTAL), both
#       set by the Makefile for the sub-make that builds units and probes.
#   $(call lc_clear)
#       erase the progress line (terminal only), before a line of its own.
#   $(eval $(call c_objects,OBJDIR,SRCDIR,CC_VAR,FLAGS_VAR,TAG))
#       OBJDIR/%.o: SRCDIR/%.c, compiled with $(CC_VAR) $(FLAGS_VAR)
#       -I$(INC_DIR), with header dependencies (-MMD -MP). CC_VAR and
#       FLAGS_VAR are variable names, expanded when the rule runs (so
#       target-specific values and commas in flags work).
#   $(call lc_write_cmd,FILE,TEXT)
#       write TEXT to FILE only if FILE holds something else (no shell,
#       expanded in a recipe). A target that depends on FILE is rebuilt
#       when TEXT changes (its command line, its flags), not on every run.
#   $(call lc_update,NEW,FILE)
#       shell: replace FILE by NEW only if their contents differ, so that
#       an unchanged FILE keeps its date; NEW is removed.
#
# Modules append the .d files of their objects to DEPS; the Makefile
# includes them at the end.
#
# Options (on the command line):
#   SHOW_FLAGS=1 : print the full command lines

ifeq ($(SHOW_FLAGS),1)
AT		:=
log		=
else
AT		:= @
log		= @printf '  %-8s %s\n' '$(1)' '$(2)'
endif

WARNINGS	:= -Wall -Wextra

LC_PROGRESS	?=
LC_TOTAL	?= 0
lc_tick		= $(if $(LC_PROGRESS),n=$$(printf . >> $(LC_PROGRESS) && wc -c < $(LC_PROGRESS)); \
	scripts/progress.sh $$n $(LC_TOTAL) '$(1)' '$(2)',$(if $(2),echo '$(2)',:))
lc_clear	= if [ -t 1 ]; then printf '\r\033[2K'; fi

lc_streq		= $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
lc_write_cmd	= $(if $(call lc_streq,$(file <$(1)),$(2)),,$(shell \
	mkdir -p $(dir $(1)))$(file >$(1),$(2)))
lc_update		= if cmp -s $(1) $(2); then rm -f $(1); else mv -f $(1) $(2); fi

# Prerequisite of the command files: their rule runs on every make.
.PHONY: FORCE
FORCE:

DEPS	:=

define c_objects
$(1)/%.o: $(2)/%.c
	@mkdir -p $$(@D)
	$$(call log,$(5),$$<)
	$$(AT)$$($(3)) $$($(4)) -I$$(INC_DIR) -MMD -MP -c $$< -o $$@
endef
