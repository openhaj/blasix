# Makefile - generic build for C projects.
#
# Configuration lives in mk/config.mk (project name, type, std...)
# and mk/sources.mk (source list). Run `make help` for the targets.

include mk/config.mk
include mk/sources.mk

# ----- Output name -----

ifeq ($(TYPE),exec)
NAME	?= $(PROJECT_NAME)
else ifeq ($(TYPE),static)
NAME	?= lib$(PROJECT_NAME).a
else ifeq ($(TYPE),shared)
NAME	?= lib$(PROJECT_NAME).so
else
$(error TYPE must be one of: exec, static, shared (got '$(TYPE)'))
endif

# ----- Toolchain and flags -----

CC	?= cc
AR	?= ar
ARFLAGS	:= rcs

# Build mode: release | debug | asan
MODE	?= release

WARNINGS := \
	-Wall -Wextra -Wpedantic \
	-Wshadow -Wconversion -Wsign-conversion -Wcast-align \
	-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	-Wold-style-definition -Wpointer-arith -Wwrite-strings -Wundef \
	-Wredundant-decls -Wnested-externs

CPPFLAGS	+= -I$(INC_DIR) -I$(SRC_DIR) -MMD -MP \
		   -D$(shell echo '$(PROJECT_NAME)' | tr 'a-z-' 'A-Z_')_VERSION='"$(VERSION)"'
CFLAGS		+= -std=$(STD) $(WARNINGS)
LDFLAGS		+=
LDLIBS		+=

ifeq ($(MODE),release)
CFLAGS		+= -O2
CPPFLAGS	+= -DNDEBUG
else ifeq ($(MODE),debug)
CFLAGS		+= -O0 -g3
else ifeq ($(MODE),asan)
CFLAGS		+= -O1 -g3 -fno-omit-frame-pointer -fsanitize=address,undefined
LDFLAGS		+= -fsanitize=address,undefined
else
$(error MODE must be one of: release, debug, asan (got '$(MODE)'))
endif

ifeq ($(WERROR),1)
CFLAGS		+= -Werror
endif

ifeq ($(TYPE),shared)
CFLAGS		+= -fPIC
LDFLAGS		+= -shared
endif

# ----- Sources and objects -----

SRCS	:= $(addprefix $(SRC_DIR)/,$(SRC))
OBJDIR	:= $(BUILD_DIR)/$(MODE)
OBJS	:= $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
DEPS	:= $(OBJS:.o=.d)

# Relink when MODE changes, even if no object is out of date.
MODE_STAMP := $(BUILD_DIR)/.mode
$(shell mkdir -p $(BUILD_DIR); \
	[ "$$(cat $(MODE_STAMP) 2>/dev/null)" = "$(MODE)" ] || echo $(MODE) > $(MODE_STAMP))

# Install locations
PREFIX	?= /usr/local
DESTDIR	?=

# ----- Main targets -----

.DEFAULT_GOAL := all

all: $(NAME)

$(NAME): $(OBJS) $(MODE_STAMP)
ifeq ($(TYPE),static)
	@echo "AR $@"
	@$(AR) $(ARFLAGS) $@ $(OBJS)
else
	@echo "LD $@"
	@$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@
endif

debug:
	@$(MAKE) --no-print-directory MODE=debug

asan:
	@$(MAKE) --no-print-directory MODE=asan

release:
	@$(MAKE) --no-print-directory MODE=release

run: all
ifeq ($(TYPE),exec)
	@./$(NAME) $(ARGS)
else
	$(error `make run` only works with TYPE=exec)
endif

clean:
	@echo "RM $(BUILD_DIR)/"
	@rm -rf $(BUILD_DIR)

fclean: clean
	@echo "RM $(NAME)"
	@rm -f $(NAME) $(PROJECT_NAME) lib$(PROJECT_NAME).a lib$(PROJECT_NAME).so

re: fclean
	@$(MAKE) --no-print-directory all

# ----- Install -----

install: all
ifeq ($(TYPE),exec)
	install -Dm755 $(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)
else
	install -Dm644 $(NAME) $(DESTDIR)$(PREFIX)/lib/$(NAME)
	@if [ -n "$$(find $(INC_DIR) -type f -name '*.h' 2>/dev/null)" ]; then \
		mkdir -p $(DESTDIR)$(PREFIX)/include; \
		cp -r $(INC_DIR)/. $(DESTDIR)$(PREFIX)/include/; \
		rm -f $(DESTDIR)$(PREFIX)/include/.gitkeep; \
	fi
endif

uninstall:
ifeq ($(TYPE),exec)
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
else
	rm -f $(DESTDIR)$(PREFIX)/lib/$(NAME)
	@cd $(INC_DIR) && find . -type f -name '*.h' | while IFS= read -r h; do \
		rm -f "$(DESTDIR)$(PREFIX)/include/$$h"; \
	done
endif

# ----- Tooling -----

# Generate compile_commands.json for clangd (needs `bear`).
compdb:
	@command -v bear >/dev/null 2>&1 || { echo "bear not found"; exit 1; }
	@$(MAKE) --no-print-directory clean
	@bear -- $(MAKE) --no-print-directory all

# Run clang-tidy on every source file (uses .clang-tidy).
tidy:
	@clang-tidy $(SRCS) -- $(CPPFLAGS) -std=$(STD)

# Format sources in place (needs a .clang-format file).
format:
	@[ -f .clang-format ] || { echo "No .clang-format file"; exit 1; }
	@find $(SRC_DIR) $(INC_DIR) -type f \( -name '*.c' -o -name '*.h' \) \
		-exec clang-format -i {} +

init:
	@echo "Installing git hooks..."
	@git config core.hooksPath .githooks
	@chmod +x .githooks/pre-commit
	@chmod +x scripts/*.sh
	@echo "Done. Git hooks are now active."

headers-add:
	@find $(SRC_DIR) $(INC_DIR) -type f \( -name '*.c' -o -name '*.h' -o -name '*.hpp' \
		-o -name '*.cpp' -o -name '*.inl' -o -name '*.S' \) \
		-exec ./scripts/header.sh {} \;

headers-check:
	@./scripts/header-check.sh

help:
	@echo "$(PROJECT_NAME) $(VERSION) ($(TYPE) -> $(NAME))"
	@echo ""
	@echo "Build:"
	@echo "  all            build \$$(NAME) (default, MODE=$(MODE))"
	@echo "  debug          build with -O0 -g3"
	@echo "  asan           build with AddressSanitizer + UBSan"
	@echo "  release        build with -O2 -DNDEBUG"
	@echo "  run            build and run (exec only, pass ARGS=...)"
	@echo "  clean          remove $(BUILD_DIR)/"
	@echo "  fclean         clean + remove \$$(NAME)"
	@echo "  re             fclean + all"
	@echo "  install        install to \$$(DESTDIR)\$$(PREFIX) ($(PREFIX))"
	@echo "  uninstall      remove installed files"
	@echo ""
	@echo "Tooling:"
	@echo "  compdb         generate compile_commands.json (bear)"
	@echo "  tidy           run clang-tidy"
	@echo "  format         run clang-format"
	@echo "  init           install git hooks"
	@echo "  headers-add    add license headers to files missing one"
	@echo "  headers-check  check license headers"
	@echo ""
	@echo "Options: MODE=release|debug|asan  WERROR=1  SHOW_FLAGS=1  UNWIND=1"

.PHONY: all debug asan release run clean fclean re install uninstall \
	compdb tidy format init headers-add headers-check help

include mk/build.mk
