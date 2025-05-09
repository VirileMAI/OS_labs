#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

//функция-обработчик сообщений D-Bus
DBusHandlerResult msghandler(DBusConnection *connection, DBusMessage *message, void *userdata)
{
    char *fileName;
    char assoc[][5][256] = {
        {".txt", "org.gnome.gedit", "/org/gnome/gedit", "org.gtk.Application", "Open"},
        {".pdf", "org.gnome.Evince", "/org/gnome/Evince", "org.gtk.Application", "Open"},
        {".jpg", "org.gnome.eog", "/org/gnome/eog", "org.gtk.Application", "Open"},
        {".png", "org.gnome.eog", "/org/gnome/eog", "org.gtk.Application", "Open"},
        {".doc", "org.libreoffice.LibreOffice", "/org/libreoffice/LibreOffice", "org.freedesktop.Application", "Open"},
    };
    DBusMessageIter args, array, dict;  //аргументы для сообщения

    // Проверяем является ли сообщение вызовом метода CALL
    if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_METHOD_CALL) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    // Получаем аргументы сообщения
    if (!dbus_message_get_args(message, NULL, DBUS_TYPE_STRING, &fileName, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "Failed to get arguments from message.\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    char *uri = calloc(4096, sizeof(char)); 
    if (uri == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    sprintf(uri, "file://%s", fileName);
    printf("Opening %s\n", uri);

    // Находим последнее вхождение '.'
    char *ext = strrchr(uri, '.');

    // Проверяем, что расширение найдено
    if (ext != NULL) {
        message = NULL;
        for (int j = 0; j < sizeof(assoc) / sizeof(assoc[0]); j++) {
            // Сравниваем расширение
            if (strcmp(ext, assoc[j][0]) == 0) {
                // message = dbus_message_new_method_call(assoc[j][1], assoc[j][2], assoc[j][3], assoc[j][4]);
                char *cmd = malloc(sizeof(char) * 1000000);
                strcat(cmd, "evince ");
                strcat(cmd, fileName);
                system(cmd);
                break;
            }
        }
    } else {
        printf("No file extension found\n");
        free(uri);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    // // Проверяем, найдено ли расширение файла
    // if (message == NULL) {
    //     printf("Unknown extension\n");
    //     free(uri);
    //     return DBUS_HANDLER_RESULT_HANDLED;
    // }

    // // Добавление итератора по аргументам в сообщение
    // dbus_message_iter_init_append(message, &args);

    // // Добавление массива строк (uris)
    // dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &array);
    // dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &uri);
    // dbus_message_iter_close_container(&args, &array);

    // // Добавление пустой строки в качестве хинта
    // const char *hint = "";
    // dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &hint);

    // // Добавление пустого словаря
    // dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);
    // dbus_message_iter_close_container(&args, &dict);

    // // Отправка сообщения
    // if (!dbus_connection_send(connection, message, NULL)) {
    //     fprintf(stderr, "Failed to send message.\n");
    //     free(uri);
    //     dbus_message_unref(message);
    //     return DBUS_HANDLER_RESULT_NEED_MEMORY;
    // }

    // free(uri);
    // dbus_message_unref(message);

    return DBUS_HANDLER_RESULT_HANDLED;
}


int main()
{
    DBusConnection *connection;     //соединение с D-Bus
    DBusMessage *message = NULL;    //сообщение
    DBusError error = {};           //структура для хранения информации об ошибках
    DBusDispatchStatus dis;         //переменная для хранения статуса диспетчеризации сообщений
    char wk_name[] = "lab.srv";     //имя службы D-Bus
    int req;

    //запрос имени службы
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    req = dbus_bus_request_name(connection, wk_name, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
    
    //добавляем фильтр для сообщений
    req = dbus_connection_add_filter(connection, msghandler, NULL, NULL);

    while (dbus_connection_read_write_dispatch(connection, -1)) {}

    return 0;
}