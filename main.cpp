#include <iostream>
#include <cstring>
#include <unistd.h>
#include <limits>
#include <stack>

using namespace std;

class Cursor {
private:
    char cursor;
    int cursorIndex;

public:
    Cursor() {
        cursor = '|';
        cursorIndex = 0;
    }

    int getCursor() {
        return cursorIndex;
    }

    void setCursor(int index) {
        cursorIndex = index+1;
    }

    void printCursor(char* text) {
        text[cursorIndex] = cursor;
    }

    void removeCursor(char* text) {
        text[cursorIndex] = ' ';
    }
};

class Buffer {
private:
    char* buffer;
    int bufferSize;
    int bufferLen;

public:
//constructor
    Buffer() {
        bufferSize = 10;
        bufferLen = 0;
        buffer = (char*)calloc(bufferSize, sizeof(char));
    }

    //destructor
    ~Buffer() {
        free(buffer);
        buffer = nullptr;
    }

    void store(char* text, int symbols) {
        if (symbols >= bufferSize) {
            bufferSize = symbols*2;
            buffer = (char*)realloc(buffer, bufferSize * sizeof(char));
        }
        strncpy(buffer, text, symbols); //copy characters from string
        buffer[symbols] = '\0';
        bufferLen = symbols;
    }

    char * getFromBuffer() {
    return buffer;
}
};

class Text {
private:
    char* text;
    int textSize;
    int textLen;
    stack<char*> stackUndo;
    stack<char*> stackRedo;
    Buffer buffer;
    Cursor cursor;

public:
    //constructor
    Text() {
        textSize = 10;
        textLen = 0;
        text = (char*)calloc(textSize, sizeof(char));
        saveState();
    }

    //destructor
    ~Text() {
        free(text);
        text = nullptr;

        while(!stackUndo.empty()) {
            free(stackUndo.top());
            stackUndo.pop();
        }
        while(!stackRedo.empty()) {
            free(stackRedo.top());
            stackRedo.pop();
        }
    }

    //methods
    void saveState() {
        char* currentText = (char*)calloc(textLen + 1, sizeof(char));
        // to      from
        strcpy(currentText, text);
        stackUndo.push(currentText);
    }

    int findIndex(int line, int column) {
        int index = 0;
        int currentLine = 0;
        int currentColumn = 0;

        while (text[index] != '\0') {
            if (currentLine == line && currentColumn == column) {
                return index;
            }
            if (text[index] == '\n') {
                currentLine++;
                currentColumn = 0;
            }
            else {
                currentColumn++;
            }
            index++;
        }
        return -1;
    }

    void appendText() {
        //cursor.removeCursor(text);
        cout << "Enter text to append:\n";
        int inputLen = 0;
        int inputSize = 10;
        char* input = (char*)calloc(inputSize, sizeof(char));

        int character;
        while ((character = getchar()) != '\n') {
            if (inputLen >= inputSize - 1) {
                inputSize *= 2;
                input = (char*)realloc(input, inputSize * sizeof(char));
            }
            input[inputLen++] = (char)character;
        }

        if (textLen + inputLen >= textSize - 1) {
            textSize = (textLen + inputLen) * 2;
            text = (char*)realloc(text, textSize);
        }

        strcat(text, input);
        textLen += inputLen;

        free(input);
        input = nullptr;
        saveState();
        //cursor.setCursor(textLen);
    }

    void startNewLine () {
        if (textLen + 1 >= textSize - 1) {
            textSize = (textSize) + 1;
            text = (char*)realloc(text, textSize);
        }

        text[textLen++] = '\n';
        text[textLen] = '\0';

        cout << "New line is started\n";
        saveState();
    }

    void saveTextToFile () {
        string fileName;
        cout << "Enter the file name for saving:\n";
        cin >> fileName;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer  smth like this \n==\0

        if (access(fileName.c_str(), F_OK) == 0) { //there is the difference between std::string and C-style string
            FILE* file = fopen(fileName.c_str(), "w");
            if (file != nullptr) {
                fprintf(file, "%s", text);
                fclose(file);
                cout << "Text has been saved successfully\n";
            }
            else {
                cout << "The file is empty\n";
            }
        }
        else {
            cout << "Error opening file " << fileName << ". There is no such file in your PC.\n";
        }
        //saveState();
    }

    void loadTextFromFile() {
        string fileName;
        cout << "Enter the file name for loading:\n";
        cin >> fileName;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        FILE* file = fopen(fileName.c_str(), "r");
        if (file != nullptr) {
            fseek(file, 0, SEEK_END); //to find file size
            int fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (fileSize == 0) {
                cout << "The file " << fileName << " is empty\n";
                fclose(file);
            }

            char* loadedText = (char*)calloc((fileSize + 1), sizeof(char));
            fread(loadedText, sizeof(char), fileSize, file);
            loadedText[fileSize] = '\0';

            free(text);
            text = loadedText;
            textSize = fileSize + 1;
            textLen = strlen(text);

            cout << "Text has been loaded successfully\n";
            fclose(file);

            //free(loadedText);
            //loadedText = nullptr;
        }
        else {
            cout << "Error opening file " << fileName << ". There is no such file in your PC.\n";
        }
    }

    void printText() {
        cout << "Text:\n";
        //cursor.printCursor(text);
        cout << text << endl;
    }

    void clearConsole() {
        cout << "Press '1' to clear the screen\n";
        int enter;
        cin >> enter;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
        if (enter == 1) {
            system("cls");
            cout << "Condole was cleared\n";
        }
    }

    void searchPosition() {
        cout << "Please write text to find out its location:\n";
        char input[20];
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';
        int inputLen = strlen(input);

        int line = 0;
        int i = 0;
        int column = 0;
        int count = 0;

        while (text[i] != '\0') {
            if (text[i] == '\n') {
                line++;
                i++;
                column = 0;
                continue;
            }

            if (strncmp(&text[i], input, inputLen) == 0) {
                cout << "Input string was found at ["<< line <<"]["<< column <<"] coordinate\n";
                count++;
                i += inputLen;
                column += inputLen;
            }
            else {
                i++;
                column++;
            }
        }

        if (count == 0) {
            cout << "Input string was not found\n";
        }
    }

    void appendByCoordinate() {
        int line = 0;
        int i = 0;
        int column = 0;

        while (text[i] != '\0') {
            if (text[i] == '\n') {
                line++;
                column = 0;
            }
            else {
                column++;
            }
            i++;
        }

        cout << "There are " << line + 1 << " lines and " << column << " columns to enter text by coordinates\n";
        cout << "But please start your counting from 0\n";
        cout << "Else text will be added in the beginning\n";

        cout << "Enter the line number:\n";
        char lineNumberInput[10];
        fgets(lineNumberInput, sizeof(lineNumberInput), stdin);
        lineNumberInput[strcspn(lineNumberInput, "\n")] = '\0';

        cout <<"Enter the column number:\n";
        char columnNumberInput[10];
        fgets(columnNumberInput, sizeof(columnNumberInput), stdin);
        lineNumberInput[strcspn(lineNumberInput, "\n")] = '\0';

        //int lineNumber = stoi(lineNumberInput); //atoi -- convert str to int
        //int columnNumber = stoi(columnNumberInput);

        char input[30];
        cout << "Enter text you want to add:\n";
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        int inputLen = strlen(input);

        if (textLen + inputLen >= textSize) {
            textSize = (textLen + inputLen) * 2;
            text = (char *)realloc(text, textSize * sizeof(char));
        }

        int insertIndex = findIndex(line, column);

        // shift text to make room for the new text
        memmove(&text[insertIndex + inputLen], &text[insertIndex], (textLen - insertIndex) + 1);
        //вказівник на місце в масиві, вказівник на початок, к-сть байт на яку треба зсунути

        // insert the new text
        memcpy(&text[insertIndex], input, inputLen);
        //вказівник на місце в масиві де хочеми вставити, вказівник на початок масиву input, який містить текст,який ми хочемо вставити;
        //кількість байтів, які потрібно скопіювати з input у text. це довжина нового тексту, який ми вставляємо

        textLen += inputLen;
        saveState();
    }

    void deleteText() {
        cout << "Deleting...\n";
        cout << "Please count lines and columns from 0\n";

        cout << "Choose line:\n";
        string lineInput;
        getline(cin, lineInput);
        int line = stoi(lineInput);

        cout << "Choose column:\n";
        string columnInput;
        getline(cin, columnInput);
        int column = stoi(columnInput);

        cout << "Choose amount of symbols:\n";
        string amountInput;
        getline(cin, amountInput);
        int amount = stoi(amountInput);

        int deleteIndex = findIndex(line, column);

        //if (deleteIndex + amount <= textLen) {   якщо виходимо за межі то видаляє перші символи
        if (deleteIndex != -1 && deleteIndex + amount <= textLen) {
            //               to                        from                         number bytes
            memmove(&text[deleteIndex], &text[deleteIndex + amount], textLen - deleteIndex - amount + 1);
            textLen -= amount;
            cout << "\nDeleting process was executed successfully\n";
        }
        else {
            cout << "\nDeleting process was not executed successfully\n";
            cout << "You were outside of bound\n";
        }
        saveState();
    }

    void undo() {
        if (stackUndo.size() > 1) {
            stackRedo.push(stackUndo.top());
            stackUndo.pop();

            if (!stackUndo.empty()) {
                char* previousState = stackUndo.top();
                textLen = strlen(previousState);
                textSize = textLen;
                text = (char*)realloc(text, textSize + 1);

                strcpy(text, previousState);
                cout << "Last operation was undone\n";
            }
        }
        else {
            cout << "There are no operations to undo\n";
        }
    }

    void redo() {
        if (!stackRedo.empty()) {
            stackUndo.push(stackRedo.top());
            stackRedo.pop();

            char* previousState = stackUndo.top();
            textLen = strlen(previousState);
            textSize = textLen;
            text = (char*)realloc(text, textSize + 1);

            strcpy(text, previousState);
            cout << "Last operation was redone\n";
        }
        else {
            cout << "There are no operations to redo\n";
        }
    }

    void cut() {
        cout << "There are " << textLen << " symbols\n";
        int line, column, symbols;
        cout << "Enter line, column, and number of symbols separated by spaces:\n";
        cin >> line >> column >> symbols;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer

        int cutIndex = findIndex(line, column);

        if (cutIndex + symbols <= textLen) {
            buffer.store(&text[cutIndex], symbols);

            memmove(&text[cutIndex], &text[cutIndex + symbols], (textLen - cutIndex - symbols) + 1);
            textLen -= symbols;

            cout << "Cut text: " << buffer.getFromBuffer() << "\n";
            saveState();
        }
        else {
            cout << "Cut operation is out of bounds\n";
        }
    }

    void paste() {
        if (strlen(buffer.getFromBuffer()) == 0) {
            cout << "Buffer is empty. Nothing to paste\n";
            return;
        }

        int line, column;
        cout << "Enter line and column separated by spaces:\n";
        cin >> line >> column;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int pasteLine = 0;
        int pasteColumn = 0;
        int pasteIndex = 0;
        int i = 0;

        while (text[i] != '\0') {
            if (pasteLine == line && pasteColumn == column) {
                pasteIndex = i;
                break;
            }
            if (text[i] == '\n') {
                pasteLine++;
                pasteColumn = 0;
            } else {
                pasteColumn++;
            }
            i++;
        }

        if (text[i] == '\0' && pasteLine == line && pasteColumn == column) {
            pasteIndex = i;
        }

        int bufferLength = strlen(buffer.getFromBuffer());

        if (pasteIndex <= textLen) {
            memmove(&text[pasteIndex + bufferLength], &text[pasteIndex], textLen - pasteIndex + 1);

            strncpy(&text[pasteIndex], buffer.getFromBuffer(), bufferLength);
            textLen += bufferLength;

            cout << "Pasted text: " << buffer.getFromBuffer() << "\n";
            saveState();
        }
        else {
            cout << "Paste operation is out of bounds\n";
        }
        saveState();
    }

    void copy() {
        int line, column, symbols;
        cout << "Enter line, column, and number of symbols separated by spaces:\n";
        cin >> line >> column >> symbols;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int copyIndex = findIndex(line, column);

        if (copyIndex + symbols <= textLen) {
            buffer.store(&text[copyIndex], symbols);
            cout << "Copied text: " << buffer.getFromBuffer() << "\n";
        }
        else {
            cout << "Copy operation is out of bounds\n";
        }
    }

    void insert() {
        int line, column;
        cout << "Enter line and column separated by spaces:\n";
        cin >> line >> column;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string input;
        cout << "Enter text to insert:\n";
        getline(cin, input);

        int insertIndex = findIndex(line, column); //+1
        int inputLen = input.length();

        if (insertIndex >= textLen) {
            // if out of bond -> insert in the end
            // strcat(), memcpy() etc. work witc c_str()
            strcat(text, input.c_str());
        }
        else {
            if (textLen + inputLen >= textSize) {
                textSize = textLen + inputLen + 1;
                text = (char *)realloc(text, textSize * sizeof(char));
            }
            memcpy(&text[insertIndex], input.c_str(), inputLen);
        }

        textLen += inputLen;

        saveState();
        cout << "Text inserted and replaced successfully.\n";
    }
};

int main() {
    setbuf(stdout, 0);
    Text text; // destructor will be called when main() ends

    while (true) {
        cout << "\nChoose the command (1/2/3/4/5/6/7/8/9/undo/redo/cut/paste/copy/insert/exit):";
        char command[10];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            cout << "Program was cancelled\n";
            break;
        }
        else if (strcmp(command, "undo") == 0) {
            text.undo();
        }
        else if (strcmp(command, "redo") == 0) {
            text.redo();
        }
        else if (strcmp(command, "cut") == 0) {
            text.cut();
        }
        else if (strcmp(command, "paste") == 0) {
            text.paste();
            }
        else if (strcmp(command, "copy") == 0) {
            text.copy();
        }
        else if (strcmp(command, "insert") == 0) {
            text.insert();
        }
        else {
            switch (command[0]) {
                case '1':
                    text.appendText();
                    break;
                case '2':
                    text.startNewLine();
                    break;
                case '3':
                    text.saveTextToFile();
                    break;
                case '4':
                    text.loadTextFromFile();
                    break;
                case '5':
                    text.printText();
                    break;
                case '6':
                    text.appendByCoordinate();
                    break;
                case '7':
                    text.searchPosition();
                    break;
                case '8':
                    text.clearConsole();
                    break;
                case '9':
                    text.deleteText();
                    break;
                default:
                    cout << "Invalid command\n";
                    break;
            }
        }
    }

    //text.~Text();
    return 0;
}