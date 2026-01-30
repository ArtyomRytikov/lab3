#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "binary_tree.h"
#include <functional>
#include <string>
#include <type_traits>

// Структура для сравнения пар по ключу
template<typename TKey, typename TValue>
struct PairComparator 
{
    static bool less(const std::pair<TKey, TValue>& a, const std::pair<TKey, TValue>& b) 
    {
        return a.first < b.first;
    }
    
    static bool equal(const std::pair<TKey, TValue>& a, const std::pair<TKey, TValue>& b) 
    {
        return a.first == b.first;
    }
};

// Специализация для DynamicArray<char>
template<>
struct PairComparator<DynamicArray<char>, int> 
{
    static bool less(const std::pair<DynamicArray<char>, int>& a, const std::pair<DynamicArray<char>, int>& b) 
    {
        int minSize;
        if (a.first.GetSize() < b.first.GetSize()) 
        {
            minSize = a.first.GetSize();
        } 
        else 
        {
            minSize = b.first.GetSize();
        }
        for (int i = 0; i < minSize; i++) 
        {
            if (a.first.Get(i) < b.first.Get(i)) return true;
            if (b.first.Get(i) < a.first.Get(i)) return false;
        }
        return a.first.GetSize() < b.first.GetSize();
    }
    
    static bool equal(const std::pair<DynamicArray<char>, int>& a, const std::pair<DynamicArray<char>, int>& b) 
    {
        if (a.first.GetSize() != b.first.GetSize()) return false;
        for (int i = 0; i < a.first.GetSize(); i++) {
            if (a.first.Get(i) != b.first.Get(i)) return false;
        }
        return true;
    }
};

template<typename TKey, typename TValue>
class DictionaryPair 
{
private:
    std::pair<TKey, TValue> pair;
    
public:
    DictionaryPair() = default;
    DictionaryPair(const TKey& key, const TValue& value) : pair(key, value) {}
    DictionaryPair(const std::pair<TKey, TValue>& p) : pair(p) {}
    
    const TKey& getKey() const { return pair.first; }
    const TValue& getValue() const { return pair.second; }
    TValue& getValue() { return pair.second; }
    
    bool operator<(const DictionaryPair& other) const 
    {
        return PairComparator<TKey, TValue>::less(pair, other.pair);
    }
    
    bool operator==(const DictionaryPair& other) const 
    {
        return PairComparator<TKey, TValue>::equal(pair, other.pair);
    }
    
    operator std::pair<TKey, TValue>() const 
    {
        return pair;
    }
};

template <typename TKey, typename TValue>
class Dictionary 
{
private:
    BinaryTree<DictionaryPair<TKey, TValue>> tree;

public:
    Dictionary() = default;

    void Add(TKey key, TValue value) 
    {
        DictionaryPair<TKey, TValue> pair(key, value);
        DictionaryPair<TKey, TValue> searchPair(key, TValue());
        if (tree.contains(searchPair)) 
        {
            tree.remove(searchPair);
        }
        tree.insert(pair);
    }

    bool ContainsKey(TKey key) const 
    {
        DictionaryPair<TKey, TValue> searchPair(key, TValue());
        return tree.contains(searchPair);
    }

    TValue Get(TKey key) const 
    {
        DictionaryPair<TKey, TValue> searchPair(key, TValue());
        auto node = tree.findNode(searchPair);
        if (node != nullptr) {
            return node->data.getValue();
        }
        throw std::runtime_error("Key not found");
    }

    void Remove(TKey key) 
    {
        DictionaryPair<TKey, TValue> searchPair(key, TValue());
        tree.remove(searchPair);
    }

    int GetCount() const 
    {
        int count = 0;
        tree.traverseInOrder([&count](const DictionaryPair<TKey, TValue>& pair) 
        {
            count++;
        });
        return count;
    }

    bool IsEmpty() const 
    {
        return tree.isEmpty();
    }

    void Clear() 
    {
        tree.clear();
    }

    // Для получения всех ключей
    template<typename Func>
    void ForEachKey(Func func) const 
    {
        tree.traverseInOrder([func](const DictionaryPair<TKey, TValue>& pair) 
        {
            func(pair.getKey());
        });
    }

    // Для получения всех значений
    template<typename Func>
    void ForEachValue(Func func) const 
    {
        tree.traverseInOrder([func](const DictionaryPair<TKey, TValue>& pair) 
        {
            func(pair.getValue());
        });
    }

    // Для получения всех пар ключ-значение
    template<typename Func>
    void ForEach(Func func) const 
    {
        tree.traverseInOrder([func](const DictionaryPair<TKey, TValue>& pair) 
        {
            func(pair.getKey(), pair.getValue());
        });
    }

    // Получение всех ключей в виде последовательности
    Sequence<TKey>* GetKeys() const 
    {
        auto keys = new MutableArraySequence<TKey>();
        ForEachKey([keys](const TKey& key) 
        {
            keys->Append(key);
        });
        return keys;
    }

    // Получение всех значений в виде последовательности
    Sequence<TValue>* GetValues() const 
    {
        auto values = new MutableArraySequence<TValue>();
        ForEachValue([values](const TValue& value) 
        {
            values->Append(value);
        });
        return values;
    }

    // Проверка наличия значения
    bool ContainsValue(TValue value) const 
    {
        bool found = false;
        ForEachValue([&found, value](const TValue& val) 
        {
            if (val == value) 
            {
                found = true;
            }
        });
        return found;
    }

    // Балансировка словаря
    void Balance() 
    {
        tree.balance();
    }
};

#endif