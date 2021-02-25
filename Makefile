# Makefile for background generator

# defines the compiler and flags
COMPILER = g++

# the linker flags for ImageMagick
LINKER_FLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`

FILES = generate.cpp delaunay.cpp

generate: $(FILES)
	$(COMPILER) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(FILES) -o generate


.PHONY: clean
clean:
	-rm -f *.o *~ generate
