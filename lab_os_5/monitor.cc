#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <random>
#include <mutex>
#include <condition_variable>

std::atomic<bool> isConfirmed{false};       // Флаг, указывающий на получение подтверждения
std::string confirmedRelative;              // Имя того, кому дозвонился Полуэкт

class Monitor {
public:
    void notifyConfirmation(const std::string& relative) {
        std::unique_lock<std::mutex> lock(mtx);
        confirmedRelative = relative;
        isConfirmed = true;
        std::cout << "Полуэкт дозвонился до " << confirmedRelative << " и подтвердил, что он на работе.\n";
        cv.notify_all(); // Оповещаем всех, что подтверждение получено
    }

    void waitForConfirmation(const std::string& name) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Если это тот, кому позвонил Полуэкт
        if (name == confirmedRelative) {
            std::cout << name << " получил подтверждение от Полуэкта, что с ним всё в порядке.\n";
            return;
        }

        // Ожидание подтверждения
        cv.wait(lock, [] { return isConfirmed.load(); });
        std::cout << name << " получил подтверждение, что с Полуэктом всё в порядке.\n";
    }

    void communicate(const std::string& caller, const std::string& receiver) {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << caller << " позвонил " << receiver << " и обсудил, что Полуэкт пока не звонил.\n";
    }

private:
    std::mutex mtx;                    // Мьютекс для защиты данных монитора
    std::condition_variable cv;        // Условная переменная для ожидания и уведомления
};

void poluekt(const std::vector<std::string>& relatives, Monitor& monitor) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Задержка перед звонком Полуэкта
    std::cout << "Полуэкт пытается дозвониться...\n";

    // Выбор случайного родственника для звонка
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, relatives.size() - 1);
    int chosenRelativeIndex = distr(gen);

    // Полуэкт дозванивается до выбранного родственника
    monitor.notifyConfirmation(relatives[chosenRelativeIndex]);
}

void relative(const std::string& name, const std::vector<std::string>& relatives, Monitor& monitor) {
    if (name == confirmedRelative) {
        monitor.waitForConfirmation(name); // Тот, кому позвонил Полуэкт, сразу получает подтверждение
        return;
    }
    
    while (!isConfirmed) {
        // Выбор случайного собеседника для общения
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, relatives.size() - 2);
        std::vector<std::string> otherRelatives;
        for (const auto& rel : relatives) {
            if (rel != name) otherRelatives.push_back(rel); // Исключаем себя из списка
        }
        std::string chosenRelative = otherRelatives[distr(gen)];

        // Симуляция звонка
        monitor.communicate(name, chosenRelative);
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Пауза перед следующим звонком

        if (isConfirmed) break; // Завершение при подтверждении
    }

    monitor.waitForConfirmation(name); // Получение подтверждения, если не первый позвонивший Полуэкту
}

int main() {
    Monitor monitor; // Объект монитора для координации
    std::vector<std::string> relatives = {"Бабушка А", "Бабушка Б", "Мама", "Девушка 1", "Девушка 2"};

    // Запуск потока Полуэкта
    std::thread poluektThread(poluekt, std::ref(relatives), std::ref(monitor));

    // Запуск потоков для каждого родственника
    std::vector<std::thread> relativeThreads;
    for (const auto& name : relatives) {
        relativeThreads.emplace_back(relative, name, std::ref(relatives), std::ref(monitor));
    }

    // Ожидание завершения всех потоков
    poluektThread.join();
    for (auto& t : relativeThreads) {
        t.join();
    }

    std::cout << "Все участники получили подтверждение. Завершение работы.\n";
    return 0;
}
