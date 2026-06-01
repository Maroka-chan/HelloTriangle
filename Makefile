CC	:= clang
CFLAGS 	:= -O2
LDFLAGS := -lglfw -lvulkan -ldl -lpthread

# Window system backend. Wayland-only by default; build with X11 support
# as well via `make X11=1`.
X11 ?= 0
ifeq ($(X11),1)
LDFLAGS += -lX11 -lXxf86vm -lXrandr -lXi
endif

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

