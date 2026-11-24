# tooling.mk - developer tools: clangd database, lint, format, headers.

.PHONY: compdb tidy format init headers-add headers-check sources-check

# Generate compile_commands.json for clangd (needs `bear`).
compdb:
	@command -v bear >/dev/null 2>&1 || { echo "bear not found"; exit 1; }
	@rm -rf $(BUILD_DIR)/runner $(OUT)/fw $(OUT)/bin
	@bear -- $(MAKE) --no-print-directory build

# clang-tidy on lcrun and the framework (uses .clang-tidy).
tidy:
	@clang-tidy $(RUNNER_SRCS) $(FW_SRCS) $(FW_COV_SRC) -- \
		-std=$(STD_HOST) -I$(INC_DIR)

# Format sources in place (needs a .clang-format file).
format:
	@[ -f .clang-format ] || { echo "No .clang-format file"; exit 1; }
	@clang-format -i $(ALL_C_FILES)

init:
	@git config core.hooksPath .githooks
	@chmod +x .githooks/pre-commit scripts/*.sh
	@echo "git hooks enabled (.githooks)"

headers-add:
	@for f in $(ALL_C_FILES) templates/test.c; do ./scripts/header.sh "$$f"; done

headers-check:
	@./scripts/header-check.sh $(SRC_DIR) $(INC_DIR) templates

# Every .c, .h of src/ and include/ and every spec must be listed in
# sources.mk or suites.mk (nothing is picked up by a wildcard).
LISTED_FILES	= $(sort $(ALL_C_FILES) $(SPECS))
sources-check:
	@mkdir -p $(BUILD_DIR)
	@printf '%s\n' $(LISTED_FILES) > $(BUILD_DIR)/.listed
	@find $(SRC_DIR) $(INC_DIR) $(SPEC_DIR) -type f \
		\( -name '*.c' -o -name '*.h' -o -name '*.spec' \) | sort > $(BUILD_DIR)/.found
	@st=0; \
	for f in $$(comm -13 $(BUILD_DIR)/.listed $(BUILD_DIR)/.found); do \
		echo "$$f: not listed in mk/sources.mk or mk/suites.mk"; st=1; done; \
	for f in $$(comm -23 $(BUILD_DIR)/.listed $(BUILD_DIR)/.found); do \
		echo "$$f: listed but missing"; st=1; done; \
	rm -f $(BUILD_DIR)/.listed $(BUILD_DIR)/.found; \
	[ $$st -eq 0 ] && echo "sources: $(words $(LISTED_FILES)) files, all listed"; exit $$st
