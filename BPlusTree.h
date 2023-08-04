#include <vector>
#include <algorithm>
#include "Book.h"
using namespace std;

class BPlusTree {
private:
    struct Node {
        bool isLeaf;
        int size;
        vector<Book> books;
        vector<Node*> children;

        Node(bool leaf) {
            this->isLeaf = leaf;
            size = 0;
        }
    };

    Node* root;

    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        if (!node->isLeaf) {
            for (Node* child : node->children) {
                destroyTree(child);
            }
        }

        delete node;
    }

    void splitChild(Node* parent, int childIndex) {
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

        // Update sizes of child and new nodes
        child->size = midIndex;
        newNode->size = midIndex;
    }


    void insertNonFull(Node* node, const Book& book) {
        int i = node->size - 1;

        if (node->isLeaf) {
            node->books.push_back(Book("", 0.0, 0, nullptr)); // Temporary book for comparison
            while (i >= 0 && book._rating > node->books[i]._rating) {
                node->books[i + 1] = node->books[i];
                i--;
            }
            node->books[i + 1] = book;
            node->size++;
        } else {
            while (i >= 0 && book._rating < node->books[i]._rating) {
                i--;
            }
            i++;

            if (node->children[i]->size == 2 * T - 1) {
                splitChild(node, i);
                if (book._rating > node->books[i]._rating) {
                    i++;
                }
            }
            insertNonFull(node->children[i], book);
        }
    }

    void printTopBooks(Node* node, int& count) {
        if (node == nullptr) {
            return;
        }

        if (!node->isLeaf) {
            for (int i = 0; i < node->size + 1; i++) {
                printTopBooks(node->children[i], count);
            }
        }

        for (int i = 0; i < node->size; i++) {
            cout << "Title: " << node->books[i]._title << ", Rating: " << node->books[i]._rating << ", Pages: " << node->books[i]._pages << ", Genre: ";
            for (int j = 0; j < 5; j++) {
                if (node->books[i]._genres[j]) {
                    cout << j << " ";
                }
            }
            cout << endl;
            count--;
            if (count == 0) {
                return;
            }
        }
    }
    void printTopBooks(Node* node, int& count, vector<Book>& topBooks) {
        for (int i = 0; i < node->size; i++) {
            topBooks.push_back(node->books[i]);
            count--;
            if (count == 0) {
                return;
            }
        }
    }

    void deleteBook(Node* node, const Book& book) {
        // Recursive function to delete the book from the tree
        if (node == nullptr) {
            return;
        }
        int i = 0;
        while (i < node->size && book._rating > node->books[i]._rating) {
            i++;
        }
        if (node->isLeaf) {
            if (i < node->size && book._rating == node->books[i]._rating) {
                node->books.erase(node->books.begin() + i);
                node->size--;
            }
        } else {
            if (i < node->size && book._rating == node->books[i]._rating) {
                deleteBook(node->children[i + 1], book);
            } else {
                deleteBook(node->children[i], book);
            }
        }
    }

public:
    const static int T = 5; // B+ tree parameter

    BPlusTree() : root(nullptr) {}

    ~BPlusTree() {
        destroyTree(root);
    }

    void insert(const Book& book) {
        if (root == nullptr) {
            root = new Node(true);
            root->books.push_back(book);
            root->size = 1;
        } else {
            if (root->size == 2 * T - 1) {
                Node* newRoot = new Node(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0);
                root = newRoot;
            }
            insertNonFull(root, book);
        }
    }

    void printTopBooks(int count) {
        if (root == nullptr) {
            cout << "No books in the tree." << endl;
            return;
        }
        printTopBooks(root, count);
    }

    vector<Book> getTopBooks(int count) {
        vector<Book> topBooks;
        printTopBooks(root, count, topBooks);
        return topBooks;
    }

    void deleteBooks(const vector<Book>& booksToDelete) {
        for (const Book& book : booksToDelete) {
            deleteBook(root, book);
        }
    }
};
