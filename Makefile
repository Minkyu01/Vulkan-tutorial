include .env

# CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include 
# LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

GLFW_PATH := $(shell brew --prefix glfw)
GLM_PATH := $(shell brew --prefix glm)
CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include -I$(GLFW_PATH)/include -I$(GLM_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib -L$(GLFW_PATH)/lib -lglfw -lvulkan


a.out: *.cpp 
	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS)

# a.out: main.cpp
# 	g++ $(CFLAGS) -o a.out main.cpp $(LDFLAGS)
# a.out: *.cpp *.hpp
# 	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS)

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out