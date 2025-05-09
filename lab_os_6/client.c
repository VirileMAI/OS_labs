#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

int main(const int argc, const char *argv[])
{
    DBusConnection *connection; //соединение с D-Bus
    DBusMessage *message;       //сообщение, отправляемое по D-Bus
    DBusMessageIter args;       //итератор по аргументам сообщения

    char wk_name[] = "lab.srv"; //имя службы D-Bus
    
    if (argc < 2) exit(-1);

    //считваем имя файла
    char *fileName = calloc(4096, sizeof(char));
    if (fileName == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(-1);
    }
    sprintf(fileName, "%s", argv[1]);

    //устанавливаем соденинение
    connection = dbus_bus_get(DBUS_BUS_SESSION, NULL);
    if (connection == NULL) {
        fprintf(stderr, "Failed to connect to the D-Bus session bus\n");
        free(fileName);
        exit(-1);
    }

    //создаем сообщение
    message = dbus_message_new_method_call(wk_name, "/", wk_name, "Open");
    if (message == NULL) {
        fprintf(stderr, "Failed to create message\n");
        dbus_connection_unref(connection);
        free(fileName);
        exit(-1);
    }

    //добавляем в сообщение аргумент - имя файла
    dbus_message_iter_init_append(message, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &fileName);

    //отправляем сообщение
    if (!dbus_connection_send(connection, message, NULL)) {
        fprintf(stderr, "Failed to send message\n");
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        free(fileName);
        exit(-1);
    }

    dbus_message_unref(message);
    dbus_connection_unref(connection);
    free(fileName);
    
    return 0;
}