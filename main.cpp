#include <iostream>
#include "Book.h"
#include <iomanip>
#include <vector>
#include <fstream>
#include "BTree.h"
#include "BPlusTree.h"
using namespace std;
int main() {
    BTree bTree;
    BPlusTree bPlusTree;
    // FIXME: used for testing. Delete in final code.
    int times = 0;
    ifstream bookList("goodreads_books.json");
    vector<Book*> books;
    string line;
    bool genres[5];
    // Values correspond to T/F values indicating whether books fits genres of
    // {0. Mystery, 1. Romance, 2. Science-Fiction, 3. Historical Fiction, 4. Fantasy}
    string title;
    int pages;
    bool inEnglish = false;
    bool hasGenre = false; // Is true if a book has one of the genres a user can choose to search for
    double rating;
    cout << fixed << setprecision(2); // Makes it so doubles print with two decimal places
    while(getline(bookList, line) && times < 54) {
        istringstream str(line);
        string token;
        // Determines if book is in English
        while (str >> token) {
            if (token == "\"language_code\":") {
                str >> token;
                if (token == "\"eng\"," || token == "\"\",") {
                    inEnglish = true;
                }
                else {
                    inEnglish = false;
                }
            }
            /* FIXME: Maybe find better way to do this. Possible store as integer value with binary value places
             corresponding to boolean values in current array to save memory */
            // Determines which of the chosen genres book is
            if (token == "\"popular_shelves\":") {
                while(token != "\"asin\":") {
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
                if (inEnglish && hasGenre && (pages != -1) && (rating != -1)) {
                    title = tempTitle.substr(0, tempTitle.size() - 1);
                    books.push_back(new Book(title, rating, pages, genres));
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
    for (Book* book : books) {
        bTree.Insert(*book);
    }
    cout <<"PRINTING";
    bTree.checkNodes();
    cout << "Top 10 Books Based on Highest Rating:" << endl;
    bTree.printTopBooks(10);
    // Delete the top books from the B+ tree

    // Free memory for the "books" vector and its elements (Book objects)
    for (Book* book : books) {
        delete book;
    }
    books.clear();
    return 0;
}
