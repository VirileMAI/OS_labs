#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>
#include <chrono>
#include <string>
#include <random>

std::vector<std::string> roles = {"Полуэкт", "Бабушка1", "Бабушка2", "Мама", "Девушка1"};
std::random_device rd;
std::mt19937 gen(rd());

struct Phone {
    std::counting_semaphore<1> access; // Семафор для доступа к телефону
    Phone() : access(1) {}
};

std::vector<Phone> phones(roles.size());

void log(const std::string &message) {
    std::cout << message << std::endl;
}

void poluekt() {
    std::uniform_int_distribution<> distr(1, roles.size() - 1);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Имитирует работу Полуэкта

        log("Полуэкт пытается позвонить...");

        int contactIndex = distr(gen);
        std::string contact = roles[contactIndex];

        log("Полуэкт звонит " + contact + ".");

        // Полуэкт пытается захватить оба телефона: свой и контактного собеседника
        if (phones[0].access.try_acquire()) { // Захватывает свой телефон
            if (phones[contactIndex].access.try_acquire()) { // Захватывает телефон собеседника
                log("Полуэкт получил подтверждение от " + contact + ".");
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Имитирует разговор
                phones[contactIndex].access.release(); // Освобождает телефон собеседника
                phones[0].access.release(); // Освобождает свой телефон
                break;
            } else {
                log("Телефон " + contact + " занят. Полуэкт пробует снова...");
                phones[0].access.release(); // Освобождает свой телефон
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

void relative(const std::string &role, int index) {
    std::uniform_int_distribution<> distr(1, roles.size() - 1);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        log(role + " пытается дозвониться...");

        int contactIndex = distr(gen);
        if (roles[contactIndex] == role) {
            continue; // Пропускаем самозвонок
        }

        std::string contact = roles[contactIndex];
        log(role + " звонит " + contact + ".");

        // Родственник пытается захватить оба телефона: свой и телефон собеседника
        if (phones[index].access.try_acquire()) { // Захватывает свой телефон
            if (phones[contactIndex].access.try_acquire()) { // Захватывает телефон собеседника
                log(contact + " получил подтверждение от " + role + ".");
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Имитирует разговор
                phones[contactIndex].access.release(); // Освобождает телефон собеседника
                phones[index].access.release(); // Освобождает свой телефон
                break;
            } else {
                log("Телефон " + contact + " занят. " + role + " ожидает возможности позвонить.");
                phones[index].access.release(); // Освобождает свой телефон
            }
        }
    }
}

int main() {
    std::thread poluektThread(poluekt);

    std::vector<std::thread> relatives;
    relatives.push_back(std::thread(relative, std::string("Бабушка1"), 1));
    relatives.push_back(std::thread(relative, std::string("Бабушка2"), 2));
    relatives.push_back(std::thread(relative, std::string("Мама"), 3));
    relatives.push_back(std::thread(relative, std::string("Девушка1"), 4));

    poluektThread.join();
    for (auto &th : relatives) th.join();

    return 0;
}
