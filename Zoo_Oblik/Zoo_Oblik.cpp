#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits>     // for std::numeric_limits
#include <chrono>     // for time measurement
#include <ctime>      // for time formatting
#include <algorithm>  // for std::max

using namespace std;

// Function prototypes
string unescape_csv(const string& s);
string escape_csv(const string& s);
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

// Constant for maximum size
const int MAX_ANIMALS = 24;

// Structure (Data)
struct Animal
{
    string name = "";           // initialize with empty string
    int age = 0;                // initialize with zero
    string location = "";
    string medical_history = "None"; // Field for medical records
};

// =====================================================
//             LOGGING HELPER MODULE
// =====================================================

/**
 * Function logAction: writes to action_log.txt
 */
void logAction(const string& action)
{
    const string log_filename = "action_log.txt";
    ofstream logFile(log_filename, ios::app);

    if (logFile.is_open())
    {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

        char time_buffer[26];
        // ctime_s is Windows-specific, but generally needed when using Windows.h
        ctime_s(time_buffer, sizeof(time_buffer), &now); 
        string time_str(time_buffer);

        // Remove trailing newline character from ctime_s output
        if (!time_str.empty() && time_str.back() == '\n')
        {
            time_str.pop_back();
        }

        logFile << "[" << time_str << "] - " << action << endl;
        logFile.close();
    }
    else
    {
        cerr << "Error: Failed to open log file" << endl;
    }
}

/**
 * Purpose: Function for data manipulation and logging
 */
void addNewAnimals(Animal data[], int& current_num)
{
    int space_left = MAX_ANIMALS - current_num;

    if (space_left <= 0) {
        cout << "\nList is full! (Maximum " << MAX_ANIMALS << " animals).\n";
        logAction("Attempt to add new records failed: list is full");
        return;
    }

    int n_to_add;
    cout << "\n--- ADDING ANIMALS (INPUT) ---\n";
    cout << "List already contains " << current_num << ". Can add " << space_left << " more.\n";

    do {
        cout << "Enter how many animals to add (0 - cancel): ";
        if (!(cin >> n_to_add)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input. Please try again.\n";
            n_to_add = -1;
        }
        else if (n_to_add < 0 || n_to_add > space_left) {
            cerr << "Invalid number. Enter a number from 0 to " << space_left << ".\n";
        }
    } while (n_to_add < 0 || n_to_add > space_left);

    if (n_to_add == 0)
    {
        cout << "Addition cancelled.\n";
        return;
    }

    logAction("Starting addition of " + to_string(n_to_add) + " new animals.");
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // buffer clearing

    int new_total_count = current_num + n_to_add;

    for (int i = current_num; i < new_total_count; i++)
    {
        cout << "\n--- Animal #" << i + 1 << " ---";
        cout << "\nEnter animal name: ";
        getline(cin, data[i].name);
        data[i].name = trim(data[i].name);

        while (data[i].name.empty())
        {
            cout << "Name cannot be empty. Enter name: ";
            getline(cin, data[i].name);
            data[i].name = trim(data[i].name);
        }

        cout << "Enter animal age: ";
        while (!(cin >> data[i].age) || data[i].age < 0) {
            cerr << "Invalid age. Enter a positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter animal location (cage number): ";
        getline(cin, data[i].location);
        while (data[i].location.empty())
        {
            cout << "Location cannot be empty. Enter location: ";
            getline(cin, data[i].location);
        }
        // medical_history defaults to "None" (from struct)
    }

    current_num = new_total_count;
    logAction("Successfully added " + to_string(n_to_add) + " animals.");
    cout << "\nNew data entered. Current count: " << current_num << ". Save them (option 5).\n";
}


// =====================================================
//             OUTPUT/INPUT MODULE (I/O)
// =====================================================

// Function prototypes
void menu(Animal data[], int& current_num);
void outputAnimals(Animal data[], int n);
void saveToCSV(Animal data[], int n);
int loadFromCSV(Animal data[], int max_size);
void moveAnimal(Animal data[], int current_num);
void addMedicalRecord(Animal data[], int current_num);

/**
 * Purpose: Function for moving animals
 */
void moveAnimal(Animal data[], int current_num)
{
    outputAnimals(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nList is empty. Nothing to move.\n";
        return;
    }

    cout << "\n--- MOVING AN ANIMAL ---";
    cout << "\nEnter animal number to move (0 - cancel): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Invalid number. Action cancelled.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Cancelled.\n";
        return;
    }

    int array_index = index_to_modify - 1;
    string old_location = data[array_index].location;

    cout << "Selected animal: " << data[array_index].name;
    cout << "\nCurrent location: " << old_location;
    cout << "\nEnter NEW location: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // buffer clearing

    string new_location;
    getline(cin, new_location);

    if (!new_location.empty())
    {
        data[array_index].location = new_location;
        logAction("Movement: " + data[array_index].name + " from [" + old_location + "] to [" + new_location + "]");
        cout << "Animal moved. Don't forget to save (option 5).\n";
    }
    else
    {
        cout << "Input is empty. Cancelled.\n";
    }
}

/**
 * MEDICAL RECORDS: Adds a new record to the medical history
 */
void addMedicalRecord(Animal data[], int current_num)
{
    outputAnimals(data, current_num);
    if (current_num <= 0)
    {
        cout << "\nList is empty. Nothing to update.\n";
        return;
    }

    cout << "\n--- MEDICAL RECORD ---";
    cout << "\nEnter animal number to update (0 - cancel): ";

    int index_to_modify;
    if (!(cin >> index_to_modify) || index_to_modify < 0 || index_to_modify > current_num)
    {
        cout << "Invalid number. Action cancelled.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    if (index_to_modify == 0)
    {
        cout << "Cancelled.\n";
        return;
    }

    int array_index = index_to_modify - 1;

    cout << "Selected animal: " << data[array_index].name;
    cout << "\nCurrent med. history: " << data[array_index].medical_history;
    cout << "\nEnter NEW RECORD (will be appended to history): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // buffer clearing

    string new_record;
    getline(cin, new_record);

    if (!new_record.empty())
    {
        if (data[array_index].medical_history == "None")
        {
            data[array_index].medical_history = new_record;
        }
        else
        {
            data[array_index].medical_history += " | " + new_record; // append with a separator
        }
        logAction("Med. Record: " + data[array_index].name + " - added: '" + new_record + "'");
        cout << "Medical record updated. Don't forget to save (option 5).\n";
    }
    else
    {
        cout << "Input is empty. Cancelled.\n";
    }
}

/**
 * Reading data from CSV file
 */
int loadFromCSV(Animal data[], int max_size)
{
    const string filename = "animals_data.csv";
    ifstream inputFile(filename);
    string line;
    int count = 0;

    if (!inputFile.is_open()) {
        cout << "\nFile '" << filename << "' not found. Create and populate it.\n";
        logAction("File 'animals_data.csv' not found. Started with 0 records.");
        return 0;
    }

    // Skipping header (Name,Age,Location,Med.History)
    getline(inputFile, line);

    while (getline(inputFile, line) && count < max_size)
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        int field_index = 0;

        // Splitting ',' into fields
        while (getline(ss, segment, ',')) {
            // ATTENTION: Unescaping segments from quotes before use
            string clean_segment = unescape_csv(segment);
            if (field_index == 0) {
                data[count].name = clean_segment;
            }
            else if (field_index == 1) {
                try {
                    data[count].age = stoi(clean_segment);
                }
                catch (...) {
                    data[count].age = 0;
                }
            }
            else if (field_index == 2) {
                data[count].location = clean_segment;
            }
            else if (field_index == 3) { // New field
                data[count].medical_history = clean_segment;
            }
            field_index++;
        }

        if (field_index > 0)
        {
            count++;
        }
    }

    inputFile.close();
    cout << "\nSuccessfully loaded " << count << " records from file: '" << filename << "'.\n";
    logAction("Successfully loaded " + to_string(count) + " records.");
    return count;
}


/**
 * Displaying data in table format
 */
void outputAnimals(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nNo data to display. Add data first (option 1).\n";
        return;
    }

    // Basic column widths
    const int w_num = 2;
    const int w_name = 16;
    const int w_age = 7;
    const int w_loc = 30;
    const int w_med = 40;

    cout << "\n--- List of " << n << " animals ---\n";

    // Creating separator and header
    string separator = "+" + string(w_num + 2, '-')
        + "+" + string(w_name + 2, '-')
        + "+" + string(w_age + 2, '-')
        + "+" + string(w_loc + 2, '-')
        + "+" + string(w_med + 2, '-') + "+\n";

    cout << separator;

    cout << "| " << left << setw(w_num) << "#"
        << " | " << left << setw(w_name) << "Name"
        << " | " << setw(w_age) << "Age"
        << " | " << setw(w_loc) << "Location"
        << " | " << setw(w_med) << "Med. History" << " |\n";

    cout << separator;

    for (int i = 0; i < n; i++)
    {
        string s_name = data[i].name;
        string s_loc = data[i].location;
        string s_med = data[i].medical_history;

    // Calculation of field height and number of rows
    // using static_cast<int to avoid C4267 warning
        int h_name = s_name.empty() ? 1 : static_cast<int>((s_name.length() + w_name - 1) / w_name);
        int h_loc = s_loc.empty() ? 1 : static_cast<int>((s_loc.length() + w_loc - 1) / w_loc);
        int h_med = s_med.empty() ? 1 : static_cast<int>((s_med.length() + w_med - 1) / w_med);

    // Selecting max height of records to cover all fields
        int max_h = h_name;
        if (h_loc > max_h) max_h = h_loc;
        if (h_med > max_h) max_h = h_med;
        if (max_h < 1) max_h = 1;

        for (int r = 0; r < max_h; r++)
        {
            cout << "| ";

            // Number (output and formatting)
            if (r == 0) cout << left << setw(w_num) << (i + 1);
            else        cout << setw(w_num) << " ";

            cout << " | ";

            // Output name
            string sub_name = "";
            // Ensuring types (signed/unsigned) during indexing
            if (static_cast<size_t>(r * w_name) < s_name.length())
                sub_name = s_name.substr(r * w_name, w_name);
            cout << left << setw(w_name) << sub_name;

            cout << " | ";

            // Output age (first column)
            if (r == 0) cout << setw(w_age) << data[i].age;
            else        cout << setw(w_age) << " ";

            cout << " | ";

            // Output location
            string sub_loc = "";
            if (static_cast<size_t>(r * w_loc) < s_loc.length())
                sub_loc = s_loc.substr(r * w_loc, w_loc);
            cout << left << setw(w_loc) << sub_loc;

            cout << " | ";

            // Output medical history
            string sub_med = "";
            if (static_cast<size_t>(r * w_med) < s_med.length())
                sub_med = s_med.substr(r * w_med, w_med);
            cout << left << setw(w_med) << sub_med;

            cout << " |\n";
        }

        // Printing separator after each record
        cout << separator;
    }
}

/**
 * Helper function for writing: escapes fields with double quotes
 * if they contain commas.
 */
string escape_csv(const string& s) {
    if (s.find(',') != string::npos || s.find('"') != string::npos) {
        // Handle quotes within the string by doubling them
        string temp = s;
        size_t pos = temp.find('"');
        while(pos != string::npos) {
            temp.replace(pos, 1, "\"\"");
            pos = temp.find('"', pos + 2);
        }
        return "\"" + temp + "\"";
    }
    return s;
}

/**
 * Helper function for reading: removes quotes if present.
 */
string unescape_csv(const string& s) {
    // Check if the string is quoted
    if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
        // Remove outer quotes
        string temp = s.substr(1, s.length() - 2);
        // Replace doubled quotes with a single quote
        size_t pos = temp.find("\"\"");
        while(pos != string::npos) {
            temp.replace(pos, 2, "\"");
            pos = temp.find("\"\"", pos + 1);
        }
        return temp;
    }
    return s;
}

/**
 * Saving data to CSV
 */
void saveToCSV(Animal data[], int n)
{
    if (n <= 0) {
        cout << "\nNo data to save.\n";
        return;
    }

    const string filename = "animals_data.csv";
    ofstream outFile(filename);

    if (outFile.is_open())
    {
        // Writing CSV header (UPDATED)
        outFile << "Name,Age,Location,Med.History\n";

        for (int i = 0; i < n; i++)
        {
            if (!data[i].name.empty())
            {
                // ATTENTION: Use escape_csv for fields that might contain quotes or commas
                outFile << escape_csv(data[i].name) << ","
                    << data[i].age << ","
                    << escape_csv(data[i].location) << ","
                    << escape_csv(data[i].medical_history) << "\n"; // Included medical_history
            }
        }

        outFile.close();
        cout << "\nData for " << n << " animals successfully saved to file: " << filename << endl;
        logAction("Data successfully saved (" + to_string(n) + " records).");
    }
    else
    {
        cerr << "\nError: Failed to open file " << filename << " for writing!" << endl;
        logAction("ERROR: Failed to save data to CSV file.");
    }
}

// ====================================================================
// Main menu
// ====================================================================

void menu(Animal data[], int& current_num)
{
    logAction("Program started.");
    bool running = true;
    while (running)
    {
        int choice = 0;

        cout << "\n================= MAIN MENU =================\n";
        cout << "  (Currently processing " << current_num << "/" << MAX_ANIMALS << " animals)\n";
        cout << "------------------------------------------------\n";
        cout << "  1 - Add animal (Input)\n";
        cout << "  2 - Move animal (Location Change)\n";
        cout << "  3 - Add medical record\n";
        cout << "  4 - Display all animals\n";
        cout << "  5 - Save current data to CSV file\n";
        cout << "  6 - Exit program\n";
        cout << "------------------------------------------------\n";
        cout << "Your choice: ";

        if (!(cin >> choice)) {
            logAction("Invalid input in menu (not a number).");
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "\nInvalid input. Try 1-6.\n";
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
            outputAnimals(data, current_num);
            break;
        case 5:
            saveToCSV(data, current_num);
            break;
        case 6:
            running = false;
            logAction("Program terminated by user.");
            cout << "Exiting program.\n";
            break;
        default:
            logAction("Invalid menu choice: " + to_string(choice));
            cerr << "\nInvalid choice. Try 1-6.\n";
            break;
        }
    }
}

int main()
{

    
    int current_animal_count = 0;
    Animal all_animals[MAX_ANIMALS];

    // Loading data on startup
    current_animal_count = loadFromCSV(all_animals, MAX_ANIMALS);

    // Calling main menu
    menu(all_animals, current_animal_count);

    cout << "\nPress Enter to exit...";
    // Clear any pending input before the final cin.get()
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}