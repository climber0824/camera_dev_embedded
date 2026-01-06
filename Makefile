# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
INCLUDES = -I./include
LDFLAGS = -pthread

# OpenCV configuration
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4 2>/dev/null || pkg-config --cflags opencv)
OPENCV_LIBS = $(shell pkg-config --libs opencv4 2>/dev/null || pkg-config --libs opencv)

# Combine flags
CXXFLAGS += $(OPENCV_CFLAGS)
LDFLAGS += $(OPENCV_LIBS)

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Find all source files (only if they exist)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)

# Check if we have any source files
ifeq ($(strip $(SOURCES)),)
    $(warning No .cpp files found in $(SRC_DIR) directory)
endif

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test_%.o,$(TEST_SOURCES))

# Check if main.cpp exists
MAIN_CPP = $(wildcard $(SRC_DIR)/main.cpp)
MAIN_OBJECT = $(OBJ_DIR)/main.o

# Separate main files from library files
ifneq ($(MAIN_CPP),)
    # main.cpp exists, exclude it from library objects
    LIB_OBJECTS = $(filter-out $(MAIN_OBJECT),$(OBJECTS))
    HAS_MAIN = 1
else
    # No main.cpp, all objects are library objects
    LIB_OBJECTS = $(OBJECTS)
    HAS_MAIN = 0
endif

# Target executables
TARGET = $(BIN_DIR)/surveillance_system
EXAMPLE_TARGETS = $(BIN_DIR)/multi_camera $(BIN_DIR)/motion_recording
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/test_%,$(TEST_SOURCES))

# Build modes
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0 -DDEBUG
else
    CXXFLAGS += -O2 -DNDEBUG
endif

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Default target
.PHONY: all
all: check-sources directories
	@if [ "$(HAS_MAIN)" = "1" ]; then \
		$(MAKE) $(TARGET); \
		echo "$(GREEN)Build complete!$(NC)"; \
		echo "$(GREEN)Main executable: $(TARGET)$(NC)"; \
	else \
		$(MAKE) lib; \
		echo "$(GREEN)Build complete!$(NC)"; \
		echo "$(YELLOW)No main executable created. Library objects built.$(NC)"; \
		echo "$(YELLOW)To create main executable, add main.cpp to $(SRC_DIR)/$(NC)"; \
	fi

# Check if source files exist
.PHONY: check-sources
check-sources:
	@if [ ! -d "$(SRC_DIR)" ]; then \
		echo "$(RED)Error: $(SRC_DIR) directory not found!$(NC)"; \
		exit 1; \
	fi
	@if [ -z "$(SOURCES)" ]; then \
		echo "$(YELLOW)Warning: No .cpp files found in $(SRC_DIR)$(NC)"; \
		echo "$(YELLOW)Looking for source files...$(NC)"; \
		find . -name "*.cpp" -o -name "*.c" 2>/dev/null || true; \
	else \
		echo "$(BLUE)Found $(words $(SOURCES)) source file(s)$(NC)"; \
	fi

# Create necessary directories
.PHONY: directories
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# Build library objects only
.PHONY: lib
lib: $(LIB_OBJECTS)
	@echo "$(GREEN)Library objects built: $(words $(LIB_OBJECTS))$(NC)"

# Main executable (only if main.cpp exists)
$(TARGET): $(OBJ_DIR)/main.o $(LIB_OBJECTS)
	@echo "$(YELLOW)Linking $@...$(NC)"
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "$(GREEN)Created $@$(NC)"

# Generic executable builder for any cpp file with main()
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(LIB_OBJECTS)
	@echo "$(YELLOW)Linking $@...$(NC)"
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "$(GREEN)Created $@$(NC)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile test files
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@echo "$(YELLOW)Compiling test $<...$(NC)"
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build main if it exists
.PHONY: main
main: directories check-sources
	@if [ -f "$(SRC_DIR)/main.cpp" ]; then \
		$(MAKE) $(TARGET); \
	else \
		echo "$(RED)Error: $(SRC_DIR)/main.cpp not found$(NC)"; \
		echo "$(YELLOW)Available source files:$(NC)"; \
		ls -1 $(SRC_DIR)/*.cpp 2>/dev/null || echo "  (none)"; \
		exit 1; \
	fi

# Build a specific executable from a source file
.PHONY: exe
exe:
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Error: Please specify FILE=<name>$(NC)"; \
		echo "$(YELLOW)Example: make exe FILE=main$(NC)"; \
		echo "$(YELLOW)Available files:$(NC)"; \
		ls -1 $(SRC_DIR)/*.cpp 2>/dev/null | sed 's|$(SRC_DIR)/||' | sed 's|.cpp$$||' || echo "  (none)"; \
		exit 1; \
	fi
	@if [ ! -f "$(SRC_DIR)/$(FILE).cpp" ]; then \
		echo "$(RED)Error: $(SRC_DIR)/$(FILE).cpp not found$(NC)"; \
		exit 1; \
	fi
	@$(MAKE) $(BIN_DIR)/$(FILE)

# Build all executables from src/ directory
.PHONY: all-exe
all-exe: directories lib
	@echo "$(BLUE)Building all executables from source files...$(NC)"
	@for src in $(SOURCES); do \
		name=$$(basename $$src .cpp); \
		if [ "$$name" != "main" ]; then \
			if grep -q "int main" $$src 2>/dev/null; then \
				echo "$(YELLOW)Building $$name...$(NC)"; \
				$(MAKE) $(BIN_DIR)/$$name; \
			fi; \
		fi; \
	done
	@if [ -f "$(SRC_DIR)/main.cpp" ]; then \
		$(MAKE) $(TARGET); \
	fi

# Build tests
.PHONY: test
test: directories $(TEST_TARGETS)
	@if [ -z "$(TEST_TARGETS)" ]; then \
		echo "$(YELLOW)No test files found in $(TEST_DIR)$(NC)"; \
	else \
		echo "$(GREEN)Running tests...$(NC)"; \
		for test in $(TEST_TARGETS); do \
			echo "$(YELLOW)Running $$test...$(NC)"; \
			$$test || exit 1; \
		done; \
		echo "$(GREEN)All tests passed!$(NC)"; \
	fi

# Install target
PREFIX ?= /usr/local
.PHONY: install
install: all
	@echo "$(YELLOW)Installing to $(PREFIX)...$(NC)"
	@install -d $(PREFIX)/bin
	@if [ -f "$(TARGET)" ]; then \
		install -m 755 $(TARGET) $(PREFIX)/bin/; \
		echo "$(GREEN)Installed $(TARGET) to $(PREFIX)/bin/$(NC)"; \
	else \
		echo "$(YELLOW)No main executable to install$(NC)"; \
	fi

# Uninstall target
.PHONY: uninstall
uninstall:
	@echo "$(YELLOW)Uninstalling from $(PREFIX)...$(NC)"
	@rm -f $(PREFIX)/bin/surveillance_system
	@echo "$(GREEN)Uninstallation complete!$(NC)"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Clean complete!$(NC)"

# Clean everything
.PHONY: distclean
distclean: clean
	@echo "$(YELLOW)Performing deep clean...$(NC)"
	@find . -name "*.o" -type f -delete 2>/dev/null || true
	@find . -name "*.a" -type f -delete 2>/dev/null || true
	@find . -name "*.so" -type f -delete 2>/dev/null || true
	@find . -name "*~" -type f -delete 2>/dev/null || true
	@echo "$(GREEN)Deep clean complete!$(NC)"

# Display help
.PHONY: help
help:
	@echo "$(GREEN)Available targets:$(NC)"
	@echo "  $(YELLOW)make$(NC) or $(YELLOW)make all$(NC)    - Build library objects"
	@echo "  $(YELLOW)make main$(NC)              - Build main executable (if main.cpp exists)"
	@echo "  $(YELLOW)make lib$(NC)               - Build library objects only"
	@echo "  $(YELLOW)make exe FILE=<name>$(NC)   - Build specific executable"
	@echo "  $(YELLOW)make all-exe$(NC)           - Build all executables with main() functions"
	@echo "  $(YELLOW)make test$(NC)              - Build and run tests"
	@echo "  $(YELLOW)make clean$(NC)             - Remove build artifacts"
	@echo "  $(YELLOW)make distclean$(NC)         - Remove all generated files"
	@echo "  $(YELLOW)make install$(NC)           - Install to $(PREFIX)/bin"
	@echo "  $(YELLOW)make uninstall$(NC)         - Uninstall from $(PREFIX)/bin"
	@echo "  $(YELLOW)make debug$(NC)             - Build with debug symbols"
	@echo "  $(YELLOW)make list-sources$(NC)      - List all source files found"
	@echo "  $(YELLOW)make info$(NC)              - Show build configuration"
	@echo ""
	@echo "$(GREEN)Build options:$(NC)"
	@echo "  $(YELLOW)DEBUG=1$(NC)                - Build with debug symbols"
	@echo "  $(YELLOW)PREFIX=/path$(NC)           - Set installation prefix"
	@echo ""
	@echo "$(GREEN)Examples:$(NC)"
	@echo "  $(YELLOW)make DEBUG=1$(NC)           - Build with debug mode"
	@echo "  $(YELLOW)make exe FILE=Camera$(NC)   - Build Camera.cpp into executable"
	@echo "  $(YELLOW)make all-exe$(NC)           - Build all files containing main()"

# List source files
.PHONY: list-sources
list-sources:
	@echo "$(BLUE)Source files in $(SRC_DIR):$(NC)"
	@if [ -n "$(SOURCES)" ]; then \
		for src in $(SOURCES); do \
			echo "  - $$src"; \
		done; \
	else \
		echo "  $(YELLOW)(none found)$(NC)"; \
	fi
	@echo ""
	@echo "$(BLUE)Test files in $(TEST_DIR):$(NC)"
	@if [ -n "$(TEST_SOURCES)" ]; then \
		for src in $(TEST_SOURCES); do \
			echo "  - $$src"; \
		done; \
	else \
		echo "  $(YELLOW)(none found)$(NC)"; \
	fi

# Debug build
.PHONY: debug
debug:
	@$(MAKE) DEBUG=1

# Show build info
.PHONY: info
info:
	@echo "$(GREEN)Build Configuration:$(NC)"
	@echo "  Compiler: $(CXX)"
	@echo "  C++ Standard: C++11"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Includes: $(INCLUDES)"
	@echo "  Libraries: $(LDFLAGS)"
	@echo "  Source files: $(words $(SOURCES))"
	@echo "  Test files: $(words $(TEST_SOURCES))"
	@echo "  Debug mode: $(DEBUG)"
	@echo ""
	@echo "$(BLUE)Directories:$(NC)"
	@echo "  Source: $(SRC_DIR)"
	@echo "  Tests: $(TEST_DIR)"
	@echo "  Build: $(BUILD_DIR)"
	@echo "  Objects: $(OBJ_DIR)"
	@echo "  Binaries: $(BIN_DIR)"

# Check if OpenCV is installed
.PHONY: check-opencv
check-opencv:
	@echo "$(YELLOW)Checking for OpenCV...$(NC)"
	@pkg-config --exists opencv4 || pkg-config --exists opencv || \
		(echo "$(RED)Error: OpenCV not found!$(NC)" && \
		echo "Install with: sudo apt-get install libopencv-dev" && exit 1)
	@echo "$(GREEN)OpenCV found!$(NC)"
	@echo "  Version: $$(pkg-config --modversion opencv4 2>/dev/null || pkg-config --modversion opencv)"
	@echo "  CFLAGS: $(OPENCV_CFLAGS)"
	@echo "  LIBS: $(OPENCV_LIBS)"

# Static analysis
.PHONY: check
check:
	@command -v cppcheck >/dev/null 2>&1 || \
		(echo "$(YELLOW)cppcheck not found, skipping static analysis$(NC)" && exit 0)
	@echo "$(YELLOW)Running static analysis...$(NC)"
	@cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR) $(TEST_DIR) 2>/dev/null || true

# Format code
.PHONY: format
format:
	@command -v clang-format >/dev/null 2>&1 || \
		(echo "$(RED)clang-format not found$(NC)" && exit 1)
	@echo "$(YELLOW)Formatting code...$(NC)"
	@find $(SRC_DIR) include -name "*.cpp" -o -name "*.h" 2>/dev/null | xargs clang-format -i 2>/dev/null || true
	@find $(TEST_DIR) -name "*.cpp" 2>/dev/null | xargs clang-format -i 2>/dev/null || true
	@echo "$(GREEN)Code formatted!$(NC)"

.PHONY: all main lib exe all-exe test clean distclean help debug info check-opencv check format list-sources install uninstall
