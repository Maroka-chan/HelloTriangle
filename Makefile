CFLAGS = -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread

VulkanTest: main.c
	clang $(CFLAGS) $(DEBUG) -o VulkanTest main.c debug.c $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest

debug: DEBUG = -DDEBUG
debug: VulkanTest
	./VulkanTest

