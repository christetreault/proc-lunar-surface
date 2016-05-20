.DEFAULT_GOAL := all

PROG_NAME = final
SRC_DIR = src
RES_DIR = res
SHADER_DIR = shader

CC_FLAGS = -g -Wall -Wconversion -std=c++14 -Ofast
OBJ_FILES = main.o Window.o OBJObject.o Shader.o Light.o \
		SkyBox.o SceneGraph.o Texture.o Scene.o \
		Util.o
CC = g++

PKG_CONFIG_LIBS = gl glu glew glfw3
MANUAL_LIBS = -lSOIL
LIBS = $(MANUAL_LIBS) $(shell pkg-config --libs $(PKG_CONFIG_LIBS))

PKG_CONFIG_INCLUDE = gl glu glew
MANUAL_INCLUDE =
INCLUDE = $(MANUAL_INCLUDE) $(shell pkg-config --cflags $(PKG_CONFIG_INCLUDE))

all : build

build : $(OBJ_FILES)
	$(CC) -o $(PROG_NAME) $(OBJ_FILES) $(CC_FLAGS) $(INCLUDE) $(LIBS)

main.o : $(SRC_DIR)/main.cpp $(SRC_DIR)/Window.hpp
	$(CC) -c $(SRC_DIR)/main.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Window.o : $(SRC_DIR)/Window.cpp $(SRC_DIR)/Window.hpp  \
		$(SRC_DIR)/SceneGraph.hpp $(SRC_DIR)/Scene.hpp \
		$(SRC_DIR)/Util.hpp
	$(CC) -c $(SRC_DIR)/Window.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

OBJObject.o : $(SRC_DIR)/OBJObject.cpp $(SRC_DIR)/OBJObject.hpp \
		$(SRC_DIR)/Vertex.hpp $(SRC_DIR)/Shader.hpp \
		$(SRC_DIR)/Material.hpp
	$(CC) -c $(SRC_DIR)/OBJObject.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Shader.o : $(SRC_DIR)/Shader.cpp $(SRC_DIR)/Shader.hpp
	$(CC) -c $(SRC_DIR)/Shader.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Texture.o : $(SRC_DIR)/Texture.cpp $(SRC_DIR)/Texture.hpp
	$(CC) -c $(SRC_DIR)/Texture.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Light.o : $(SRC_DIR)/Light.cpp $(SRC_DIR)/Light.hpp \
		$(SRC_DIR)/SceneGraph.hpp
	$(CC) -c $(SRC_DIR)/Light.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

SkyBox.o : $(SRC_DIR)/SkyBox.cpp $(SRC_DIR)/SkyBox.hpp \
		$(SRC_DIR)/SceneGraph.hpp $(SRC_DIR)/Shader.hpp \
		$(SRC_DIR)/OBJObject.hpp $(SRC_DIR)/Texture.hpp
	$(CC) -c $(SRC_DIR)/SkyBox.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

SceneGraph.o : $(SRC_DIR)/SceneGraph.cpp $(SRC_DIR)/SceneGraph.hpp \
		$(SRC_DIR)/OBJObject.hpp $(SRC_DIR)/Material.hpp \
		$(SRC_DIR)/Shader.hpp
	$(CC) -c $(SRC_DIR)/SceneGraph.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Scene.o : $(SRC_DIR)/Scene.cpp $(SRC_DIR)/Scene.hpp \
		$(SRC_DIR)/Material.hpp $(SRC_DIR)/Light.hpp \
		$(SRC_DIR)/OBJObject.hpp $(SRC_DIR)/SceneGraph.hpp \
		$(SRC_DIR)/Shader.hpp $(SRC_DIR)/Texture.hpp
	$(CC) -c $(SRC_DIR)/Scene.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

Util.o : $(SRC_DIR)/Util.cpp $(SRC_DIR)/Util.hpp
	$(CC) -c $(SRC_DIR)/Util.cpp $(CC_FLAGS) $(INCLUDE) $(LIBS)

clean :
	$(RM) *.o
	$(RM) core
	$(RM) *~
	$(RM) $(PROG_NAME)
	$(RM) $(SRC_DIR)/*~
	$(RM) $(RES_DIR)/*~
	$(RM) $(SHADER_DIR)/*~
