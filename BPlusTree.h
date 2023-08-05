#include <vector>
#include "Book.h"
#pragma once
using namespace std;
class BPlusTree {
private:
    struct Node
    {
        vector<Book> books;
        int size = 0;
        vector<double> keys;
        vector<Node *> children;
        bool isLeaf = false;

        Node(bool leaf)
        {
            isLeaf = leaf;
        }
    };

    Node *root = nullptr;
    const static int T = 5;
public:
    void Insert(Book &book)
    {
        if (root == nullptr) {
            root = new Node(true);
            root->books.push_back(book);
            root->keys.push_back(book._pages);
            root->size = 1;
        } else if (!root->isLeaf) {
            Node *parent = findSpot(book, root);
            int pos = parent->children.size() - 1;
            for (int i = 0; i < parent->size; i++) {
                if (book._pages < parent->keys[i]) {
                    if (i < parent->children.size()) {
                        pos = i;
                        break;
                    }
                }
            }
            if (parent->children[pos]->size == 2 * T - 2) {
                insertNonFull(parent->children[pos], book);
                splitChild(parent, pos);
            } else {
                insertNonFull(parent->children[pos], book);
            }
        } else {
            if (root->size == 2 * T - 1) {
                Node *newRoot = new Node(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0);
                root = newRoot;
                root->size = 1;
            } else {
                insertNonFull(root, book);
            }
        }
    }

    Node *findSpot(Book &b, Node *r)
    {
        Node *n = r;
        int i = 0;
        int pos = 0;
        while (!n->children[0]->isLeaf) {
            if (b._pages >= n->keys[n->keys.size() - 1]) {
                pos = n->children.size() - 1;
            } else {
                for (i = 0; i < n->children.size(); i++) {
                    if (i < n->size && b._pages < n->keys[i]) {
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

    void insertNonFull(Node *node, const Book &book)
    {
        int i = node->size - 1;
        if (node->isLeaf) {
            bool arr[5] = {false, false, false, false, false};
            node->books.emplace_back(Book("", 0.0, 0, arr)); // Temporary book for comparison
            node->keys.emplace_back(0.0);
            while (i >= 0 && book._pages < node->keys[i]) {
                node->books[i + 1] = node->books[i];
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->books[i + 1] = book;
            node->keys[i + 1] = book._pages;
            node->size++;
        } else {
            while (i >= 0 && book._pages < node->books[i]._pages) {
                i--;
            }
            i++;

            if (node->children[i]->size == 2 * T - 1) {
                splitChild(node, i);
                if (book._pages >= node->books[i]._pages) {
                    i++;
                }
            }
            insertNonFull(node->children[i], book);
        }
    }

    void splitChild(Node *parent, int childIndex)
    {
        if (parent == nullptr) {
            Node *newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
            root->size = 1;
        } else {
            Node *child = parent->children[childIndex];
            Node *newNode = new Node(child->isLeaf);
            int midIndex = child->size / 2;
            if (child->keys[midIndex] == child->keys[midIndex - 1]) {
                cout << "equals";
                midIndex = midIndex - 1;
            }
            // Copy elements to the new node without resizing the original child vector
            for (int i = midIndex; i < child->size; i++) {
                if (child->isLeaf) {
                    newNode->books.push_back(child->books[i]);
                }
                newNode->keys.push_back(child->keys[i]);
            }
            // Remove the copied elements from the original child node
            if (child->isLeaf) {
                child->books.erase(child->books.begin() + midIndex, child->books.end());
            }
            child->keys.erase(child->keys.begin() + midIndex, child->keys.end());

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
            parent->keys.insert(parent->keys.begin() + childIndex, child->keys[midIndex]);
            if (!newNode->isLeaf) {
                newNode->keys.erase(newNode->keys.begin());
            }
            parent->size++;
            // Update sizes of child and new nodes
            child->size = child->keys.size();
            newNode->size = newNode->keys.size();

            if (parent->keys.size() >= 2 * T - 1) {
                Node *newParent = nullptr;
                int pos = 0;
                if (parent != root) {
                    newParent = findParent(parent);
                    if (parent->keys[0] >= newParent->keys[newParent->size - 1]) {
                        pos = newParent->children.size() - 1;
                    }
                    for (int i = 0; i < newParent->size; i++) {
                        if (parent->keys[0] < newParent->keys[i]) {
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

    Node *findParent(Node *node)
    {
        Node *n = root;
        Node *parent = nullptr;
        int i = 0;
        while (n != node) {
            int pos = 0;
            if (node->keys[0] >= n->keys[n->keys.size() - 1]) {
                pos = n->children.size() - 1;
            }
            for (i = 0; i < n->children.size(); i++) {
                if (i < n->keys.size() && node->keys[0] < n->keys[i]) {
                    pos = i;
                    break;
                }
            }
            parent = n;
            n = n->children[pos];
        }
        return parent;
    }
    void checkNodes()
    {
        for (auto key : root->keys)
        {
            cout << key << ", ";
        }
        cout << "||";
        cout << "LEVEL 1";
        for (auto child : root->children)
        {
            for (auto key : child->keys)
            {
                cout << key << ", ";
            }
            cout << "||";
        }
        cout << "LEVEL 2";
        for (auto child1 : root->children)
        {
            for (auto child : child1->children) {
                for (auto key: child->keys) {
                    cout << key << ", ";
                }
                cout << "||";
            }
            cout << "NEWROOT";
        }
        cout << "LEVEL 3";
        for (auto child2 : root->children) {
            for (auto child1: child2->children) {
                for (auto child: child1->children) {
                    for (auto key: child->keys) {
                        cout << key << ", ";
                    }
                    cout << "||";
                }
                cout << "NEWROOT";
            }
        }

        cout << "LEVEL 4";
        {
            for (auto child3 :root->children) {
                for (auto child2: child3->children) {
                    for (auto child1: child2->children) {
                        cout << "NEWROOT";
                        for (auto child: child1->children) {
                            for (auto key: child->keys) {
                                cout << key << ", ";
                            }
                            cout << "||";
                        }
                    }
                }
            }
        }
    }
    void printTopBooks(Node* node, int& count) {
        if (node->children.size() != 0)
        {
            for (int i = node->children.size() - 1; i >= 0; i--) {
                printTopBooks(node->children[i], count);
                if (count > 0 && node->isLeaf) {
                    cout << "Title: " << node->books[i]._title << ", Rating: " << node->books[i]._rating << ", Pages: "
                         << node->books[i]._pages << ", Genres: ";
                    if (node->books[i]._genres[0] == 1) {
                        std::cout << "Mystery ";
                    }
                    if (node->books[i]._genres[1] == 1) {
                        std::cout << "Romance ";
                    }
                    if (node->books[i]._genres[2] == 1) {
                        std::cout << "Science Fiction ";
                    }
                    if (node->books[i]._genres[3] == 1) {
                        std::cout << "Historical-Fiction ";
                    }
                    if (node->books[i]._genres[4] == 1) {
                        std::cout << "Fantasy";
                    }
                    cout << endl;
                    count--;
                }
            }
        }
        for (int i = node->size-1; i >= 0; i--)
        {
            if (count > 0 && node->isLeaf) {
                cout << "Title: " << node->books[i]._title << ", Rating: " << node->books[i]._rating << ", Pages: " << node->books[i]._pages << ", Genres: ";
                if (node->books[i]._genres[0] == 1)
                {
                    std::cout << "Mystery ";
                }
                if (node->books[i]._genres[1] == 1)
                {
                    std::cout << "Romance ";
                }
                if (node->books[i]._genres[2] == 1)
                {
                    std::cout << "Science Fiction ";
                }
                if (node->books[i]._genres[3] == 1)
                {
                    std::cout << "Historical-Fiction ";
                }if (node->books[i]._genres[4] == 1)
                {
                    std::cout << "Fantasy";
                }
                cout << endl;
                count--;
            }
        }

    }
    void printTopBooks(int count) {
        if (root == nullptr) {
            cout << "No books in the tree." << endl;
            return;
        }
        printTopBooks(root, count);
    }
};