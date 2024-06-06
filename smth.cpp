#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using namespace std;

class Text {
private:
    char* text;
    int text_len;
    int text_size;

public:
    Text() {
        text_size = 10;
        text_len = 0;
        text = (char*)calloc(text_size, sizeof(char));
    }

    ~Text() {
        free(text);
    }

    void append_text() {
        cout << "\nEnter text to append:\n";

        int input_len = 0;
        int input_size = 10;
        char* input = (char*)calloc(input_size, sizeof(char));

        int character;
        while ((character = getchar()) != '\n') {
            if (input_len >= input_size - 1) {
                input_size *= 2;
                input = (char*)realloc(input, input_size * sizeof(char));
            }
            input[input_len++] = (char)character;
        }

        if (text_len + input_len >= text_size - 1) {
            text_size = (text_len + input_len) * 2;
            text = (char*)realloc(text, text_size * sizeof(char));
        }

        strcat(text, input);
        text_len += input_len;
        cout << "Current text:\n" << text << endl;

        free(input);
    }

    void start_new_line() {
        if (text_len + 1 >= text_size - 1) {
            text_size = (text_size) + 1;
            text = (char*)realloc(text, text_size * sizeof(char));
        }
        strcat(text, "\n");
        text_len++;
        cout << "\nNew line is started\n";
        cout << "Current text:\n" << text << endl;
    }

    void save_text_to_file() {
        cout << "\nEnter the file name for saving: ";
        string file_name;
        getline(cin, file_name);

        if (FILE* file = fopen(file_name.c_str(), "w")) {
            fprintf(file, "%s", text);
            fclose(file);
            cout << "Text has been saved successfully\n";
        } else {
            cout << "Error opening file " << file_name << " for writing\n";
        }
    }

    void load_text_from_file() {
        cout << "Enter the file name for loading: \n";
        string file_name;
        getline(cin, file_name);

        if (FILE* file = fopen(file_name.c_str(), "r")) {
            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (file_size == 0) {
                cout << "The file " << file_name << " is empty\n";
                fclose(file);
                return;
            }

            char* loaded_text = (char*)malloc((file_size + 1) * sizeof(char));
            fread(loaded_text, sizeof(char), file_size, file);
            loaded_text[file_size] = '\0';

            free(text);
            text = loaded_text;
            text_size = file_size + 1;
            text_len = strlen(text);

            cout << "\nText has been loaded successfully\n";
            fclose(file);
        } else {
            cout << "\nError opening file " << file_name << " for reading\n";
        }
    }

    void print_loaded_text() const {
        cout << "Loaded text:\n";
        cout << text << endl;
    }

    void clear_console() const {
        std::cout << "Press enter to clear the screen\n";
        std::string enter;
        std::getline(std::cin, enter);
        if (enter.empty()) {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            std::cout << "\nConsole was cleared\n";
        }
    }

    void search_position() const {
        cout << "Please write text to find out its location:\n";
        string input;
        getline(std::cin, input);

        int line = 0;
        int column = 0;
        int count = 0;

        for (int i = 0; text[i] != '\0'; ++i) {
            if (text[i] == '\n') {
                line++;
                column = 0;
                continue;
            }

            if (strncmp(&text[i], input.c_str(), input.length()) == 0) {
                cout << "Input string was found at [" << line << "][" << column << "] coordinate\n";
                count++;
                i += input.length() - 1;
                column += input.length() - 1;
            }
            column++;
        }

        if (count == 0) {
            cout << "Input string was not found\n";
        }
    }

    void append_by_coordinate() {
        int line = 0;
        int column = 0;

        for (int i = 0; text[i] != '\0'; ++i) {
            if (text[i] == '\n') {
                line++;
                column = 0;
            } else {
                column++;
            }
        }

        cout << "There are " << line + 1 << " lines and " << column << " columns to enter text by coordinates\n";
        cout << "But please start your counting from 0\n";

        cout << "Enter the line number: \n";
        string line_number_input;
        getline(cin, line_number_input);
        cout << "Enter the column number: \n";
        string column_number_input;
        getline(cin, column_number_input);

        int line_number = stoi(line_number_input);
        int column_number = stoi(column_number_input);

        cout << "Enter text you want to add: \n";
        string input;
        getline(cin, input);
        int input_len = input.length();

        if (text_len + input_len >= text_size) {
            text_size = (text_len + input_len) * 2;
            text = (char*)realloc(text, text_size * sizeof(char));
        }

        int insertion_line = 0;
        int insertion_column = 0;
        int insert_index = 0;

        for (int i = 0; text[i] != '\0'; ++i) {
            if (insertion_line == line_number && insertion_column == column_number) {
                insert_index = i;
                break;
            }
            if (text[i] == '\n') {
                insertion_line++;
                insertion_column = 0;
            } else {
                insertion_column++;
            }
        }

        memmove(&text[insert_index + input_len], &text[insert_index], (text_len - insert_index) + 1);
        memcpy(&text[insert_index], input.c_str(), input_len);
        text_len += input_len;
    }
};

int main() {
    Text text;

    while (true) {
        cout << "\nChoose the command (1/2/3/4/5/6/7/8/exit): ";
        string command;
        getline(cin, command);

        if (command == "exit") {
            cout << "\nProgram was cancelled\n";
            break;
        }

        if (command == "1") {
            text.append_text();
        } else if (command == "2") {
            text.start_new_line();
        } else if (command == "3") {
            text.save_text_to_file();
        } else if (command == "4") {
            text.load_text_from_file();
        } else if (command == "5") {
            text.print_loaded_text();
        } else if (command == "6") {
            text.append_by_coordinate();
        } else if (command == "7") {
            text.search_position();
        } else if (command == "8") {
            text.clear_console();
        } else {
            cout << "\nInvalid command\n";
        }
    }

    return 0;
}