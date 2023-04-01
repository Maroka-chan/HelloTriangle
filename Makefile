CC	:= clang
CFLAGS 	:= -O2
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: main.c **/*.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $? $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest

debug: DEBUG := -g -DDEBUG
debug: VulkanTest
	./VulkanTest

symbols: DEBUG := -g
symbols: clean VulkanTest

