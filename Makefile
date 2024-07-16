SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
# objs := $(patsubst %.cpp,./build/%.o, $(notdir $(wildcard ./src/*)))
target := mapmaker

$(target): main.cpp objects.hpp
	g++ -o $(target) $< $(SFML_FLAGS)
clean:
	rm $(target)
	rm ./maps
run: $(target)
	./$(target)
