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

using namespace std;
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}
// Константа для максимального розміру
const int MAX_ANIMALS = 24;

// СТРУКТУРА
struct Animal
{
    string name = ""; // Ініціалізація пустим рядком
    int age = 0;      // Ініціалізація нулем
    string location = "";
    string medical_history = "Немає"; // Нове поле для медичних записів
};

/**
 * ЖУРНАЛ ПОДІЙ: Записує дію у файл action_log.txt (Я.П.)
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
 * ВВЕДЕННЯ: Функція для додавання НОВИХ тваринок до існуючого списку (Н.Б.)
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
        data[i].name = trim(data[i].name);

        while (data[i].name.empty())
        {
            cout << "Ім'я не може бути порожнім. Введіть ім'я: ";
            getline(cin, data[i].name);
            data[i].name = trim(data[i].name);
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

// Прототипи
void menu(Animal data[], int& current_num);
void outputAnimal(Animal data[], int n);
void saveToCSV(Animal data[], int n);
int loadFromCSV(Animal data[], int max_size);
void moveAnimal(Animal data[], int current_num);
void addMedicalRecord(Animal data[], int current_num);

/**
 * ПЕРЕМІЩЕННЯ: Змінює локацію обраної тварини (Я.П.)
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
 * МЕДИЧНІ ЗАПИСИ: Додає новий запис до історії хвороби (Л.Д.)
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

    int array_index = index_to_modify;

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
 * Читання даних з CSV-файлу
 */
int loadFromCSV(Animal data[], int max_size)
{
    const string filename = "animals_data.csv";
    ifstream inputFile(filename);
    string line;
    int count = 0;

    if (!inputFile.is_open()) {
        cout << "\nФайл даних '" << filename << "' не знайдено. Список порожній.\n";
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
 * Виведення даних у форматі таблиці
 */
void outputAnimal(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nНемає даних для виведення. Спочатку додайте дані (пункт 1).\n";
        return;
    }

    cout << "\n--- ДАНІ ПРО " << n << " ТВАРИНОК ---\n";
    cout << "+----+------------------+---------+------------------+--------------------+\n";
    cout << "| " << left << setw(2) << "#"
        << " | " << left << setw(16) << "Ім'я"
        << " | " << setw(7) << "Вік"
        << " | " << setw(16) << "Розміщення"
        << " | " << setw(18) << "Мед. Історія" << " |\n";
    cout << "+----+------------------+---------+------------------+--------------------+\n";

    for (int i = 0; i < n; i++)
    {
        string name_preview = data[i].name.length() > 16 ? data[i].name.substr(0, 13) + "..." : data[i].name;
        string loc_preview = data[i].location.length() > 16 ? data[i].location.substr(0, 13) + "..." : data[i].location;
        string med_preview = data[i].medical_history.length() > 18 ? data[i].medical_history.substr(0, 15) + "..." : data[i].medical_history;

        cout << "| " << left << setw(2) << (i + 1)
            << " | " << left << setw(16) << name_preview
            << " | " << setw(7) << data[i].age
            << " | " << setw(16) << loc_preview
            << " | " << setw(18) << med_preview << " |\n";
    }
    cout << "+----+------------------+---------+------------------+--------------------+\n";
}

/**
 * Збереження даних в CSV
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