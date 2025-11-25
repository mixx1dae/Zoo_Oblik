#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> // Äëÿ std::numeric_limits
#include <chrono> // Äëÿ ðîáîòè ç ÷àñîì
#include <ctime>  // Äëÿ ïåðåòâîðåííÿ ÷àñó

using namespace std;

// ïðîòîòèïè
string unescape_csv(const string& s);
string escape_csv(const string& s);
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}
// Êîíñòàíòà äëÿ ìàêñèìàëüíîãî ðîçì³ðó
const int MAX_ANIMALS = 24;

// ÑÒÐÓÊÒÓÐÀ
struct Animal
{
    string name = ""; // ²í³ö³àë³çàö³ÿ ïóñòèì ðÿäêîì
    int age = 0;      // ²í³ö³àë³çàö³ÿ íóëåì
    string location = "";
    string medical_history = "Íåìàº"; // Íîâå ïîëå äëÿ ìåäè÷íèõ çàïèñ³â
};

/**
 * ÆÓÐÍÀË ÏÎÄ²É: Çàïèñóº ä³þ ó ôàéë action_log.txt (ß.Ï.)
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
        cerr << "Ïîìèëêà: Íå âäàëîñÿ â³äêðèòè ëîã-ôàéë" << endl;
    }
}

/**
 * ÂÂÅÄÅÍÍß: Ôóíêö³ÿ äëÿ äîäàâàííÿ ÍÎÂÈÕ òâàðèíîê äî ³ñíóþ÷îãî ñïèñêó (Í.Á.)
 */
void addNewAnimals(Animal data[], int& current_num)
{
    int space_left = MAX_ANIMALS - current_num;

    if (space_left <= 0) {
        cout << "\nÑïèñîê ïîâíèé! (Ìàêñèìóì " << MAX_ANIMALS << " òâàðèíîê).\n";
        logAction("Ñïðîáà äîäàòè íîâ³ çàïèñè. Ñïèñîê ïîâíèé.");
        return;
    }

    int n_to_add;
    cout << "\n--- ÄÎÄÀÂÀÍÍß ÒÂÀÐÈÍÈ (ÂÂÅÄÅÍÍß) ---\n";
    cout << "Ó ñïèñêó âæå º " << current_num << ". Ìîæíà äîäàòè ùå " << space_left << ".\n";

    do {
        cout << "Ââåä³òü, ñê³ëüêè òâàðèíîê äîäàòè (0 - ñêàñóâàòè): ";
        if (!(cin >> n_to_add)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Ïîìèëêà ââîäó. Ñïðîáóéòå ùå ðàç.\n";
            n_to_add = -1;
        }
        else if (n_to_add < 0 || n_to_add > space_left) {
            cerr << "Íåêîðåêòíà ê³ëüê³ñòü. Ââåä³òü ÷èñëî â³ä 0 äî " << space_left << ".\n";
        }
    } while (n_to_add < 0 || n_to_add > space_left);

    if (n_to_add == 0)
    {
        cout << "Äîäàâàííÿ ñêàñîâàíî.\n";
        return;
    }

    logAction("Ïî÷àòîê äîäàâàííÿ " + to_string(n_to_add) + " íîâèõ òâàðèíîê.");
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Î÷èùåííÿ áóôåðà

    int new_total_count = current_num + n_to_add;

    for (int i = current_num; i < new_total_count; i++)
    {
        cout << "\n--- Òâàðèíêà #" << i + 1 << " ---";
        cout << "\nÂâåä³òü ³ì'ÿ òâàðèíêè: ";
        getline(cin, data[i].name);
        data[i].name = trim(data[i].name);

        while (data[i].name.empty())
        {
            cout << "²ì'ÿ íå ìîæå áóòè ïîðîæí³ì. Ââåä³òü ³ì'ÿ: ";
            getline(cin, data[i].name);
            data[i].name = trim(data[i].name);
        }

        cout << "Ââåä³òü â³ê òâàðèíêè: ";
        while (!(cin >> data[i].age) || data[i].age < 0) {
            cerr << "Íåêîðåêòíèé â³ê. Ââåä³òü äîäàòíå ÷èñëî: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Ââåä³òü ðîçì³ùåííÿ òâàðèíêè (íîìåð êë³òêè): ";
        getline(cin, data[i].location);
        while (data[i].location.empty())
        {
            cout << "Ðîçì³ùåííÿ íå ìîæå áóòè ïîðîæí³ì. Ââåä³òü ðîçì³ùåííÿ: ";
            getline(cin, data[i].location);
        }
        // medical_history àâòîìàòè÷íî = "Íåìàº" (ç³ ñòðóêòóðè)
    }

    current_num = new_total_count;
    logAction("Óñï³øíî äîäàíî " + to_string(n_to_add) + " òâàðèíîê.");
    cout << "\nÍîâ³ äàí³ ââåäåíî. Çàãàëüíà ê³ëüê³ñòü: " << current_num << ". Çáåðåæ³òü ¿õ (ïóíêò 5).\n";
}

// Ïðîòîòèïè
void menu(Animal data[], int& current_num);
void outputAnimal(Animal data[], int n);
void saveToCSV(Animal data[], int n);
int loadFromCSV(Animal data[], int max_size);
void moveAnimal(Animal data[], int current_num);
void addMedicalRecord(Animal data[], int current_num);

/**
 * ÏÅÐÅÌ²ÙÅÍÍß: Çì³íþº ëîêàö³þ îáðàíî¿ òâàðèíè (ß.Ï.)
 */
void moveAnimal(Animal data[], int current_num)
{
    outputAnimal(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nÑïèñîê ïîðîæí³é. Íåìàº êîãî ïåðåì³ùóâàòè.\n";
        return;
    }

    cout << "\n--- ÏÅÐÅÌ²ÙÅÍÍß ÒÂÀÐÈÍÈ ---";
    cout << "\nÂâåä³òü íîìåð òâàðèíè äëÿ ïåðåì³ùåííÿ (0 - ñêàñóâàòè): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Íåâ³ðíèé íîìåð. Ñêàñóâàííÿ ä³¿.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Ñêàñîâàíî.\n";
        return;
    }

    int array_index = index_to_modify - 1;
    string old_location = data[array_index].location;

    cout << "Îáðàíà òâàðèíà: " << data[array_index].name;
    cout << "\nÏîòî÷íå ðîçì³ùåííÿ: " << old_location;
    cout << "\nÂâåä³òü ÍÎÂÅ ðîçì³ùåííÿ: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Î÷èùåííÿ áóôåðà

    string new_location;
    getline(cin, new_location);

    if (!new_location.empty())
    {
        data[array_index].location = new_location;
        logAction("Ïåðåì³ùåííÿ: " + data[array_index].name + " ç [" + old_location + "] äî [" + new_location + "]");
        cout << "Òâàðèíó ïåðåì³ùåíî. Íå çàáóäüòå çáåðåãòè (ïóíêò 5).\n";
    }
    else
    {
        cout << "Ââåäåííÿ ïîðîæíº. Ñêàñîâàíî.\n";
    }
}

/**
 * ÌÅÄÈ×Í² ÇÀÏÈÑÈ: Äîäàº íîâèé çàïèñ äî ³ñòîð³¿ õâîðîáè (Ë.Ä.)
 */
void addMedicalRecord(Animal data[], int current_num)
{
    outputAnimal(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nÑïèñîê ïîðîæí³é. Íåìàº êîìó äîäàâàòè çàïèñ.\n";
        return;
    }

    cout << "\n--- ÌÅÄÈ×ÍÈÉ ÆÓÐÍÀË ---";
    cout << "\nÂâåä³òü íîìåð òâàðèíè äëÿ îíîâëåííÿ (0 - ñêàñóâàòè): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Íåâ³ðíèé íîìåð. Ñêàñóâàííÿ ä³¿.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Ñêàñîâàíî.\n";
        return;
    }

    int array_index = index_to_modify;

    cout << "Îáðàíà òâàðèíà: " << data[array_index].name;
    cout << "\nÏîòî÷íà ìåä. ³ñòîð³ÿ: " << data[array_index].medical_history;
    cout << "\nÂâåä³òü ÍÎÂÈÉ ÇÀÏÈÑ (áóäå äîäàíî äî ³ñòîð³¿): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Î÷èùåííÿ áóôåðà

    string new_record;
    getline(cin, new_record);

    if (!new_record.empty())
    {
        if (data[array_index].medical_history == "Íåìàº")
        {
            data[array_index].medical_history = new_record;
        }
        else
        {
            data[array_index].medical_history += " | " + new_record; // Äîäàºìî ç ðîçä³ëüíèêîì
        }
        logAction("Ìåä. çàïèñ: " + data[array_index].name + " - äîäàíî: '" + new_record + "'");
        cout << "Ìåäè÷íèé çàïèñ îíîâëåíî. Íå çàáóäüòå çáåðåãòè (ïóíêò 5).\n";
    }
    else
    {
        cout << "Ââåäåííÿ ïîðîæíº. Ñêàñîâàíî.\n";
    }
}


/**
 * ×èòàííÿ äàíèõ ç CSV-ôàéëó
 */
int loadFromCSV(Animal data[], int max_size)
{
    const string filename = "animals_data.csv";
    ifstream inputFile(filename);
    string line;
    int count = 0;

    if (!inputFile.is_open()) {
        cout << "\nÔàéë äàíèõ '" << filename << "' íå çíàéäåíî. Ñïèñîê ïîðîæí³é.\n";
        return 0;
    }

    // Ïðîïóñêàºìî çàãîëîâîê (²ì'ÿ,Â³ê,Ðîçì³ùåííÿ,Ìåä.²ñòîð³ÿ)
    getline(inputFile, line);

    while (getline(inputFile, line) && count < max_size)
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        int field_index = 0;

        while (getline(ss, segment, ',')) {
            // !!! ÂÈÏÐÀÂËÅÍÍß: Î÷èùóºìî ñåãìåíò â³ä ëàïîê ïåðåä âèêîðèñòàííÿì !!!
            string clean_segment = unescape_csv(segment);
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
            else if (field_index == 3) { // Íîâå ïîëå
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
    cout << "\nÓñï³øíî çàâàíòàæåíî " << count << " çàïèñ³â ç ôàéëó '" << filename << "'.\n";
    return count;
}


/**
 * Âèâåäåííÿ äàíèõ ó ôîðìàò³ òàáëèö³
 */
void outputAnimal(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nÍåìàº äàíèõ äëÿ âèâåäåííÿ. Ñïî÷àòêó äîäàéòå äàí³ (ïóíêò 1).\n";
        return;
    }

    cout << "\n--- ÄÀÍ² ÏÐÎ " << n << " ÒÂÀÐÈÍÎÊ ---\n";
    cout << "+----+------------------+---------+------------------+--------------------+\n";
    cout << "| " << left << setw(2) << "#"
        << " | " << left << setw(16) << "²ì'ÿ"
        << " | " << setw(7) << "Â³ê"
        << " | " << setw(16) << "Ðîçì³ùåííÿ"
        << " | " << setw(18) << "Ìåä. ²ñòîð³ÿ" << " |\n";
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
 * Çáåðåæåííÿ äàíèõ â CSV
 */
 // Äîïîì³æíà ôóíêö³ÿ äëÿ çàïèñó: åêðàíóº êîìó ïîäâ³éíèìè ëàïêàìè
string escape_csv(const string& s) {
    if (s.find(',') != string::npos) {
        return "\"" + s + "\"";
    }
    return s;
}

// Äîïîì³æíà ôóíêö³ÿ äëÿ ÷èòàííÿ: âèäàëÿº ëàïêè, ÿêùî º
string unescape_csv(const string& s) {
    if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.length() - 2);
    }
    return s;
}

void saveToCSV(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nÍåìàº äàíèõ äëÿ çáåðåæåííÿ.\n";
        return;
    }

    const string filename = "animals_data.csv";
    ofstream outFile(filename);

    if (outFile.is_open())
    {
        // Çàïèñóºìî çàãîëîâîê (ÎÍÎÂËÅÍÎ)
        outFile << "²ì'ÿ,Â³ê,Ðîçì³ùåííÿ,Ìåä.²ñòîð³ÿ\n";

        for (int i = 0; i < n; i++)
        {
            if (!data[i].name.empty())
            {
                // !!! ÂÈÏÐÀÂËÅÍÍß: Âèêîðèñòîâóºìî escape_csv äëÿ ïîë³â ç ìîæëèâèìè êîìàìè !!!
                outFile << escape_csv(data[i].name) << ","
                    << data[i].age << ","
                    << escape_csv(data[i].location) << "\n";
            }
        }

        outFile.close();
        cout << "\nÄàí³ ïðî " << n << " òâàðèíîê óñï³øíî çáåðåæåíî ó ôàéë: " << filename << endl;
        logAction("Äàí³ óñï³øíî çáåðåæåíî (" + to_string(n) + " çàïèñ³â).");
    }
    else
    {
        cerr << "\nÏîìèëêà: Íå âäàëîñÿ â³äêðèòè ôàéë " << filename << " äëÿ çàïèñó!" << endl;
        logAction("ÏÎÌÈËÊÀ: Íå âäàëîñÿ çáåðåãòè äàí³ ó ôàéë CSV.");
    }
}

void menu(Animal data[], int& current_num)
{
    logAction("Ïðîãðàìà çàïóùåíà.");
    bool running = true;
    while (running)
    {
        int choice = 0;

        cout << "\n================= ÃÎËÎÂÍÅ ÌÅÍÞ =================\n";
        cout << "  (Íàðàç³ îáðîáëÿºòüñÿ " << current_num << "/" << MAX_ANIMALS << " òâàðèíîê)\n";
        cout << "------------------------------------------------\n";
        cout << "  1 - Äîäàòè òâàðèíó (Ââåäåííÿ)\n";
        cout << "  2 - Ïåðåì³ñòèòè òâàðèíó (Ïåðåì³ùåííÿ)\n";
        cout << "  3 - Äîäàòè ìåäè÷íèé çàïèñ\n";
        cout << "  4 - Âèâåñòè âñ³ òâàðèíêè íà åêðàí\n";
        cout << "  5 - Çáåðåãòè ïîòî÷í³ äàí³ ó ôàéë CSV\n";
        cout << "  6 - Âèõ³ä ç ïðîãðàìè\n";
        cout << "------------------------------------------------\n";
        cout << "Âàø âèá³ð: ";

        if (!(cin >> choice)) {
            logAction("Íåâ³ðíèé ââ³ä â ìåíþ (íå ÷èñëî).");
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "\nÍåâ³ðíèé ââ³ä. Ñïðîáóéòå 1-6.\n";
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
            logAction("Ïðîãðàìà çàâåðøåíà êîðèñòóâà÷åì.");
            cout << "Âèõ³ä ç ïðîãðàìè.\n";
            break;
        default:
            logAction("Íåâ³ðíèé âèá³ð â ìåíþ: " + to_string(choice));
            cerr << "\nÍåâ³ðíèé âèá³ð. Ñïðîáóéòå 1-6.\n";
            break;
        }
    }
}

int main()
{
    // Âñòàíîâëþºìî êîäóâàííÿ êîíñîë³ äëÿ Windows
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int current_animal_count = 0;
    Animal all_animals[MAX_ANIMALS];

    // Çàâàíòàæåííÿ äàíèõ ïðè ñòàðò³
    current_animal_count = loadFromCSV(all_animals, MAX_ANIMALS);

    // Âèêëèê ãîëîâíîãî ìåíþ
    menu(all_animals, current_animal_count);

    cout << "\nÍàòèñí³òü Enter äëÿ âèõîäó...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}