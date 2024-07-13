SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
# objs := $(patsubst %.cpp,./build/%.o, $(notdir $(wildcard ./src/*)))
target := mapmaker

$(target): main.cpp
	g++ -o $(target) $^ $(SFML_FLAGS)
clean:
	rm $(target)
	rm ./maps
run: $(target)
	rm ./maps
	./$(target)
