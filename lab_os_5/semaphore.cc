#include <iostream>
#include <thread>
#include <semaphore.h>
#include <vector>
#include <string>
#include <atomic>
#include <random>

std::atomic<bool> isConfirmed{false}; // Флаг, указывающий на получение подтверждения
std::string confirmedRelative; // Имя того, кому дозвонился Полуэкт
sem_t confirmationSemaphore; // Семафор для подтверждения
sem_t relativeSemaphore; // Семафор для общения родственников

void poluekt(const std::vector<std::string>& relatives) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Задержка перед звонком Полуэкта
    std::cout << "Полуэкт пытается дозвониться...\n";

    // Выбор случайного родственника для звонка
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, relatives.size() - 1);
    int chosenRelativeIndex = distr(gen);
    
    // Полуэкт дозванивается до выбранного родственника
    if (!isConfirmed.exchange(true)) {
        confirmedRelative = relatives[chosenRelativeIndex];
        std::cout << "Полуэкт дозвонился до " << confirmedRelative << " и подтвердил, что он на работе.\n";
        sem_post(&confirmationSemaphore); // Сообщить выбранному родственнику напрямую
    }
}

void relative(const std::string& name, const std::vector<std::string>& relatives) {
    if (name == confirmedRelative) {
        // Если это тот, кому Полуэкт позвонил первым, сразу получает подтверждение
        sem_wait(&confirmationSemaphore);
        std::cout << name << " получил подтверждение от Полуэкта, что с ним всё в порядке.\n";
        return; // Завершение работы потока, чтобы не участвовать в дальнейших подтверждениях
    }
    
    while (!isConfirmed) {
        sem_wait(&relativeSemaphore); // Ждать возможности для звонка другому родственнику
        
        // Выбор случайного собеседника из остальных родственников
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, relatives.size() - 2);
        std::vector<std::string> otherRelatives;
        for (const auto& rel : relatives) {
            if (rel != name) otherRelatives.push_back(rel); // Исключаем себя из списка
        }
        std::string chosenRelative = otherRelatives[distr(gen)];
        
        std::cout << name << " пытается дозвониться до " << chosenRelative << "...\n";
        std::cout << name << " поговорил с " << chosenRelative << " и узнал, что Полуэкт пока не звонил.\n";

        // Передача возможности звонка следующему родственнику
        sem_post(&relativeSemaphore);
        
        // Пауза перед следующей попыткой общения
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Дождаться подтверждения от других участников, если это не первый позвонивший Полуэкту
    sem_wait(&confirmationSemaphore);
    std::cout << name << " получил подтверждение, что с Полуэктом всё в порядке.\n";
    sem_post(&confirmationSemaphore); // Передать подтверждение другим
}

int main() {
    sem_init(&confirmationSemaphore, 0, 0); // Инициализация семафора подтверждения
    sem_init(&relativeSemaphore, 0, 1); // Инициализация семафора общения родственников (1, чтобы начать цепочку)

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
    
    // Уничтожение семафоров
    sem_destroy(&confirmationSemaphore);
    sem_destroy(&relativeSemaphore);

    std::cout << "Все участники получили подтверждение. Завершение работы.\n";
    return 0;
}
