# lab_os
## Лабораторная работа №1. Архиватор
### Задание:
1. Написать программу архиватор на языке C. На вход программе-архиватору 
передается папка с файлами, файлы имеют различное расширение, результатом работы 
программы является один файл, объединяющий все файлы из входной директории, а 
также содержащий заголовок с информацией об именах файлов и их размерах. Учесть 
возможность вложенной структуры папок. 
2. Написать программу разархиватор, разделяющий архивированный файл обратно на 
файлы с учетом структуры папок.
### Вариант:
Добавить для архивирования и разархивирования ввод пароля, который будет 
сохраняться в файле-архиве.
### Литература
Основы программирования в Linux. Автор: Мэтью Нейл, Стоунс Ричард. 
Глава 3. Работа с файлами.
## Лабораторная работа №2. Управление процессом
### Задание:
1. Написать программу «терминал», которая анализирует входную строку и при 
обнаружении ключевых слов «ls», «cat», «nice» и «killall» запускает 
соответствующие процессы.
2. В программе реализовать возможность запуска процессов других программ, 
например, браузера.
3. Написать обработчики сигналов, например, при получении сигнала CTRL+C 
завершить запущенный программой процесс.
4. Заменить системный bash на собственный терминал.
### Вариант:
Программа хранит все открытые процессы. Запустить 3 программы через 
терминал, сигнал CTRL+C должен закрывать программы в 
последовательности FIFO
### Литература
Основы программирования в Linux. Автор: Мэтью Нейл, Стоунс 
Ричард. Глава 11. Процессы и сигналы.