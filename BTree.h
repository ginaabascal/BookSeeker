#include <vector>
#include <algorithm>
#include "Book.h"
#pragma once
using namespace std;
class BTree {
private:
    struct Node
    {
        bool isLeaf;
        int size;
        vector<Book*> books;
        vector<Node*> children;

        Node(bool leaf)
        {
            this->isLeaf = leaf;
            size = 0;
        }

    };

    Node* root;
    int T = 5; // B+ tree parameter

    void destroyTree(Node* node)
    {
        if (node == nullptr) {
            return;
        }

        if (!node->isLeaf) {
            for (Node* child: node->children) {
                destroyTree(child);
            }
        }

        delete node;
    }

    void splitChild(Node* parent, int childIndex)
    {
        if (parent == nullptr) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
            root->size = 1;
        }
        else {
            Node* child = parent->children[childIndex];
            Node* newNode = new Node(child->isLeaf);
            int midIndex = child->size / 2;

            // Copy elements to the new node without resizing the original child vector
            for (int i = midIndex + 1; i < child->size; i++) {
                newNode->books.push_back(child->books[i]);
            }
            // Remove the copied elements from the original child node
            child->books.erase(child->books.begin() + midIndex + 1, child->books.end());

            if (!child->isLeaf) {
                // Copy child pointers to the new node without resizing the original child vector
                for (int i = midIndex + 1; i <= child->size; i++) {
                    newNode->children.push_back(child->children[i]);
                }

                // Remove the copied child pointers from the original child node
                child->children.erase(child->children.begin() + midIndex + 1, child->children.end());
            }

            // Insert the new node into the parent's children vector
            parent->children.insert(parent->children.begin() + childIndex + 1, newNode);

            // Update parent's books vector with the appropriate book from the child node
            parent->books.insert(parent->books.begin() + childIndex, child->books[midIndex]);
            parent->size++;
            // Update sizes of child and new nodes
            child->books.erase(child->books.begin() + midIndex, child->books.end());
            child->size = midIndex;
            newNode->size = midIndex;

            if (parent->books.size() >= 2 * T - 1) {
                Node* newParent = nullptr;
                int pos = 0;
                if (parent != root) {
                    newParent = findParent(parent);
                    if (parent->books[0]->_pages >= newParent->books[newParent->size-1]->_pages) {
                        pos = newParent->children.size()-1;
                    }
                    for (int i = 0; i < newParent->size; i++) {
                        if (parent->books[0]->_pages < newParent->books[i]->_pages) {
                            if (i < newParent->children.size()) {
                                pos = i;
                                break;
                            }
                        }
                    }
                }
                splitChild(newParent, pos);
            }
        }
    }

    void insertNonFull(Node* node, Book* book) {
        int i = node->size - 1;
        if (node->isLeaf) {
            node->books.emplace_back(nullptr); // Temporary book for comparison
            // Shifts books that come after new book, assigns position for new book
            while (i >= 0 && book->_pages < node->books[i]->_pages) {
                node->books[i + 1] = node->books[i];
                i--;
            }
            node->books[i + 1] = book;
            node->size++;
        } else {
            while (i >= 0 && book->_pages < node->books[i]->_pages) {
                i--;
            }
            i++;

            if (node->children[i]->size == 2 * T - 1) {
                splitChild(node, i);
                if (book->_pages >= node->books[i]->_pages) {
                    i++;
                }
            }
            insertNonFull(node->children[i], book);
        }
    }

    void printCloseBooks(Node* node, int& count, int desiredGenre, int pageCount) {
        if (!node->isLeaf && pageCount > node->books[node->size - 1]->_pages){
            printCloseBooks(node->children[node->size -1], count, desiredGenre, pageCount);
        }
        for (int i = 0; i < node->size; i++) {
            if (!node->isLeaf && node->books[i]->_pages > pageCount) {
                printCloseBooks(node->children[i], count, desiredGenre, pageCount);
            }
        }
        if (count > 0) {
            for (int i = 0; i < node->size; i++) {
                if (node->books[i]->_genres[desiredGenre]) {
                    cout << "Title: " << node->books[i]->_title << ", Rating: " << node->books[i]->_rating
                    << ", Pages: "
                    << node->books[i]->_pages << ", Genres: ";
                    if (node->books[i]->_genres[0] == 1) {
                        std::cout << "Mystery ";
                    }
                    if (node->books[i]->_genres[1] == 1) {
                        std::cout << "Romance ";
                    }
                    if (node->books[i]->_genres[2] == 1) {
                        std::cout << "Science Fiction ";
                    }
                    if (node->books[i]->_genres[3] == 1) {
                        std::cout << "Historical-Fiction ";
                    }
                    if (node->books[i]->_genres[4] == 1) {
                        std::cout << "Fantasy";
                    }
                    cout << endl << endl;
                    count--;
                }
                if (count == 0){
                    break;
                }
            }
        }

    }

    // Finds and returns parent of node book will be inserted
    Node* findSpot(Book* book, Node* node)
    {
        Node* n = node;
        int i = 0;
        int pos = 0;
        while (!n->children[0]->isLeaf) {
            if (book->_pages >= n->books[n->books.size() - 1]->_pages) {
                pos = n->children.size() - 1;
            }
            else {
                for (i = 0; i < n->children.size(); i++) {
                    if (i < n->size && book->_pages < n->books[i]->_pages) {
                        if (!n->children[i]->isLeaf) {
                            pos = i;
                            break;
                        }
                    }
                }
            }
            n = n->children[pos];
        }
        return n;
    }

    // Finds and returns parent of a given node
    Node* findParent(Node* node)
    {
        Node* n = root;
        Node* parent;
        int i = 0;
        while (n != node) {
            int pos = 0;
            if (node->books[0]->_pages >= n->books[n->books.size() - 1]->_pages) {
                pos = n->children.size() - 1;
            }
            for (i = 0; i < n->children.size(); i++) {
                if (i < n->books.size() && node->books[0]->_pages < n->books[i]->_pages) {
                    pos = i;
                    break;
                }
            }
            parent = n;
            n = n->children[pos];
        }
        return parent;
    }

public:

    BTree() : root(nullptr) {}

    ~BTree() {
        destroyTree(root);
    }

    void Insert(Book* book) {
        if (root == nullptr) {
            root = new Node(true);
            root->books.push_back(book);
            root->size = 1;
        } else if (!root->isLeaf) {
            Node* parent = findSpot(book, root);
            int pos = parent->children.size() - 1;
            for(int i = 0; i < parent->size; i++) {
                if (book->_pages < parent->books[i]->_pages) {
                    if (i < parent->children.size()) {
                        pos = i;
                        break;
                    }
                }
            }
            if (parent->children[pos]->size == 2 * T - 2) {
                insertNonFull(parent->children[pos], book);
                splitChild(parent, pos);
            }
            else {
                insertNonFull(parent->children[pos], book);
            }
        }
        else
        {
            if (root->size == 2 * T - 1) {
                Node* newRoot = new Node(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0);
                root = newRoot;
                root->size = 1;
            }
            else {
                insertNonFull(root, book);
            }
        }
    }

    // Prints books with amount of pages close to given pageCount and with user inputted genre
    void printCloseBooks(int count, int desiredGenre, int pageCount) {
        if (root == nullptr) {
            cout << "No books in the tree." << endl;
            return;
        }
        printCloseBooks(root, count, desiredGenre, pageCount);
    }
};
