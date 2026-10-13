BIN_PATH := ./a.out

.PHONY: build
build:
	g++ \
		main.cpp \
		program.cpp \
		utils.cpp \
		-ldl \
		$$(pkg-config --cflags --libs glew) \
		$$(pkg-config --cflags --libs glfw3) \
		-o $(BIN_PATH)

.PHONY: run
run:
	$(BIN_PATH)

.PHONY: clean
clean:
	rm -f $(BIN_PATH)

.PHONY: install-deps
install-deps:
	sudo apt install libglfw3-dev libglew-dev
