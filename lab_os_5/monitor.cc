#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <atomic>
#include <random>

// Глобальные переменные
std::atomic<bool> isConfirmed{false}; // Флаг, указывающий на то, что Полуэкт дозвонился и получил подтверждение
std::string confirmedRelative; // Имя первого родственника, которому дозвонился Полуэкт
std::mutex confirmationMutex; // Мьютекс для синхронизации доступа к подтверждению
std::condition_variable firstConfirmationCV; // Условная переменная для уведомления первого получателя
std::condition_variable otherConfirmationCV; // Условная переменная для уведомления остальных родственников
std::mutex relativeMutex; // Мьютекс для синхронизации звонков родственников друг другу

bool firstConfirmationNotified = false; // Флаг для проверки уведомления первого получателя
bool allConfirmedNotified = false; // Флаг для уведомления остальных родственников

// Функция, моделирующая звонок Полуэкта одному из родственников
void poluekt(const std::vector<std::string>& relatives) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Имитация задержки перед звонком
    std::cout << "Полуэкт пытается дозвониться...\n";

    // Выбор случайного родственника для звонка
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, relatives.size() - 1);
    int chosenRelativeIndex = distr(gen);

    // Проверка и установка флага подтверждения
    if (!isConfirmed.exchange(true)) {
        confirmedRelative = relatives[chosenRelativeIndex]; // Сохраняем имя первого получателя
        std::cout << "Полуэкт дозвонился до " << confirmedRelative << " и подтвердил, что он на работе.\n";

        // Уведомление первого получателя
        std::lock_guard<std::mutex> lock(confirmationMutex);
        firstConfirmationNotified = true;
        firstConfirmationCV.notify_one(); // Уведомляем условную переменную
    }
}

// Функция для имитации звонков между родственниками и ожидания звонка от Полуэкта
void relative(const std::string& name, const std::vector<std::string>& relatives) {
    // Ожидание подтверждения от Полуэкта
    while (!isConfirmed) {
        std::unique_lock<std::mutex> relativeLock(relativeMutex); // Блокируем мьютекс на время звонков между родственниками
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Имитация задержки

        // Пока Полуэкт не дозвонился, родственники звонят друг другу
        if (!isConfirmed) {
            // Создаем список из других родственников (исключаем себя)
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(0, relatives.size() - 2);
            std::vector<std::string> otherRelatives;
            for (const auto& rel : relatives) {
                if (rel != name) otherRelatives.push_back(rel);
            }
            std::string chosenRelative = otherRelatives[distr(gen)]; // Выбор случайного родственника для звонка
            
            std::cout << name << " пытается дозвониться до " << chosenRelative << "...\n";
            std::cout << name << " поговорил с " << chosenRelative << " и узнал, что Полуэкт пока не звонил.\n";
        }
    }

    // Если Полуэкт дозвонился, родственники получают подтверждение
    if (name == confirmedRelative) {
        // Если это первый получатель звонка от Полуэкта
        std::unique_lock<std::mutex> lock(confirmationMutex);
        firstConfirmationCV.wait(lock, [] { return firstConfirmationNotified; }); // Ожидание уведомления от Полуэкта
        std::cout << name << " (получатель звонка от Полуэкта) первым получил подтверждение, что с Полуэктом всё в порядке.\n";

        // Уведомляем остальных родственников
        allConfirmedNotified = true;
        otherConfirmationCV.notify_all(); // Уведомление для остальных родственников
    } else {
        // Если это не первый получатель, ждем уведомления от первого получателя
        std::unique_lock<std::mutex> lock(confirmationMutex);
        otherConfirmationCV.wait(lock, [] { return allConfirmedNotified; });
        std::cout << name << " получил подтверждение, что с Полуэктом всё в порядке.\n";
    }
}

int main() {
    // Список родственников
    std::vector<std::string> relatives = {"Бабушка А", "Бабушка Б", "Мама", "Девушка 1", "Девушка 2"};
    
    // Запуск потока Полуэкта
    std::thread poluektThread(poluekt, std::ref(relatives));
    
    // Запуск потоков для каждого родственника
    std::vector<std::thread> relativeThreads;
    for (const auto& name : relatives) {
        relativeThreads.emplace_back(relative, name, std::ref(relatives));
    }
    
    // Ожидание завершения всех потоков
    poluektThread.join();
    for (auto& t : relativeThreads) {
        t.join();
    }

    std::cout << "Все участники получили подтверждение. Завершение работы.\n";
    return 0;
}
