#OBJS specifies which files to compile as part of the project
OBJS = main.c glad.c shader.c renderer.c importer.c vertex_hashtable.c

#CC specifies which compiler we're using
CC = gcc -g -std=c99 `pkg-config --cflags glfw3`

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
#  COMPILER_FLAGS = -w
#
#  #LINKER_FLAGS specifies the libraries we're linking against

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = microdrag
LINKER_FLAGS = `pkg-config --static --libs glfw3`

#This is the target that compiles our executable

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) -o $(OBJ_NAME) $(LINKER_FLAGS)

clean:
	rm -f $(OBJ_NAME)
