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
    }
};