#OBJS specifies which files to compile as part of the project
OBJS = game/main.o game/ui.o game/editor.o engine/glad.o engine/shader.o engine/renderer.o engine/importer.o engine/dict.o engine/debug.o

#CC specifies which compiler we're using
CC = gcc -g -std=c99 `pkg-config --cflags glfw3`

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
#  COMPILER_FLAGS = -w
#
#  #LINKER_FLAGS specifies the libraries we're linking against

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = game/microdrag
LINKER_FLAGS = `pkg-config --static --libs glfw3`

#This is the target that compiles our executable

$(OBJ_NAME): $(OBJS)
	$(CC) -o $@ $^ $(LINKER_FLAGS)

clean:
	rm -f $(OBJ_NAME)
