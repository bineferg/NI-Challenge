CC=g++
CFLAGS=-c -Wall -Iinclude -std=c++11
BUILD_FOLDER=build
FILE = audioeffect
all: clean build audios

audios: effect
	$(CC) -o $(BUILD_FOLDER)/$(FILE) -g $(BUILD_FOLDER)/*.o 

effect: $(FILE).cpp
	$(CC) $(CFLAGS) -o $(BUILD_FOLDER)/$(FILE).o -ggdb $(FILE).cpp

build:
	mkdir -p $(BUILD_FOLDER)

clean:
	rm -rf $(BUILD_FOLDER)

debug:
	gdb ./$(BUILD_FOLDER)/$(FILE)

docker-build:
	docker build -t leak-check:0.1 . 

leak-check: docker-build
	docker run -ti -v "$(shell pwd):/test" leak-check:0.1 bash -c "cd /test/; g++ -std=c++11 -g -o $(FILE) $(FILE).cpp && valgrind --leak-check=full ./$(FILE)"

run:
	./$(BUILD_FOLDER)/$(FILE)
