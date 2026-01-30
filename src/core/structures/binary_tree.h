#ifndef BINARY_TREE_BINARY_TREE_H
#define BINARY_TREE_BINARY_TREE_H

#include "Sequence.h"
#include "LinkedList.h"
#include "DynamicArray.h"
#include <iostream>
#include <stdexcept>
#include <functional>

template <typename T>
struct Node 
{
    T data;
    Node* left;
    Node* right;
    
    Node(const T& value) : data(value), left(nullptr), right(nullptr) {}
};

template <typename T>
class BinaryTree 
{
private:
    Node<T>* root;

    Node<T>* insertRecursive(Node<T>* node, const T& value)
    {
        if (node == nullptr) 
        {
            return new Node<T>(value);
        }

        if (value < node->data) 
        {
            node->left = insertRecursive(node->left, value);
        } 
        else if (node->data < value) 
        {
            node->right = insertRecursive(node->right, value);
        }

        return node;
    }
    
    Node<T>* removeRecursive(Node<T>* node, const T& value)
    {
        if (node == nullptr) return nullptr;

        if (value < node->data) 
        {
            node->left = removeRecursive(node->left, value);
        } 
        else if (node->data < value) 
        {
            node->right = removeRecursive(node->right, value);
        } 
        else 
        {
            if (node->left == nullptr) 
            {
                Node<T>* temp = node->right;
                delete node;
                return temp;
            } 
            else if (node->right == nullptr) 
            {
                Node<T>* temp = node->left;
                delete node;
                return temp;
            }

            Node<T>* temp = findMin(node->right);
            node->data = temp->data;
            node->right = removeRecursive(node->right, temp->data);
        }
        return node;
    }
    
    Node<T>* findMin(Node<T>* node) const
    {
        while (node != nullptr && node->left != nullptr) 
        {
            node = node->left;
        }
        return node;
    }
    
    bool containsRecursive(Node<T>* node, const T& value) const
    {
        if (node == nullptr) return false;
        if (node->data == value) return true;
        if (value < node->data) return containsRecursive(node->left, value);
        return containsRecursive(node->right, value);
    }
    
    void clearRecursive(Node<T>* node)
    {
        if (node != nullptr) 
        {
            clearRecursive(node->left);
            clearRecursive(node->right);
            delete node;
        }
    }
    
    Node<T>* copyTree(Node<T>* node) const
    {
        if (node == nullptr) return nullptr;
        Node<T>* newNode = new Node<T>(node->data);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        return newNode;
    }
    
    // Методы для балансировки
    void storeInOrder(Node<T>* node, MutableArraySequence<T>& arr) const
    {
        if (node == nullptr) return;
        storeInOrder(node->left, arr);
        arr.Append(node->data);
        storeInOrder(node->right, arr);
    }
    
    Node<T>* buildBalancedTree(const MutableArraySequence<T>& arr, int start, int end)
    {
        if (start > end) return nullptr;
        
        int mid = (start + end) / 2;
        Node<T>* node = new Node<T>(arr.Get(mid));
        
        node->left = buildBalancedTree(arr, start, mid - 1);
        node->right = buildBalancedTree(arr, mid + 1, end);
        
        return node;
    }

    // Методы для сериализации
    void valueToString(const T& value, DynamicArray<char>& buffer) const
    {
        std::string str = std::to_string(value);
        for (char c : str) 
        {
            buffer.InsertAt(c, buffer.GetSize());
        }
        buffer.InsertAt(' ', buffer.GetSize());
    }
    
    T stringToValue(const DynamicArray<char>& buffer, int& pos) const
    {
        std::string str;
        while (pos < buffer.GetSize() && buffer[pos] != ' ') 
        {
            str += buffer[pos];
            pos++;
        }
        pos++;
        if constexpr (std::is_same<T, int>::value) 
        {
            return std::stoi(str);
        }
        else if constexpr (std::is_same<T, double>::value) 
        {
            return std::stod(str);
        }
        else if constexpr (std::is_same<T, float>::value) 
        {
            return std::stof(str);
        }
        else 
        {
            return T(str);
        }
    }
    
    void serializePreOrder(Node<T>* node, DynamicArray<char>& buffer) const
    {
        if (node == nullptr) return;
        valueToString(node->data, buffer);
        serializePreOrder(node->left, buffer);
        serializePreOrder(node->right, buffer);
    }
    
    void serializeInOrder(Node<T>* node, DynamicArray<char>& buffer) const
    {
        if (node == nullptr) return;
        serializeInOrder(node->left, buffer);
        valueToString(node->data, buffer);
        serializeInOrder(node->right, buffer);
    }
    
    void serializePostOrder(Node<T>* node, DynamicArray<char>& buffer) const
    {
        if (node == nullptr) return;
        serializePostOrder(node->left, buffer);
        serializePostOrder(node->right, buffer);
        valueToString(node->data, buffer);
    }
    
    bool compareFormats(const DynamicArray<char>& format1, const DynamicArray<char>& format2) const
    {
        if (format1.GetSize() != format2.GetSize()) return false;
        for (int i = 0; i < format1.GetSize(); i++) 
        {
            if (format1[i] != format2[i]) return false;
        }
        return true;
    }

    class TreeIterator : public IIterator<T> 
    {
    private:
        const BinaryTree<T>* tree;
        MutableArraySequence<T> elements;
        int currentIndex;

        void collectElements(Node<T>* node) 
        {
            if (node == nullptr) return;
            collectElements(node->left);
            elements.Append(node->data);
            collectElements(node->right);
        }

    public:
        TreeIterator(const BinaryTree<T>* bt) : tree(bt), currentIndex(0) 
        {
            collectElements(tree->root);
        }

        T GetCurrentItem() const override 
        {
            if (currentIndex < 0 || currentIndex >= elements.GetLength()) 
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return elements.Get(currentIndex);
        }

        bool HasNext() const override 
        {
            return currentIndex < elements.GetLength();
        }

        void Next() override 
        {
            if (HasNext()) 
            {
                currentIndex++;
            }
        }

        void Reset() override 
        {
            currentIndex = 0;
        }
    };

public:
    BinaryTree() : root(nullptr) {}
    
    BinaryTree(const BinaryTree& other) 
    {
        root = copyTree(other.root);
    }
    
    BinaryTree& operator=(const BinaryTree& other) 
    {
        if (this != &other) 
        {
            clear();
            root = copyTree(other.root);
        }
        return *this;
    }
    
    ~BinaryTree() 
    {
        clear();
    }

    void insert(const T& value) 
    {
        root = insertRecursive(root, value);
    }
    
    void remove(const T& value) 
    {
        root = removeRecursive(root, value);
    }
    
    bool contains(const T& value) const 
    {
        return containsRecursive(root, value);
    }
    
    bool isEmpty() const 
    { 
        return root == nullptr; 
    }
    
    void clear() 
    {
        clearRecursive(root);
        root = nullptr;
    }
    
    Node<T>* getRoot() const 
    { 
        return root; 
    }

    void balance() 
    {
        if (root == nullptr) return;
        
        MutableArraySequence<T> elements;
        storeInOrder(root, elements);
        
        clear();
        root = buildBalancedTree(elements, 0, elements.GetLength() - 1);
    }

    // Обходы
    template <typename Func>
    void traversePreOrder(Func func) const 
    {
        struct TraversalHelper 
        {
            static void preOrder(Node<T>* node, Func func) 
            {
                if (node == nullptr) return;
                func(node->data);
                preOrder(node->left, func);
                preOrder(node->right, func);
            }
        };
        
        TraversalHelper::preOrder(root, func);
    }
    
    template <typename Func>
    void traverseInOrder(Func func) const 
    {
        struct TraversalHelper 
        {
            static void inOrder(Node<T>* node, Func func) 
            {
                if (node == nullptr) return;
                inOrder(node->left, func);
                func(node->data);
                inOrder(node->right, func);
            }
        };
        
        TraversalHelper::inOrder(root, func);
    }
    
    template <typename Func>
    void traversePostOrder(Func func) const 
    {
        struct TraversalHelper {
            static void postOrder(Node<T>* node, Func func) 
            {
                if (node == nullptr) return;
                postOrder(node->left, func);
                postOrder(node->right, func);
                func(node->data);
            }
        };
        
        TraversalHelper::postOrder(root, func);
    }

    // Функциональные операции
    BinaryTree<T> map(std::function<T(T)> func) const
    {
        BinaryTree<T> result;
        traversePreOrder([&result, &func](const T& value) 
        {
            result.insert(func(value));
        });
        return result;
    }
    
    BinaryTree<T> where(std::function<bool(T)> predicate) const 
    {
        BinaryTree<T> result;
        traversePreOrder([&result, &predicate](const T& value) 
        {
            if (predicate(value)) 
            {
                result.insert(value);
            }
        });
        return result;
    }
    
    template <typename R>
    R reduce(std::function<R(R, T)> func, R initial) const 
    {
        R result = initial;
        traverseInOrder([&result, &func](const T& value) 
        {
            result = func(result, value);
        });
        return result;
    }

    void merge(const BinaryTree<T>& other) 
    {
        other.traversePreOrder([this](const T& value) 
        {
            this->insert(value);
        });
    }
    
    BinaryTree<T> extractSubtree(const T& value) const 
    {
        BinaryTree<T> result;
        Node<T>* node = findNode(value);
        if (node != nullptr) 
        {
            result.root = copyTree(node);
        }
        return result;
    }
    
    bool containsSubtree(const BinaryTree<T>& subtree) const 
    {
        if (subtree.root == nullptr) return true;
        
        Node<T>* node = findNode(subtree.root->data);
        if (node == nullptr) return false;
        
        DynamicArray<char> thisSubtree = extractSubtree(subtree.root->data).serialize();
        DynamicArray<char> otherSubtree = subtree.serialize();
        
        if (thisSubtree.GetSize() != otherSubtree.GetSize()) return false;
        for (int i = 0; i < thisSubtree.GetSize(); i++) 
        {
            if (thisSubtree[i] != otherSubtree[i]) return false;
        }
        return true;
    }
    
    Node<T>* findNode(const T& value) const 
    {
        Node<T>* current = root;
        while (current != nullptr) 
        {
            if (current->data == value) return current;
            if (value < current->data) 
            {
                current = current->left;
            } 
            else 
            {
                current = current->right;
            }
        }
        return nullptr;
    }

    // Сериализация
    DynamicArray<char> serialize(const DynamicArray<char>& format) const 
    {
        DynamicArray<char> buffer;
        
        // Проверяем формат
        DynamicArray<char> klpFormat;
        klpFormat.InsertAt('K', 0);
        klpFormat.InsertAt('L', 1);
        klpFormat.InsertAt('P', 2);
        
        DynamicArray<char> lkpFormat;
        lkpFormat.InsertAt('L', 0);
        lkpFormat.InsertAt('K', 1);
        lkpFormat.InsertAt('P', 2);
        
        DynamicArray<char> lpkFormat;
        lpkFormat.InsertAt('L', 0);
        lpkFormat.InsertAt('P', 1);
        lpkFormat.InsertAt('K', 2);
        
        if (compareFormats(format, klpFormat)) 
        {
            serializePreOrder(root, buffer);
        }
        else if (compareFormats(format, lkpFormat)) 
        {
            serializeInOrder(root, buffer);
        }
        else if (compareFormats(format, lpkFormat)) 
        {
            serializePostOrder(root, buffer);
        }
        else 
        {
            throw std::invalid_argument("Неизвестный формат сериализации");
        }
        
        return buffer;
    }
    
    DynamicArray<char> serialize() const 
    {
        DynamicArray<char> format;
        format.InsertAt('K', 0);
        format.InsertAt('L', 1);
        format.InsertAt('P', 2);
        return serialize(format);
    }
    
    void deserialize(const DynamicArray<char>& str, const DynamicArray<char>& format) 
    {
        clear();
        
        int pos = 0;
        while (pos < str.GetSize()) 
        {
            try 
            {
                T value = stringToValue(str, pos);
                insert(value);
            } 
            catch (...) 
            {
                break;
            }
        }
    }
    
    void deserialize(const DynamicArray<char>& str) 
    {
        DynamicArray<char> format;
        format.InsertAt('K', 0);
        format.InsertAt('L', 1);
        format.InsertAt('P', 2);
        deserialize(str, format);
    }

    IIterator<T>* CreateIterator() const 
    {
        return new TreeIterator(this);
    }
};

#endif
