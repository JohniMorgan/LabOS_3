#pragma comment ( lib, "Winmm.Lib")
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string>

using namespace std;

void cleanInStream() {
    cin.clear();
    if (char(cin.peek()) == '\n') cin.ignore();
}

const int N = 100000000; //Точность числа пи
const DWORD iteration_block = 305040; //Количество блоков итераций на поток

//Вычисляемое значение Пи
double pi_num = 0; //Вычисляемое число Пи
DWORD offset = 0; //Смещение потоков (отслеживание итераций)
//Т.к. по завершении функции вызова поток завершается
//Будем использовать критическую секцию
//Для доступа к общему числу пи а так же для приостновки потоков
LPCRITICAL_SECTION crit_sec = new CRITICAL_SECTION;

DWORD WINAPI ThreadCalculate(LPVOID arg) {
    DWORD start = *((DWORD*)arg);
    DWORD end = start + iteration_block;

    while (start < N) {
        double part_of_pi_num = 0; //Часть числа пи, считаемая на данном блоке
        int i = start;
        while (i < N && i < end) {
            double x = ((i + 0.5) / N); // Непосредственный рассчёт
            part_of_pi_num += (4 / (1 + x * x)); //Представленной формулы
            i++;
        }
        //Блок итераций окончен, приостанавливаем поток
        //Все завершённые потоки будут блокироваться текущим
        EnterCriticalSection(crit_sec);
        start = offset; //Сдвигаемся на край посчитанного участка
        end = start + iteration_block; //Сдвигаем так же конец блока
        offset += iteration_block; //Увеличиваем сдвиг, как будто мы уже этим потоком посчитали
        pi_num += part_of_pi_num / N; //Добавляем к числу пи посчитанную часть
        //Блок итераций "выделен", выводим поток из секции
        //Давая возможность продолжать работу и выдать итерации следующему потоку
        LeaveCriticalSection(crit_sec); 
    }
    return 0; //Поток завершится и перейдёт в сигнальное состояние
}

int InputAsNumber() {
    string user;
    int in = -1;
    bool flag = true;
    do {
        cout << ">> ";
        cleanInStream();
        getline(cin, user);
        try {
            in = stoi(user);
            flag = true;
        }
        catch (const std::invalid_argument) {
            flag = false;
            cout << "Invalid format of command! Expected natural number" << endl;
        }

    } while (!flag);
    return in;
}

int main()
{
    DWORD num_of_threads;
    DWORD start_time, end_time;
    int user = 1;
    cout << "Welcom in program 'count Pi number'\n";
    cout << "Created by Eugeny Butov. Group 0305\n";
    while (user == 1) {
        pi_num = 0;
        cout << "Enter count of threads:\n>>";
        cin >> num_of_threads;

        InitializeCriticalSection(crit_sec); //Инициализация критической секции
        DWORD* start_position = new DWORD[num_of_threads]; //Старотовые позиции потоков
        HANDLE* thread_description = new HANDLE[num_of_threads]; //Сами потоки непосредственно
        offset = iteration_block * num_of_threads; //Сразу сделаем сдвиг как будто мы уже выполнили по блоку в каждом потоке

        for (int i = 0; i < num_of_threads; i++) {
            start_position[i] = iteration_block * i; //Старотовая позиция итого потока
            thread_description[i] = CreateThread(NULL, 0, ThreadCalculate, &start_position[i], CREATE_SUSPENDED, NULL); //инициализируем приостановленным
        }
        //Начинаем временный эксперимент
        start_time = timeGetTime();
        for (int i = 0; i < num_of_threads; i++) {
            ResumeThread(thread_description[i]); //Запускаем потоки
        }
        //Будем ждать до тех пор, пока все потоки не завершаться логичным образом
        // Т.е. пока число пи не будет посчитано
        WaitForMultipleObjects(num_of_threads, thread_description, TRUE, INFINITE);
        end_time = timeGetTime();

        printf("Number pi: %.13f\n", pi_num);
        cout << "Total time: " << end_time - start_time << " ms\n";
        cout << "Do you want run programm again?\n 1 - YES 0 - NO\n";
        user = InputAsNumber();
        system("cls");
    }
    cout << "Goodbay and Thank you!";
    return 0;
}

