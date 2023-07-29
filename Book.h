#include <vector>
#include <map>
#include <string>
#include <iostream>
#pragma once
struct Book {
    std::string _title;
    bool _genres[5]; // Values correspond to T/F values indicating whether books fits genres of
    // {0. Mystery, 1. Romance, 2. Science-Fiction, 3. Historical Fiction, 4. Fantasy}
    double _rating;
    int _pages;
    Book(std::string title, double rating, int pages, bool genres[5])
    {
        _title = title;
        _rating = rating;
        _pages = pages;
        memcpy(_genres, genres, 5);

        // Prints book info for testing and debugging, delete in final project
        std::cout << "title: " << title << ", rating: " << rating << ", pages: " << pages << ", genres: ";
        if (_genres[0] == 1)
        {
            std::cout << "Mystery ";
        }
        if (_genres[1] == 1)
        {
            std::cout << "Romance ";
        }
        if (_genres[2] == 1)
        {
            std::cout << "Science Fiction ";
        }
        if (_genres[3] == 1)
        {
            std::cout << "Historical-Fiction ";
        }if (_genres[4] == 1)
        {
            std::cout << "Fantasy";
        }
        std::cout << std::endl << std::endl;
    }
};