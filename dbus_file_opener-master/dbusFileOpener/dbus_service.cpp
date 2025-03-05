#include "dbus_service.h"

// Конструктор класса DBusService
// Инициализирует словарь fileApplications, который сопоставляет расширения файлов с соответствующими приложениями.
DBusService::DBusService()
{
    fileApplications = {
        {"txt", "gedit"},              // .txt файлы открываются с помощью gedit
        {"c", "gedit"},                // .c файлы открываются с помощью gedit
        {"jpg", "eog"},                // .jpg файлы открываются с помощью eog (Eye of GNOME)
        {"png", "eog"},                // .png файлы открываются с помощью eog
        {"gif", "eog"},                // .gif файлы открываются с помощью eog
        {"mp3", "totem"},              // .mp3 файлы открываются с помощью totem (GNOME Videos)
        {"wav", "totem"},              // .wav файлы открываются с помощью totem
        {"mp4", "vlc.exe"},            // .mp4 файлы открываются с помощью VLC Media Player
        {"avi", "vlc.exe"},            // .avi файлы открываются с помощью VLC
        {"mov", "quicktime.exe"},      // .mov файлы открываются с помощью QuickTime
        {"pdf", "evince"}
    };
}

// Функция для получения имени приложения по расширению файла
// filePath - путь к файлу
QString getAppName(const QString& filePath)
{
    QString fileExtension;

    // Извлекаем расширение файла (последовательность символов после последней точки)
    for (int i = filePath.length() - 1; i >= 0; i--)
    {
        if (filePath[i] == '.')
        {
            // Остановка, когда достигнута точка
            break;
        }
        // Добавляем символ к расширению файла (в обратном порядке)
        fileExtension += filePath[i];
    }

    // Переворачиваем строку, так как символы были добавлены в обратном порядке
    QString reversedStr;
    for (int i = fileExtension.length() - 1; i >= 0; i--) {
        reversedStr += fileExtension[i];
    }
    fileExtension = reversedStr;

    QString application;
    // Создаем временный объект DBusService для доступа к словарю fileApplications
    DBusService temp;
    QMap<QString, QString> fileApplications = temp.get();

    // Проверяем, есть ли такое расширение файла в словаре
    if (fileApplications.count(fileExtension) > 0)
    {
        // Если расширение найдено, получаем соответствующее приложение
        application = fileApplications[fileExtension];
        qDebug() << "Открываем файл с помощью приложения: " << application << '\n';
    }
    else
    {
        // Если расширение неизвестно, выводим сообщение об этом
        qDebug() << "Неизвестный тип файла" << '\n';
    }

    return application;
}

// Метод для запуска приложения, соответствующего расширению файла
// filePath - путь к файлу
void DBusService::launchApplication(const QString& filePath)
{
    // Определяем имя приложения по расширению файла
    QString appName = getAppName(filePath);

    // Создаем список аргументов, который содержит путь к файлу (передается приложению при запуске)
    QStringList appArgs = {filePath};

    // Запускаем приложение асинхронно, передавая путь к файлу как аргумент
    QProcess::startDetached(appName, appArgs);
}

// Функция для отправки сообщения через D-Bus для запуска приложения
// filePath - путь к файлу, который требуется открыть
void sendMessageToDBus(const QString& filePath)
{
    // Подключаемся к сессионной шине D-Bus
    QDBusConnection bus = QDBusConnection::sessionBus();

    // Создаем сообщение для отправки вызова метода через D-Bus
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.example.Service",               // Имя целевого D-Bus сервиса (service name)
        "/interfaces",                       // Объектный путь, соответствующий методу
        "local.dbusFileOpener.DBusService",  // Интерфейс D-Bus
        "launchApplication"                  // Метод интерфейса, который будет вызван
    );

    // Добавляем путь к файлу (filePath) как аргумент для метода
    message << filePath;

    // Отправляем сообщение и ожидаем ответ
    QDBusMessage reply = bus.call(message);

    // Проверяем, успешно ли отправлено сообщение
    if (reply.type() == QDBusMessage::ReplyMessage) {
        qDebug() << "Сообщение успешно отправлено";
    } else {
        // Если произошла ошибка, выводим информацию об ошибке
        qDebug() << "Ошибка при отправке сообщения:" << reply.errorName() << reply.errorMessage();
    }
}
