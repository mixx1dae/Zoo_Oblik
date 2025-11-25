#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> // Для std::numeric_limits
#include <chrono> // Для роботи з часом
#include <ctime>  // Для перетворення часу
#include <algorithm> // Для std::max

using namespace std;

// Константа для максимального розміру
const int MAX_ANIMALS = 24;

// СТРУКТУРА (ОНОВЛЕНО)
struct Animal
{
    string name = ""; // Ініціалізація пустим рядком
    int age = 0;      // Ініціалізація нулем
    string location = "";
    string medical_history = "Немає"; // Нове поле для медичних записів
};

// =====================================================
//           ГОЛОВНІ ВИКОНАВЧІ ФУНКЦІЇ
// =====================================================

/**
 * ЖУРНАЛ ПОДІЙ: Записує дію у файл action_log.txt
 */
void logAction(const string& action)
{
    const string log_filename = "action_log.txt";
    ofstream logFile(log_filename, ios::app);

    if (logFile.is_open())
    {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

        char time_buffer[26];
        ctime_s(time_buffer, sizeof(time_buffer), &now);
        string time_str(time_buffer);

        if (!time_str.empty() && time_str.back() == '\n')
        {
            time_str.pop_back();
        }

        logFile << "[" << time_str << "] - " << action << endl;
        logFile.close();
    }
    else
    {
        cerr << "Помилка: Не вдалося відкрити лог-файл" << endl;
    }
}

/**
 * ВВЕДЕННЯ: Функція для додавання НОВИХ тваринок до існуючого списку
 *
 */
void addNewAnimals(Animal data[], int& current_num)
{
    int space_left = MAX_ANIMALS - current_num;

    if (space_left <= 0) {
        cout << "\nСписок повний! (Максимум " << MAX_ANIMALS << " тваринок).\n";
        logAction("Спроба додати нові записи. Список повний.");
        return;
    }

    int n_to_add;
    cout << "\n--- ДОДАВАННЯ ТВАРИНИ (ВВЕДЕННЯ) ---\n";
    cout << "У списку вже є " << current_num << ". Можна додати ще " << space_left << ".\n";

    do {
        cout << "Введіть, скільки тваринок додати (0 - скасувати): ";
        if (!(cin >> n_to_add)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Помилка вводу. Спробуйте ще раз.\n";
            n_to_add = -1;
        }
        else if (n_to_add < 0 || n_to_add > space_left) {
            cerr << "Некоректна кількість. Введіть число від 0 до " << space_left << ".\n";
        }
    } while (n_to_add < 0 || n_to_add > space_left);

    if (n_to_add == 0)
    {
        cout << "Додавання скасовано.\n";
        return;
    }

    logAction("Початок додавання " + to_string(n_to_add) + " нових тваринок.");
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищення буфера

    int new_total_count = current_num + n_to_add;

    for (int i = current_num; i < new_total_count; i++)
    {
        cout << "\n--- Тваринка #" << i + 1 << " ---";
        cout << "\nВведіть ім'я тваринки: ";
        getline(cin, data[i].name);
        while (data[i].name.empty())
        {
            cout << "Ім'я не може бути порожнім. Введіть ім'я: ";
            getline(cin, data[i].name);
        }

        cout << "Введіть вік тваринки: ";
        while (!(cin >> data[i].age) || data[i].age < 0) {
            cerr << "Некоректний вік. Введіть додатне число: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Введіть розміщення тваринки (номер клітки): ";
        getline(cin, data[i].location);
        while (data[i].location.empty())
        {
            cout << "Розміщення не може бути порожнім. Введіть розміщення: ";
            getline(cin, data[i].location);
        }
        // medical_history автоматично = "Немає" (зі структури)
    }

    current_num = new_total_count;
    logAction("Успішно додано " + to_string(n_to_add) + " тваринок.");
    cout << "\nНові дані введено. Загальна кількість: " << current_num << ". Збережіть їх (пункт 5).\n";
}


// =====================================================
//           ДОПОМІЖНІ ФУНКЦІЇ (Меню, I/O)
// =====================================================

// Прототипи
void menu(Animal data[], int& current_num);
void outputAnimal(Animal data[], int n);
void saveToCSV(Animal data[], int n);
int loadFromCSV(Animal data[], int max_size);
void moveAnimal(Animal data[], int current_num);
void addMedicalRecord(Animal data[], int current_num);

/**
 * ПЕРЕМІЩЕННЯ: Змінює локацію обраної тварини
 */
void moveAnimal(Animal data[], int current_num)
{
    outputAnimal(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nСписок порожній. Немає кого переміщувати.\n";
        return;
    }

    cout << "\n--- ПЕРЕМІЩЕННЯ ТВАРИНИ ---";
    cout << "\nВведіть номер тварини для переміщення (0 - скасувати): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Невірний номер. Скасування дії.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Скасовано.\n";
        return;
    }

    int array_index = index_to_modify - 1;
    string old_location = data[array_index].location;

    cout << "Обрана тварина: " << data[array_index].name;
    cout << "\nПоточне розміщення: " << old_location;
    cout << "\nВведіть НОВЕ розміщення: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищення буфера

    string new_location;
    getline(cin, new_location);

    if (!new_location.empty())
    {
        data[array_index].location = new_location;
        logAction("Переміщення: " + data[array_index].name + " з [" + old_location + "] до [" + new_location + "]");
        cout << "Тварину переміщено. Не забудьте зберегти (пункт 5).\n";
    }
    else
    {
        cout << "Введення порожнє. Скасовано.\n";
    }
}

/**
 * МЕДИЧНІ ЗАПИСИ: Додає новий запис до історії хвороби
 */
void addMedicalRecord(Animal data[], int current_num)
{
    outputAnimal(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nСписок порожній. Немає кому додавати запис.\n";
        return;
    }

    cout << "\n--- МЕДИЧНИЙ ЖУРНАЛ ---";
    cout << "\nВведіть номер тварини для оновлення (0 - скасувати): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Невірний номер. Скасування дії.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Скасовано.\n";
        return;
    }

    int array_index = index_to_modify - 1;

    cout << "Обрана тварина: " << data[array_index].name;
    cout << "\nПоточна мед. історія: " << data[array_index].medical_history;
    cout << "\nВведіть НОВИЙ ЗАПИС (буде додано до історії): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищення буфера

    string new_record;
    getline(cin, new_record);

    if (!new_record.empty())
    {
        if (data[array_index].medical_history == "Немає")
        {
            data[array_index].medical_history = new_record;
        }
        else
        {
            data[array_index].medical_history += " | " + new_record; // Додаємо з роздільником
        }
        logAction("Мед. запис: " + data[array_index].name + " - додано: '" + new_record + "'");
        cout << "Медичний запис оновлено. Не забудьте зберегти (пункт 5).\n";
    }
    else
    {
        cout << "Введення порожнє. Скасовано.\n";
    }
}


/**
 * Читання даних з CSV-файлу (ОНОВЛЕНО)
 */
int loadFromCSV(Animal data[], int max_size)
{
    const string filename = "animals_data.csv";
    ifstream inputFile(filename);
    string line;
    int count = 0;

    if (!inputFile.is_open()) {
        cout << "\nФайл даних '" << filename << "' не знайдено. Почнемо з порожнього списку.\n";
        return 0;
    }

    // Пропускаємо заголовок (Ім'я,Вік,Розміщення,Мед.Історія)
    getline(inputFile, line);

    while (getline(inputFile, line) && count < max_size)
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        int field_index = 0;

        // Використовуємо ',' як роздільник
        while (getline(ss, segment, ',')) {
            if (field_index == 0) {
                data[count].name = segment;
            }
            else if (field_index == 1) {
                try {
                    data[count].age = stoi(segment);
                }
                catch (...) {
                    data[count].age = 0;
                }
            }
            else if (field_index == 2) {
                data[count].location = segment;
            }
            else if (field_index == 3) { // Нове поле
                data[count].medical_history = segment;
            }
            field_index++;
        }

        if (field_index > 0)
        {
            count++;
        }
    }

    inputFile.close();
    cout << "\nУспішно завантажено " << count << " записів з файлу '" << filename << "'.\n";
    return count;
}


/**
 * Виведення даних у форматі таблиці (ОНОВЛЕНО - Перенос рядків)
 */
void outputAnimal(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nНемає даних для виведення. Спочатку додайте дані (пункт 1).\n";
        return;
    }

    // Визначаємо ширину колонок
    const int w_num = 2;
    const int w_name = 16;
    const int w_age = 7;
    const int w_loc = 30;
    const int w_med = 40;

    cout << "\n--- ДАНІ ПРО " << n << " ТВАРИНОК ---\n";

    // Формуємо розділювач таблиці
    string separator = "+" + string(w_num + 2, '-')
        + "+" + string(w_name + 2, '-')
        + "+" + string(w_age + 2, '-')
        + "+" + string(w_loc + 2, '-')
        + "+" + string(w_med + 2, '-') + "+\n";

    cout << separator;

    cout << "| " << left << setw(w_num) << "#"
        << " | " << left << setw(w_name) << "Ім'я"
        << " | " << setw(w_age) << "Вік"
        << " | " << setw(w_loc) << "Розміщення"
        << " | " << setw(w_med) << "Мед. Історія" << " |\n";

    cout << separator;

    for (int i = 0; i < n; i++)
    {
        string s_name = data[i].name;
        string s_loc = data[i].location;
        string s_med = data[i].medical_history;

        // Обчислюємо необхідну кількість рядків для поточного запису
        // Використовуємо static_cast<int> для уникнення попереджень C4267
        int h_name = s_name.empty() ? 1 : static_cast<int>((s_name.length() + w_name - 1) / w_name);
        int h_loc = s_loc.empty() ? 1 : static_cast<int>((s_loc.length() + w_loc - 1) / w_loc);
        int h_med = s_med.empty() ? 1 : static_cast<int>((s_med.length() + w_med - 1) / w_med);

        // Знаходимо максимальну висоту для цього рядка таблиці
        int max_h = h_name;
        if (h_loc > max_h) max_h = h_loc;
        if (h_med > max_h) max_h = h_med;
        if (max_h < 1) max_h = 1;

        for (int r = 0; r < max_h; r++)
        {
            cout << "| ";

            // Колонка # (тільки в першому рядку)
            if (r == 0) cout << left << setw(w_num) << (i + 1);
            else        cout << setw(w_num) << " ";

            cout << " | ";

            // Колонка Ім'я
            string sub_name = "";
            // Перетворення для уникнення попереджень порівняння signed/unsigned
            if (static_cast<size_t>(r * w_name) < s_name.length())
                sub_name = s_name.substr(r * w_name, w_name);
            cout << left << setw(w_name) << sub_name;

            cout << " | ";

            // Колонка Вік (тільки в першому рядку)
            if (r == 0) cout << setw(w_age) << data[i].age;
            else        cout << setw(w_age) << " ";

            cout << " | ";

            // Колонка Розміщення
            string sub_loc = "";
            if (static_cast<size_t>(r * w_loc) < s_loc.length())
                sub_loc = s_loc.substr(r * w_loc, w_loc);
            cout << left << setw(w_loc) << sub_loc;

            cout << " | ";

            // Колонка Мед. Історія
            string sub_med = "";
            if (static_cast<size_t>(r * w_med) < s_med.length())
                sub_med = s_med.substr(r * w_med, w_med);
            cout << left << setw(w_med) << sub_med;

            cout << " |\n";
        }

        // Друкуємо розділювач після кожного запису
        cout << separator;
    }
}

/**
 * Збереження даних в CSV (ОНОВЛЕНО)
 */
void saveToCSV(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nНемає даних для збереження.\n";
        return;
    }

    const string filename = "animals_data.csv";
    ofstream outFile(filename);

    if (outFile.is_open())
    {
        // Записуємо заголовок (ОНОВЛЕНО)
        outFile << "Ім'я,Вік,Розміщення,Мед.Історія\n";

        for (int i = 0; i < n; i++)
        {
            if (!data[i].name.empty())
            {
                // Записуємо дані (ОНОВЛЕНО)
                outFile << data[i].name << ","
                    << data[i].age << ","
                    << data[i].location << ","
                    << data[i].medical_history << "\n";
            }
        }

        outFile.close();
        cout << "\nДані про " << n << " тваринок успішно збережено у файл: " << filename << endl;
        logAction("Дані успішно збережено (" + to_string(n) + " записів).");
    }
    else
    {
        cerr << "\nПомилка: Не вдалося відкрити файл " << filename << " для запису!" << endl;
        logAction("ПОМИЛКА: Не вдалося зберегти дані у файл CSV.");
    }
}

// ====================================================================
// ГОЛОВНЕ МЕНЮ (ОНОВЛЕНО)
// ====================================================================

void menu(Animal data[], int& current_num)
{
    logAction("Програма запущена.");
    bool running = true;
    while (running)
    {
        int choice = 0;

        cout << "\n================= ГОЛОВНЕ МЕНЮ =================\n";
        cout << "  (Наразі обробляється " << current_num << "/" << MAX_ANIMALS << " тваринок)\n";
        cout << "------------------------------------------------\n";
        cout << "  1 - Додати тварину (Введення)\n";
        cout << "  2 - Перемістити тварину (Переміщення)\n";
        cout << "  3 - Додати медичний запис\n";
        cout << "  4 - Вивести всі тваринки на екран\n";
        cout << "  5 - Зберегти поточні дані у файл CSV\n";
        cout << "  6 - Вихід з програми\n";
        cout << "------------------------------------------------\n";
        cout << "Ваш вибір: ";

        if (!(cin >> choice)) {
            logAction("Невірний ввід в меню (не число).");
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "\nНевірний ввід. Спробуйте 1-6.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            addNewAnimals(data, current_num);
            break;
        case 2:
            moveAnimal(data, current_num);
            break;
        case 3:
            addMedicalRecord(data, current_num);
            break;
        case 4:
            outputAnimal(data, current_num);
            break;
        case 5:
            saveToCSV(data, current_num);
            break;
        case 6:
            running = false;
            logAction("Програма завершена користувачем.");
            cout << "Вихід з програми.\n";
            break;
        default:
            logAction("Невірний вибір в меню: " + to_string(choice));
            cerr << "\nНевірний вибір. Спробуйте 1-6.\n";
            break;
        }
    }
}

int main()
{
    // Встановлюємо кодування консолі для Windows
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int current_animal_count = 0;
    Animal all_animals[MAX_ANIMALS];

    // Завантаження даних при старті
    current_animal_count = loadFromCSV(all_animals, MAX_ANIMALS);

    // Виклик головного меню
    menu(all_animals, current_animal_count);

    cout << "\nНатисніть Enter для виходу...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}