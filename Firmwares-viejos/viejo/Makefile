# Make es una serie de reglas: Hay que prestar atencion al formato (espacios, tabs, etc)
#
# objetivo:[espacio]dependencies
# [tab]comando

# Compilador
CC := avr-gcc

CFLAGS := -mmcu=atmega16 -Wall

# Nombre del ejecutable → carga en APP el nombre de la carpeta donde esta el makefile
APP := $(notdir $(shell pwd))

# variables: ruta a los archivos fuentes
SRC_PATH := src

# ruta a los includes
INC_PATH := inc

# ruta de salida
OUT_PATH := out

# ruta a los objetos
OBJ_PATH := $(OUT_PATH)/obj


# Agregamos ruta a archivos.c
vpath %.c src

# Agregamos ruta a archivos.o
vpath %.o out/obj

#listado de archivos.c
SRC_FILES := $(wildcard $(SRC_PATH)/* .c)

#listado de archivos.o
OBJ_FILES := $(subst $(SRC_PATH),$(OBJ_PATH),$(SRC_FILES:.c=.o))

#listado de archivos.o sin ruta
OBJS := $(notdir $(OBJ_FILES))



# Regla generica de compiladion. Si necesitas un .o buscá un .c
# $< es la variable con la dependencias
# $@ es la variable con el objetivo

# -I indica donde está la carpeta con los includes

%.o: %.c
	$(CC) $(CFLAGS) -I $(INC_PATH) -c $< -o $(OBJ_PATH)/$@


#Regla de linkeo → Los objetos van sin path, solo el nombre

$(OUT_PATH)/$(APP): $(OBJS)
	$(CC) $(OBJ_FILES) -o $(OUT_PATH)/$(APP)

#Regla de ejecucion

run: $(OUT_PATH)/$(APP)
	@./$(OUT_PATH)/$(APP)




# Regla para borrar los archivos generados
clean:
	rm  $(OUT_PATH)/$(APP)
	rm  $(OBJ_PATH)/*.o


# Para que muestre parametros
info:
	@echo SRC_FILES: $(SRC_FILES)
	@echo OBJ_FILES: $(OBJ_FILES)
	@echo OBJS: $(OBJS)
	@echo APP: $(APP)
