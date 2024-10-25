include .env

# CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
# LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
GLFW_PATH := $(shell brew --prefix glfw)
GLM_PATH := $(shell brew --prefix glm)
CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include -I$(GLFW_PATH)/include -I$(GLM_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib -L$(GLFW_PATH)/lib -lglfw -lvulkan


# create list of all spv files and set as dependency
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp
	g++ $(CFLAGS) /opt/homebrew/lib/libglfw.3.4.dylib /Users/miyu/VulkanSDK/1.3.290.0/macOS/lib/libvulkan.1.dylib /Users/miyu/VulkanSDK/1.3.290.0/macOS/lib/libvulkan.1.3.290.dylib -o $(TARGET) *.cpp $(LDFLAGS)
# g++ $(CFLAGS)  -o $(TARGET) *.cpp $(LDFLAGS)

# make shader targets
%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f *.spv
