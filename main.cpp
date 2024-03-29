#include <iostream>
#include "Book.h"
#include <iomanip>
#include <vector>
#include <fstream>
#include "BTree.h"
#include "BPlusTree.h"
#include <chrono>
using namespace std;

int main() {
    BTree bTree;
    BPlusTree bPlusTree;

    int times = 0;
    ifstream bookList("goodreads_books.json");
    vector<Book*> books;
    vector<bool> pageNumbersVisited(10000);
    string line;
    bool* genres = new bool[5]{0};

    // Values correspond to T/F values indicating whether books fits genres of
    // {0. Mystery, 1. Romance, 2. Science-Fiction, 3. Historical Fiction, 4. Fantasy}
    string title;
    int pages;
    bool inEnglish = false, hasGenre = false; // Is true if a book has one of the genres a user can choose to search for
    double rating;

    cout << fixed << setprecision(2); // Makes it so doubles print with two decimal places
    cout << "Inserting nodes..." << endl;
    while(getline(bookList, line) && times < 1000) {
        istringstream str(line);
        string token;
        // Determines if book is in English
        while (str >> token) {
            if (token == "\"language_code\":") {
                str >> token;
                if (token == "\"eng\",") {
                    inEnglish = true;
                }
                else {
                    inEnglish = false;
                }
            }

            // Determines which of the chosen genres book is
            if (token == "\"popular_shelves\":") {
                while (token != "\"asin\":") {
                    str >> token;
                    if (token == "\"name\":") {
                        str >> token;
                        if (token.find("mystery") != string::npos) {
                            genres[0] = true;
                            hasGenre = true;
                        }
                        if (token.find("romance") != string::npos) {
                            genres[1] = true;
                            hasGenre = true;
                        }
                        if (token == "\"science-fiction\"},") {
                            genres[2] = true;
                            hasGenre = true;
                        }
                        if (token == "\"historical-fiction\"},") {
                            genres[3] = true;
                            hasGenre = true;
                        }
                        if (token.find("fantasy") != string::npos) {
                            genres[4] = true;
                            hasGenre = true;
                        }
                    }
                }
            }

            // Gets average rating of book
            if (token == "\"average_rating\":") {
                try {
                    str >> token;
                    token.erase(0, 1);
                    token.erase(4, 2);
                    rating = stod(token);
                }
                // If book does not have rating available, sets value to -1
                catch (out_of_range& oor) {
                    rating = -1;
                }
            }
            // Gets the number of pages of the book
            if(token == "\"num_pages\":") {
                try {
                    str >> token;
                    token.erase(0, 1);
                    token.erase(token.end() - 1);
                    token.erase(token.end() - 1);
                    pages = stoi(token);
                }
                // If book does not have number of pages available, sets value to -1
                catch(invalid_argument& ia) {
                    pages = -1;
                }
            }
            // Gets the title of the book words by word, storing each word in vector.
            if (token == "\"title\":") {
                vector<string> titleVec;
                while(token.find("\",") == string::npos) {
                    str >> token;
                    titleVec.push_back(token);
                }
                titleVec[titleVec.size()-1].erase(titleVec[titleVec.size()-1].end() - 1);
                string tempTitle;
                // Adds each word in the vector to the title string
                for (auto word : titleVec) {
                    tempTitle += word;
                    tempTitle += ' ';
                }
                // Creates and object and adds to vector of books if language is English, it contains a searchable genre
                // page count is available, and rating is available

                if (inEnglish && hasGenre && tempTitle[1] != '\\' && (pages != -1) && (rating != -1) && times < 1000 && !pageNumbersVisited[pages]) {
                        title = tempTitle.substr(0, tempTitle.size() - 1);
                        Book *b = new Book(title, rating, pages, genres);
                        bPlusTree.Insert(b);
                        bTree.Insert(b);
                        pageNumbersVisited[pages] = true;
                        times++;
                }
                // Resets genre array and hasGenre bool
                hasGenre = false;
                for (int i = 0; i < 5; i++) {
                    genres[i] = false;
                }
            }
        }
    }
    int desiredGenre;
    int hours;
    int minutes;
    int readingSpeed;
    int choice;

    // Gets user input and on a loop until user decides to exit
    while (true) {
        cout << "Enter number corresponding to desired genre (1. Mystery 2. Romance 3. Sci-fi 4. Historical Fiction 5. Fantasy): ";
        cin >> desiredGenre;
        while (!cin || desiredGenre < 1 || desiredGenre > 5) {
            cout << "Invalid input. Please enter a number between 1 and 5: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> desiredGenre;
        }
        desiredGenre--;
        cout << endl << "On a scale of 1 to 4, with 4 being the fastest, How fast do you read? ";
        cin >> readingSpeed;
        while (!cin || readingSpeed < 1 || readingSpeed > 4) {
            cout << "Invalid input. Please enter a number between 1 and 4: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> readingSpeed;
        }
        cout << endl <<"How long would you like the book to take to read?" << endl << "Hours: ";
        cin >> hours;
        while (!cin|| hours < 0) {
            cout << "Invalid input. Please enter a positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> hours;
        }
        cout << "Minutes: ";
        cin >> minutes;
        while (!cin || minutes < 0 || minutes >= 60) {
            cout << "Invalid input. Please enter a number between 0 and 59: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> minutes;
        }
        int pageCount = minutes + hours * 60;
        // Reading speed stats from https://swiftread.com/reading-time/1-page
        if (readingSpeed == 1) {
            pageCount = (int)(pageCount/3.3);
        }
        else if (readingSpeed == 2) {
            pageCount = (int)(pageCount / 1.7);
        }
        else if (readingSpeed == 3) {
            pageCount = (int)(pageCount / 1.1);
        }
        else if (readingSpeed == 5) {
            pageCount = (int)(pageCount / (5.0/6.0));
        }
        cout << endl << "Reading for " << hours << " hours and " << minutes << " minutes at a speed level of " << readingSpeed <<
        " is equivalent to about " << pageCount << " pages" << endl << "Books with close to " << pageCount << " pages: " << endl << endl;
        cout << endl << "BTREE:" << endl;
        auto start = chrono::steady_clock::now();
        bTree.printCloseBooks(10, desiredGenre, pageCount);
        auto end = chrono::steady_clock::now();
        cout << "time taken to find books: " << chrono::duration_cast<chrono::microseconds >(end-start).count() << " microseconds" << endl;
        cout << endl << "BPlusTREE:" << endl;
        start = chrono::steady_clock::now();
        bPlusTree.printCloseBooks(10, desiredGenre, pageCount);
        end = chrono::steady_clock::now();
        cout << "time taken to find books: " << chrono::duration_cast<chrono::microseconds >(end-start).count() << " microseconds" << endl;
        cout << endl << "Enter 1 to search again or any other number to quit: ";
        cin >> choice;

        if (choice != 1) {
            break;
        }
    }

    return 0;
}
