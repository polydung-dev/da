name=da

# sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
# warnings=-Wall -Wextra -pedantic

CFLAGS=$(warnings) $(sanitize) $(defines) -I./src/ -MMD -g3
LDFLAGS=-L/usr/local/lib/ -Wl,-rpath,/usr/local/lib/ $(sanitize)
LDLIBS=-lGL -lglfw -lglad -lm

sources=$(wildcard src/*.c) $(wildcard src/*/*.c)
headers=$(wildcard src/*.h) $(wildcard src/*/*.h)
objects=$(patsubst src/%,build/%,$(sources:.c=.o))
depends=$(objects:.o=.d)

build_dirs=out/ build/
dirs=$(filter-out build/,$(sort $(dir $(objects))))

###############################################################################

.PHONY:
all: $(build_dirs) $(dirs) out/$(name)

.PHONY:
clean:
	-rm -r $(filter-out build/ext/,$(dirs))
	-rm build/main.o build/main.d

.PHONY:
realclean:
	-rm -r build/

.PHONY:
install:
	mkdir -p ~/.local/bin/
	install -vm 0755 out/$(name) ~/.local/bin/$(name)
	mkdir -p ~/.local/share/$(name)/
	@for item in $$(find data -type f); \
		do install -Dvm 0755 $${item} ~/.local/share/$(name)/$${item#data/}; \
	done

.PHONY:
uninstall:
	-rm ~/.local/bin/$(name)
	-rm -r ~/.local/share/$(name)/

%/:
	mkdir -p $@

out/$(name): $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(depends)

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

###############################################################################

.PHONY:
glad:
	-mkdir -p ./build/glad
	-mkdir -p ./out/glad
	glad --api=gl:core=4.6 --out-path ./build/glad
	$(CC) -o ./out/glad/libglad.so.4.6 ./build/glad/src/gl.c -fPIC -shared \
		-Wl,-soname,libglad.so.4 -I./build/glad/include -g3 -O3

.PHONY:
install_glad:
	install -m 0755 out/glad/libglad.so.4.6 /usr/local/lib/libglad.so.4.6
	ln -sf libglad.so.4 /usr/local/lib/libglad.so
	ln -sf libglad.so.4.6 /usr/local/lib/libglad.so.4
	install -m 0755 -d ./build/glad/include/glad /usr/local/include/glad
	install -m 0755 ./build/glad/include/glad/gl.h /usr/local/include/glad/gl.h
	install -m 0755 -d ./build/glad/include/KHR /usr/local/include/KHR
	install -m 0755 ./build/glad/include/KHR/khrplatform.h \
		/usr/local/include/KHR/khrplatform.h
	ldconfig -n /usr/local/lib

###############################################################################

.PHONY:
memcheck:
	@echo ---
	valgrind --tool=memcheck --show-leak-kinds=all out/$(name)

.PHONY:
cachegrind:
	@echo ---
	valgrind --tool=cachegrind out/$(name)

.PHONY:
callgrind:
	@echo ---
	valgrind --tool=callgrind out/$(name)
