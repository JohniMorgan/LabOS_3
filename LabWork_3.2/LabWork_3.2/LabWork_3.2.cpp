#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <omp.h>

using namespace std;


void cleanInStream() {
    cin.clear();
    if (char(cin.peek()) == '\n') cin.ignore();
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

const int N = 100000000; //Точность числа пи
const DWORD iteration_block = 305040; //Количество блоков итераций на поток

//Вычисляемое значение Пи
double pi_num = 0; //Вычисляемое число Пи

int main()
{
    DWORD start_time, end_time;
    int num_of_threads, user = 1;
    
    while (user == 1) {
        cout << "Welcom in program 'count Pi number' by OpenMP source\n";
        cout << "Created by Eugeny Butov. Group 0305\n";
        cout << "Enter count of threads:\n>>";
        cin >> num_of_threads;

        omp_set_num_threads(num_of_threads); //Установить количество потоков

        start_time = timeGetTime();
#pragma omp parallel reduction(+ : pi_num)
        {
#pragma omp for schedule(dynamic, iteration_block)
            for (int i = 0; i < N; i++) {
                double x = ((i + 0.5) / N);
                pi_num += (4 / (1 + x * x));
            }
        }
        end_time = timeGetTime();
        pi_num = pi_num / (N * 1.0);
        cout << "Total time is: " << end_time - start_time << " ms\n";
        printf("Calculated pi number is: %.13f\n", pi_num);
        cout << "Do you want run programm again?\n 1 - YES 0 - NO\n";
        user = InputAsNumber();
        system("cls");
    }
    cout << "Goodbay and Thank you!";
    return 0;
}

