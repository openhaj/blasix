# tooling.mk - developer tools: clangd database, lint, format, headers.

.PHONY: compdb tidy format init headers-add headers-check

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
