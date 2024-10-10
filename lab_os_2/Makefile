TARGET = my_terminal
CC = gcc

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

all: clean $(TARGET)

install: $(TARGET)
	sudo cp $^ /usr/bin/
	echo /usr/bin/$^ | sudo tee -a /etc/shells
	sudo chsh -s /usr/bin/$^

uninstall:
	sudo rm -rf /usr/bin/$(TARGET)


$(TARGET): dir $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(PREF_OBJ)%.o: $(PREF_SRC)%.c
	$(CC) -c $< -o $@

clean: 
	rm -rf $(TARGET) $(PREF_OBJ)*.o

dir:
	mkdir -p obj/