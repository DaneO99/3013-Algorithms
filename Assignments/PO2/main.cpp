/**
 * This program uses a "getch" function which means "get character".
 * However, getch does not print the character to the terminal, it
 * lets you decide what to do based on what character you are pressing.
 *
 * You can test which characters are being pressed using their ascii values.
 *
 * An ascii table here should be helpful
 *      http://www.asciitable.com/
 *
 * Some integer values for some keys:
 *      LowerCase Letters   = 97(a) -> 122(z)
 *      UpperCase Letters   = 65(A) -> 90(Z)
 *      Enter Key           = 10
 *      Space Bar           = 32
 *
 *      Arrow Keys          = Have same values as some letters
 *                            so we can't distinguish between
 *                            the two (in this context).
 *
 * Code below is a basic example of using a "getch" function along with
 * searching an array of words for partial matches.
 *
 * https://repl.it/@rugbyprof/getchexample#main.cpp

 rang only depends on C++ standard library, unistd.h system header on unix and
 windows.h & io.h system headers on windows based systems. In other words, you
 don't need any 3rd party dependencies.
 */

#include "./headers/console.hpp"
#include "./headers/rang.hpp"
#include "./headers/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace rang;
using json = nlohmann::json;
using namespace std::chrono;

consoleSize console_size; // Used to store the size of console (width=cols and height=rows)

/**
 * Prints a long background row of gray, with text centered.
 * Not a good function since colors are hard coded, but using
 * rang, I don't want to invest the time to figure out how to
 * interact with rang's enumerated class types and ostream.
 */
void titleBar(string title, int length = console_size.width) 
{
    string padding = string((length / 2) - (title.size() / 2), ' ');
    title = padding + title + padding;
    cout << bgB::black << fg::gray << title << fg::reset << bg::reset << endl;
}

/**
 * Prints a horizontal bar of a given length
 */
void horizontalBar(int length = console_size.width) 
{
    string line = string(length, '_');
    cout << fg::gray << line << fg::reset << bg::reset << endl;
}

/**
 * Prints the current key pressed and the current substring to the screen.
 */
void printCurrent(char k, string word) 
{
    cout << fg::green << style::bold << "KeyPressed: \t\t" << style::reset << fgB::yellow;
    if (int(k) == 127) 
    {
        cout << "del";
    } 
    else 
    {
        cout << k;
    }
    cout << " = " << (int)k << fg::reset << endl;
    cout << fg::green << style::bold << "Current Substr: \t" << fg::reset << fgB::blue << word << fg::reset << style::reset << endl;
    cout << endl;
}

/**
 * Prints an error message to the screen.
 */
void errorMessage(string message) 
{
    cout << bgB::red << fgB::gray << message << fg::reset << bg::reset << endl;
    // sleep(1); // Sleep function not used here, uncomment if needed
}

/**
 * Finds partial matches in an array of strings and returns them. It
 * doesn't matter where in the string the match is.
 */
vector<string> partialMatch(const vector<string>& array, const string& substring) 
{
    vector<string> matches;

    if (substring.empty()) 
    {
        return matches;
    }

    for (const string& str : array) 
    {
        if (str.find(substring) == 0) 
        { // Check if the word starts with the substring
            matches.push_back(str);
        }
    }

    return matches;
}

/**
 * Given a word, print the substr underlined and red vs blue for the rest of the word.
 */
void printHighlightedSubstr(const string& word, const string& substr) 
{
    size_t loc = word.find(substr);
    if (loc != string::npos) {
        for (size_t i = 0; i < word.size(); ++i) 
        {
            if (i >= loc && i < loc + substr.size()) 
            {
                cout << fg::red << style::underline << word[i] << fg::blue << style::reset;
            } 
            else 
            {
                cout << fg::blue << word[i] << style::reset;
            }
        }
    } 
    else 
    {
        cout << fg::blue << word << style::reset;
    }
}

int main() 
{
    console_size = getConsoleSize(); // Get width and height of console
    char k;                          // Holder for character being typed
    string substr = "";              // Var to concatenate letters to

    // Load dictionary
    ifstream file("./data/dictionary.json");
    if (!file.is_open()) 
    {
        cerr << "Failed to open dictionary file." << endl;
        return 1;
    }

    json dictionary;
    file >> dictionary;
    file.close();

    // Extract words from dictionary
    vector<string> words;
    for (auto it = dictionary.begin(); it != dictionary.end(); ++it) 
    {
        words.push_back(it.key());
    }

    clearConsole();
    titleBar("Dictionary Search", console_size.width);

    while ((k = getch()) != 'Z') 
    {
        clearConsole();
        titleBar("Dictionary Search", console_size.width);

        auto start = high_resolution_clock::now(); // Start the timer

        if ((int)k == 127) 
        {
            if (substr.size() > 0)
            {
                substr.pop_back();
            }
        } 
        else 
        {
            if (!isalpha(k)) 
            {
                errorMessage("Letters only!");
                continue;
            }
            if ((int)k < 97) 
            {
                k += 32;
            }
            substr += k; // Append char to substr
        }

        horizontalBar();
        printCurrent(k, substr);

        // Find partial matches in dictionary
        vector<string> matches = partialMatch(words, substr);

        auto stop = high_resolution_clock::now(); // Stop the timer
        auto duration = duration_cast<milliseconds>(stop - start); // Calculate duration
        // Print matches and duration
        cout << style::bold << style::underline << fg::black << "MATCHES: " << fg::green << matches.size() << style::reset << fg::reset << endl;
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
        for (const string& match : matches) 
        {
            printHighlightedSubstr(match, substr);
            cout << endl;
        }
    }
    return 0;
}