#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

template <class T>
class LinkedList 
{
private:
    struct Node 
    {
        T data;
        Node* next;
        Node(T value, Node* next = nullptr) : data(value), next(next) {}
    };
    Node* head;
    int length;
    void Clear() 
    {
        while (head) 
        {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        length = 0;
    }
    void CopyFrom(const LinkedList<T>& other) 
    {
        Node** current = &head;
        Node* otherCurrent = other.head;
        while (otherCurrent) 
        {
            *current = new Node(otherCurrent->data);
            current = &((*current)->next);
            otherCurrent = otherCurrent->next;
        }
        length = other.length;
    }
public:
    LinkedList() : head(nullptr), length(0) {}
    LinkedList(T* items, int count) : LinkedList() 
    {
        for (int i = 0; i < count; i++) 
        {
            Append(items[i]);
        }
    }
    LinkedList(const LinkedList<T>& other) : LinkedList() 
    {
        CopyFrom(other);
    }
    LinkedList(LinkedList<T>&& other) noexcept : head(other.head), length(other.length) 
    {
        other.head = nullptr;
        other.length = 0;
    }
    ~LinkedList() 
    {
        Clear();
    }
    LinkedList<T>& operator=(const LinkedList<T>& other) 
    {
        if (this != &other) 
        {
            Clear();
            CopyFrom(other);
        }
        return *this;
    }
    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept 
    {
        if (this != &other) 
        {
            Clear();
            head = other.head;
            length = other.length;
            other.head = nullptr;
            other.length = 0;
        }
        return *this;
    }
    T& operator[](int index) 
    {
        return Get(index);
    }
    const T& operator[](int index) const 
    {
        return Get(index);
    }
    T& Get(int index) 
    {
        if (index < 0 || index >= length) throw std::out_of_range("Индекс вне диапазона");
        Node* current = head;
        for (int i = 0; i < index; i++) 
        {
            current = current->next;
        }
        return current->data;
    }
    const T& Get(int index) const 
    {
        if (index < 0 || index >= length) throw std::out_of_range("Индекс вне диапазона");
        Node* current = head;
        for (int i = 0; i < index; i++) 
        {
            current = current->next;
        }
        return current->data;
    }
    T& GetFirst() 
    {
        if (!head) throw std::runtime_error("Список пуст");
        return head->data;
    }
    const T& GetFirst() const 
    {
        if (!head) throw std::runtime_error("Список пуст");
        return head->data;
    }
    T& GetLast() 
    {
        if (!head) throw std::runtime_error("Список пуст");
        Node* current = head;
        while (current->next) 
        {
            current = current->next;
        }
        return current->data;
    }
    const T& GetLast() const 
    {
        if (!head) throw std::runtime_error("Список пуст");
        Node* current = head;
        while (current->next) 
        {
            current = current->next;
        }
        return current->data;
    }
    LinkedList<T> GetSubList(int startIndex, int endIndex) const 
    {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex) throw std::out_of_range("Invalid index range");
        LinkedList<T> sublist;
        Node* current = head;
        for (int i = 0; i <= endIndex; i++) 
        {
            if (i >= startIndex) 
            {
                sublist.Append(current->data);
            }
            current = current->next;
        }
        return sublist;
    }
    int GetLength() const 
    {
        return length;
    }
    void Append(T item) 
    {
        Node** current = &head;
        while (*current) 
        {
            current = &((*current)->next);
        }
        *current = new Node(item);
        length++;
    }
    void Prepend(T item) 
    {
        head = new Node(item, head);
        length++;
    }
    void InsertAt(T item, int index) 
    {
        if (index < 0 || index > length) throw std::out_of_range("Index out of range");
        Node** current = &head;
        for (int i = 0; i < index; i++) 
        {
            current = &((*current)->next);
        }
        *current = new Node(item, *current);
        length++;
    }
    LinkedList<T> Concat(const LinkedList<T>& other) const 
    {
        LinkedList<T> result(*this);
        Node* current = other.head;
        while (current) 
        {
            result.Append(current->data);
            current = current->next;
        }
        return result;
    }
};

#endif