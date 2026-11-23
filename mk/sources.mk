# sources.mk - list of source files, relative to $(SRC_DIR).
#
# Leave SRC empty to let the build discover every .c / .S file under
# $(SRC_DIR) automatically. List files explicitly once the project
# grows and you want full control (recommended for libraries).
#
# Example:
#   SRC := \
#   	main.c \
#   	utils/string.c

SRC :=
