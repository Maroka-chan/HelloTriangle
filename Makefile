CFLAGS = -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread

VulkanTest: main.c
	gcc $(CFLAGS) -o VulkanTest main.c debug.c $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest

