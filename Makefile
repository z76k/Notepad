CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = 

# Check for pkg-config
ifneq ($(shell which pkg-config),)
    SDL2_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf)
    SDL2_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
else
    # Fallback for systems without pkg-config (e.g. some macOS setups without it)
    # Assumes headers/libs are in standard search paths
    SDL2_CFLAGS = -I/usr/include/SDL2 -D_REENTRANT
    SDL2_LIBS = -lSDL2 -lSDL2_ttf
endif

CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS += $(SDL2_LIBS)

TARGET = NotepadInc
SRC_DIRS = src src/States src/Utils
BUILD_DIR = build

# Recursively find sources
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# Create object file paths (preserving structure in build dir is tricky with simple makefile, 
# let's just use flattened structure or mkdir -p)
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Default Host Build
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = 

# Check for pkg-config for Host
ifneq ($(shell which pkg-config),)
    SDL2_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf)
    SDL2_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
else
    SDL2_CFLAGS = -I/usr/include/SDL2 -D_REENTRANT
    SDL2_LIBS = -lSDL2 -lSDL2_ttf
endif

CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS += $(SDL2_LIBS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Compilation rule that handles subdirectories
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Miyoo Mini Cross-Compilation Target
# Usage: make miyoo
miyoo: CXX = arm-linux-gnueabihf-g++
miyoo: CXXFLAGS = -std=c++17 -Wall -O3 -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4 -DMIYOO
miyoo: LDFLAGS = -lSDL2 -lSDL2_ttf -lm -ldl -lpthread -lrt
miyoo: clean $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean miyoo
