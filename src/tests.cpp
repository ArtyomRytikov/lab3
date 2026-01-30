#include <gtest/gtest.h>
#include "core/structures/DynamicArray.h"
#include "core/structures/LinkedList.h"
#include "core/structures/Sequence.h"
#include "core/structures/binary_tree.h"
#include "core/structures/dictionary.h"
#include "core/structures/set.h"
#include "core/structures/sorted_sequence.h"
#include "core/structures/priority_queue.h"

#include "core/graph/GraphBase.h"
#include "core/graph/DirectedGraph.h"
#include "core/graph/UndirectedGraph.h"
#include "core/tasks/ConnectedComponents.h"
#include "core/tasks/PartialOrder.h"
#include "core/tasks/ShortestPath.h"
#include "core/tasks/TopologicalSort.h"

#include <functional>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <unordered_set>

extern "C" 
{
    void _Exit(int status) { exit(status); }
    void quick_exit(int status) { exit(status); }
}

// 1. Тесты DynamicArray

TEST(DynamicArrayTest, EmptyArray) 
{
    // Проверяет: Создание пустого массива
    DynamicArray<int> arr;
    EXPECT_EQ(arr.GetSize(), 0);
}

TEST(DynamicArrayTest, SizeConstructor) 
{
    // Проверяет: Создание массива заданного размера
    DynamicArray<int> arr(5);
    EXPECT_EQ(arr.GetSize(), 5);
}

TEST(DynamicArrayTest, ArrayConstructor) 
{
    // Проверяет: Создание массива из существующего массива данных
    int data[] = {1, 2, 3, 4, 5};
    DynamicArray<int> arr(data, 5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(4), 5);
}

TEST(DynamicArrayTest, CopyConstructorDeepCopy) 
{
    // Проверяет: Глубокое копирование через конструктор копирования
    DynamicArray<int> arr1(3);
    arr1.Set(0, 10);
    arr1.Set(1, 20);
    arr1.Set(2, 30);
    
    DynamicArray<int> arr2(arr1);
    EXPECT_EQ(arr2.GetSize(), 3);
    EXPECT_EQ(arr2.Get(0), 10);
    EXPECT_EQ(arr2.Get(1), 20);
    EXPECT_EQ(arr2.Get(2), 30);

    arr1.Set(0, 100);
    EXPECT_EQ(arr2.Get(0), 10);
}

TEST(DynamicArrayTest, MoveConstructor) 
{
    // Проверяет: Перемещающий конструктор
    DynamicArray<int> arr1(3);
    arr1.Set(0, 100);
    arr1.Set(1, 200);
    arr1.Set(2, 300);
    
    DynamicArray<int> arr2(std::move(arr1));
    EXPECT_EQ(arr2.GetSize(), 3);
    EXPECT_EQ(arr1.GetSize(), 0);
    EXPECT_EQ(arr2.Get(0), 100);
    EXPECT_EQ(arr2.Get(2), 300);
}

TEST(DynamicArrayTest, AssignmentOperatorDeepCopy) 
{
    // Проверяет: Оператор присваивания с глубоким копированием
    DynamicArray<int> arr1(2);
    arr1.Set(0, 5);
    arr1.Set(1, 15);
    
    DynamicArray<int> arr2;
    arr2 = arr1;
    EXPECT_EQ(arr2.GetSize(), 2);
    EXPECT_EQ(arr2.Get(1), 15);
    
    // Проверяем само-присваивание
    arr2 = arr2;
    EXPECT_EQ(arr2.GetSize(), 2);
}

TEST(DynamicArrayTest, MoveAssignmentOperator) 
{
    // Проверяет: Перемещающий оператор присваивания
    DynamicArray<int> arr1(2);
    arr1.Set(0, 25);
    arr1.Set(1, 35);
    
    DynamicArray<int> arr2;
    arr2 = std::move(arr1);
    EXPECT_EQ(arr2.GetSize(), 2);
    EXPECT_EQ(arr2.Get(0), 25);
    EXPECT_EQ(arr1.GetSize(), 0);
}

TEST(DynamicArrayTest, IndexOperatorReadWrite) 
{
    // Проверяет: Оператор [] для чтения и записи
    DynamicArray<int> arr(3);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    
    arr[1] = 20;
    EXPECT_EQ(arr[1], 20);
    
    const DynamicArray<int>& constArr = arr;
    EXPECT_EQ(constArr[0], 1);
}

TEST(DynamicArrayTest, ResizeOperationGrow) 
{
    // Проверяет: Увеличение размера массива
    DynamicArray<int> arr(3);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 3);
    
    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
}

TEST(DynamicArrayTest, ResizeOperationShrink) 
{
    // Проверяет: Уменьшение размера массива
    DynamicArray<int> arr(5);
    for (int i = 0; i < 5; i++) {
        arr.Set(i, i + 1);
    }
    
    arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
}

TEST(DynamicArrayTest, InsertAtBeginning) 
{
    // Проверяет: Вставку элемента в начало массива
    DynamicArray<int> arr(3);
    arr.Set(0, 2);
    arr.Set(1, 3);
    arr.Set(2, 4);
    
    arr.InsertAt(1, 0);
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
    EXPECT_EQ(arr.Get(3), 4);
}

TEST(DynamicArrayTest, InsertAtEnd) 
{
    // Проверяет: Вставку элемента в конец массива
    DynamicArray<int> arr(3);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 3);
    
    arr.InsertAt(4, 3);
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(3), 4);
}

TEST(DynamicArrayTest, InsertAtMiddle) 
{
    // Проверяет: Вставку элемента в середину массива
    DynamicArray<int> arr(4);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 4);
    arr.Set(3, 5);
    
    arr.InsertAt(3, 2);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
    EXPECT_EQ(arr.Get(3), 4);
    EXPECT_EQ(arr.Get(4), 5);
}

TEST(DynamicArrayTest, NegativeSizeThrows) 
{
    // Проверяет: Обработку отрицательного размера массива
    EXPECT_THROW(DynamicArray<int> arr(-1), std::invalid_argument);
}

TEST(DynamicArrayTest, OutOfRangeAccessThrows) 
{
    // Проверяет: Обработку выхода за границы массива
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Get(5), std::out_of_range);
    EXPECT_THROW(arr.Set(5, 10), std::out_of_range);
    EXPECT_THROW(arr[5] = 10, std::out_of_range);
    
    const DynamicArray<int>& constArr = arr;
    EXPECT_THROW(constArr.Get(5), std::out_of_range);
}

TEST(DynamicArrayTest, StringArrayOperations) 
{
    // Проверяет: Работу DynamicArray с строковыми типами
    DynamicArray<std::string> arr(2);
    arr.Set(0, "Hello");
    arr.Set(1, "World");
    
    EXPECT_EQ(arr.Get(0), "Hello");
    EXPECT_EQ(arr.Get(1), "World");
    
    arr.InsertAt("Middle", 1);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(1), "Middle");
}

TEST(DynamicArrayTest, LargeArrayPerformance) 
{
    // Проверяет: Производительность работы с большим массивом
    const int SIZE = 10000;
    DynamicArray<int> arr(SIZE);
    
    for (int i = 0; i < SIZE; i++) {
        arr.Set(i, i * 2);
    }
    
    EXPECT_EQ(arr.GetSize(), SIZE);
    
    // Проверяем случайные доступы
    EXPECT_EQ(arr.Get(0), 0);
    EXPECT_EQ(arr.Get(SIZE/2), SIZE);
    EXPECT_EQ(arr.Get(SIZE-1), (SIZE-1)*2);
}

// 2. Тесты LinkedList

TEST(LinkedListTest, EmptyList) 
{
    // Проверяет: Создание пустого списка
    LinkedList<int> list;
    EXPECT_EQ(list.GetLength(), 0);
    EXPECT_THROW(list.GetFirst(), std::runtime_error);
    EXPECT_THROW(list.GetLast(), std::runtime_error);
    EXPECT_THROW(list.Get(0), std::out_of_range);
}

TEST(LinkedListTest, AppendOperationMultiple) 
{
    // Проверяет: Добавление нескольких элементов в конец списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    list.Append(3);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
    EXPECT_EQ(list.Get(1), 2);
}

TEST(LinkedListTest, PrependOperationMultiple) 
{
    // Проверяет: Добавление нескольких элементов в начало списка
    LinkedList<int> list;
    list.Prepend(3);
    list.Prepend(2);
    list.Prepend(1);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
    EXPECT_EQ(list.Get(1), 2);
}

TEST(LinkedListTest, InsertAtBeginning) 
{
    // Проверяет: Вставку элемента в начало списка
    LinkedList<int> list;
    list.Append(2);
    list.Append(3);
    list.InsertAt(1, 0); 
    
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
}

TEST(LinkedListTest, InsertAtEnd) 
{
    // Проверяет: Вставку элемента в конец списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    list.InsertAt(3, 2);
    
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
}

TEST(LinkedListTest, InsertAtMiddle) 
{
    // Проверяет: Вставку элемента в середину списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(3);
    list.Append(4);
    list.InsertAt(2, 1);
    
    EXPECT_EQ(list.GetLength(), 4);
    EXPECT_EQ(list.Get(0), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
    EXPECT_EQ(list.Get(3), 4);
}

TEST(LinkedListTest, GetSubListValidRange) 
{
    // Проверяет: Получение подсписка в допустимом диапазоне
    LinkedList<int> list;
    for (int i = 0; i < 10; i++) {
        list.Append(i * 10);
    }
    
    LinkedList<int> sublist = list.GetSubList(2, 5);
    EXPECT_EQ(sublist.GetLength(), 4);
    EXPECT_EQ(sublist.Get(0), 20);
    EXPECT_EQ(sublist.Get(1), 30);
    EXPECT_EQ(sublist.Get(2), 40);
    EXPECT_EQ(sublist.Get(3), 50);
}

TEST(LinkedListTest, GetSubListSingleElement) 
{
    // Проверяет: Получение подсписка из одного элемента
    LinkedList<int> list;
    list.Append(10);
    list.Append(20);
    list.Append(30);
    
    LinkedList<int> sublist = list.GetSubList(1, 1);
    EXPECT_EQ(sublist.GetLength(), 1);
    EXPECT_EQ(sublist.Get(0), 20);
}

TEST(LinkedListTest, GetSubListInvalidRangeThrows) 
{
    // Проверяет: Обработку неверного диапазона для подсписка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    
    EXPECT_THROW(list.GetSubList(-1, 2), std::out_of_range);
    EXPECT_THROW(list.GetSubList(0, 3), std::out_of_range);
    EXPECT_THROW(list.GetSubList(2, 1), std::out_of_range);
}

TEST(LinkedListTest, ConcatOperationWithEmpty) 
{
    // Проверяет: Конкатенацию списка с пустым списком
    LinkedList<int> list1;
    list1.Append(1);
    list1.Append(2);
    
    LinkedList<int> list2;
    
    LinkedList<int> result = list1.Concat(list2);
    EXPECT_EQ(result.GetLength(), 2);
    EXPECT_EQ(result.GetFirst(), 1);
    EXPECT_EQ(result.GetLast(), 2);
}

TEST(LinkedListTest, ConcatOperationTwoNonEmpty) 
{
    // Проверяет: Конкатенацию двух непустых списков
    LinkedList<int> list1;
    list1.Append(1);
    list1.Append(2);
    
    LinkedList<int> list2;
    list2.Append(3);
    list2.Append(4);
    
    LinkedList<int> result = list1.Concat(list2);
    EXPECT_EQ(result.GetLength(), 4);
    EXPECT_EQ(result.Get(0), 1);
    EXPECT_EQ(result.Get(1), 2);
    EXPECT_EQ(result.Get(2), 3);
    EXPECT_EQ(result.Get(3), 4);
}

TEST(LinkedListTest, CopyConstructorDeepCopy) 
{
    // Проверяет: Глубокое копирование через конструктор копирования
    LinkedList<int> list1;
    list1.Append(10);
    list1.Append(20);
    list1.Append(30);
    
    LinkedList<int> list2(list1);
    EXPECT_EQ(list2.GetLength(), 3);
    EXPECT_EQ(list2.GetFirst(), 10);
    EXPECT_EQ(list2.GetLast(), 30);
    
    list1.Append(40);
    EXPECT_EQ(list1.GetLength(), 4);
    EXPECT_EQ(list2.GetLength(), 3);
}

TEST(LinkedListTest, MoveConstructor) 
{
    // Проверяет: Перемещающий конструктор
    LinkedList<int> list1;
    list1.Append(100);
    list1.Append(200);
    list1.Append(300);
    
    LinkedList<int> list2(std::move(list1));
    EXPECT_EQ(list2.GetLength(), 3);
    EXPECT_EQ(list1.GetLength(), 0);
    EXPECT_EQ(list2.GetFirst(), 100);
    EXPECT_EQ(list2.GetLast(), 300);
}

TEST(LinkedListTest, AssignmentOperatorDeepCopy) 
{
    // Проверяет: Оператор присваивания с глубоким копированием
    LinkedList<int> list1;
    list1.Append(100);
    list1.Append(200);
    
    LinkedList<int> list2;
    list2 = list1;
    EXPECT_EQ(list2.GetLength(), 2);
    EXPECT_EQ(list2.GetLast(), 200);
    
    // Проверяем само-присваивание
    list2 = list2;
    EXPECT_EQ(list2.GetLength(), 2);
}

TEST(LinkedListTest, MoveAssignmentOperator) 
{
    // Проверяет: Перемещающий оператор присваивания
    LinkedList<int> list1;
    list1.Append(25);
    list1.Append(35);
    
    LinkedList<int> list2;
    list2 = std::move(list1);
    EXPECT_EQ(list2.GetLength(), 2);
    EXPECT_EQ(list2.Get(0), 25);
    EXPECT_EQ(list1.GetLength(), 0);
}

TEST(LinkedListTest, IndexOperatorReadWrite) 
{
    // Проверяет: Оператор [] для чтения и записи
    LinkedList<int> list;
    list.Append(5);
    list.Append(15);
    list.Append(25);
    
    EXPECT_EQ(list[0], 5);
    EXPECT_EQ(list[2], 25);
    
    list[1] = 10;
    EXPECT_EQ(list.Get(1), 10);
    
    const LinkedList<int>& constList = list;
    EXPECT_EQ(constList[0], 5);
}

TEST(LinkedListTest, LargeLinkedList) 
{
    // Проверяет: Работу с большим связным списком
    LinkedList<int> list;
    const int COUNT = 1000;
    
    for (int i = 0; i < COUNT; i++) {
        list.Append(i);
    }
    
    EXPECT_EQ(list.GetLength(), COUNT);
    EXPECT_EQ(list.GetFirst(), 0);
    EXPECT_EQ(list.GetLast(), COUNT - 1);
    
    // Проверяем случайные доступы
    EXPECT_EQ(list.Get(500), 500);
    EXPECT_EQ(list.Get(999), 999);
}

// 3. Тесты Sequence

TEST(SequenceTest, ImmutableArraySequenceBasicOperations) 
{
    // Проверяет: Базовые операции ImmutableArraySequence
    int data[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> seq(data, 5);
    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.GetLast(), 5);
    EXPECT_EQ(seq.Get(2), 3);
    EXPECT_EQ(seq.GetLength(), 5);
}

TEST(SequenceTest, ImmutableArraySequenceEmpty) 
{
    // Проверяет: Поведение пустой ImmutableArraySequence
    ImmutableArraySequence<int> seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
    EXPECT_THROW(seq.Get(0), std::out_of_range);
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceValid) 
{
    // Проверяет: Получение корректной подпоследовательности
    int data[] = {1, 2, 3, 4, 5, 6, 7};
    ImmutableArraySequence<int> seq(data, 7);
    
    auto subseq = seq.GetSubsequence(2, 5);
    EXPECT_EQ(subseq->GetLength(), 4);
    EXPECT_EQ(subseq->GetFirst(), 3);
    EXPECT_EQ(subseq->GetLast(), 6);
    EXPECT_EQ(subseq->Get(1), 4);
    delete subseq;
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceSingle) 
{
    // Проверяет: Получение подпоследовательности из одного элемента
    int data[] = {10, 20, 30};
    ImmutableArraySequence<int> seq(data, 3);
    
    auto subseq = seq.GetSubsequence(1, 1);
    EXPECT_EQ(subseq->GetLength(), 1);
    EXPECT_EQ(subseq->GetFirst(), 20);
    delete subseq;
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceInvalid) 
{
    // Проверяет: Обработку неверных индексов подпоследовательности
    int data[] = {1, 2, 3};
    ImmutableArraySequence<int> seq(data, 3);
    
    EXPECT_THROW(seq.GetSubsequence(-1, 2), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(0, 3), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(2, 1), std::out_of_range);
}

TEST(SequenceTest, MutableArraySequenceAppend) 
{
    // Проверяет: Добавление элементов в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(1);
    seq.Append(2);
    seq.Append(3);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetLast(), 3);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(SequenceTest, MutableArraySequencePrepend) 
{
    // Проверяет: Добавление элементов в начало MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Prepend(3);
    seq.Prepend(2);
    seq.Prepend(1);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.GetLast(), 3);
}

TEST(SequenceTest, MutableArraySequenceInsertAt) 
{
    // Проверяет: Вставку элемента в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(1);
    seq.Append(3);
    seq.InsertAt(2, 1);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
}

TEST(SequenceTest, MutableArraySequenceSet) 
{
    // Проверяет: Установку значения в MutableArraySequence
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);
    
    seq.Set(1, 20);
    EXPECT_EQ(seq.Get(1), 20);
    
    EXPECT_THROW(seq.Set(5, 100), std::out_of_range);
}

TEST(SequenceTest, MutableArraySequenceGetRef) 
{
    // Проверяет: Получение ссылки на элемент в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(10);
    seq.Append(20);
    
    int& ref = seq.GetRef(1);
    ref = 30;
    EXPECT_EQ(seq.Get(1), 30);
}

TEST(SequenceTest, MutableArraySequenceIteratorFullTraversal) 
{
    // Проверяет: Полный обход через итератор MutableArraySequence
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);
    
    auto iterator = seq.CreateIterator();
    std::vector<int> collected;
    
    while (iterator->HasNext()) {
        collected.push_back(iterator->GetCurrentItem());
        iterator->Next();
    }
    
    EXPECT_EQ(collected.size(), 3);
    EXPECT_EQ(collected[0], 10);
    EXPECT_EQ(collected[1], 20);
    EXPECT_EQ(collected[2], 30);
    
    iterator->Reset();
    EXPECT_EQ(iterator->GetCurrentItem(), 10);
    
    delete iterator;
}

TEST(SequenceTest, ImmutableListSequenceBasic) 
{
    // Проверяет: Базовые операции ImmutableListSequence
    int data[] = {5, 10, 15, 20};
    ImmutableListSequence<int> seq(data, 4);
    EXPECT_EQ(seq.GetFirst(), 5);
    EXPECT_EQ(seq.GetLast(), 20);
    EXPECT_EQ(seq.GetLength(), 4);
    EXPECT_EQ(seq.Get(2), 15);
}

TEST(SequenceTest, MutableListSequenceModifications) 
{
    // Проверяет: Модификации MutableListSequence
    MutableListSequence<int> seq;
    seq.Append(100);
    seq.Append(200);
    seq.Prepend(50);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 50);
    EXPECT_EQ(seq.GetLast(), 200);
    
    seq.InsertAt(150, 2);
    EXPECT_EQ(seq.Get(2), 150);
}

TEST(SequenceTest, ListSequenceIterator) 
{
    // Проверяет: Итератор для ListSequence
    int data[] = {1, 2, 3};
    ImmutableListSequence<int> seq(data, 3);
    
    auto iterator = seq.CreateIterator();
    int sum = 0;
    int count = 0;
    
    while (iterator->HasNext()) {
        sum += iterator->GetCurrentItem();
        iterator->Next();
        count++;
    }
    
    EXPECT_EQ(sum, 6);
    EXPECT_EQ(count, 3);
    delete iterator;
}

TEST(SequenceTest, SequenceGetCount) 
{
    // Проверяет: Метод GetCount в Sequence
    int data[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> seq(data, 5);
    
    EXPECT_EQ(seq.GetCount(), 5);
    EXPECT_EQ(seq.GetLength(), 5);
}

TEST(SequenceTest, LargeMutableArraySequence) 
{
    // Проверяет: Работу с большой MutableArraySequence
    const int SIZE = 1000;
    MutableArraySequence<int> seq;
    
    for (int i = 0; i < SIZE; i++) {
        seq.Append(i);
    }
    
    EXPECT_EQ(seq.GetLength(), SIZE);
    
    // Проверяем что можем получить все элементы
    for (int i = 0; i < SIZE; i++) {
        EXPECT_EQ(seq.Get(i), i);
    }
    
    // Проверяем итератор на большом наборе
    auto iterator = seq.CreateIterator();
    int count = 0;
    while (iterator->HasNext()) {
        iterator->Next();
        count++;
    }
    EXPECT_EQ(count, SIZE);
    delete iterator;
}

TEST(SequenceTest, StringSequence) 
{
    // Проверяет: Sequence с строковыми типами
    MutableArraySequence<std::string> seq;
    seq.Append("Hello");
    seq.Append("World");
    seq.Prepend("Start");
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), "Start");
    EXPECT_EQ(seq.GetLast(), "World");
    EXPECT_EQ(seq.Get(1), "Hello");
}

// 4. Тесты BinaryTree

TEST(BinaryTreeTest, EmptyTreeOperations) 
{
    // Проверяет: Операции с пустым деревом
    BinaryTree<int> tree;
    EXPECT_TRUE(tree.isEmpty());
    EXPECT_FALSE(tree.contains(5));
    EXPECT_EQ(tree.getRoot(), nullptr);
}

TEST(BinaryTreeTest, InsertSingleElement) 
{
    // Проверяет: Вставку одного элемента в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_TRUE(tree.contains(5));
    EXPECT_FALSE(tree.contains(10));
    EXPECT_NE(tree.getRoot(), nullptr);
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, InsertMultipleElements) 
{
    // Проверяет: Вставку нескольких элементов в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(6));
    EXPECT_TRUE(tree.contains(8));
    EXPECT_FALSE(tree.contains(10));
}

TEST(BinaryTreeTest, InsertDuplicateElements) 
{
    // Проверяет: Вставку дублирующихся элементов в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(5);
    tree.insert(5);
    
    EXPECT_TRUE(tree.contains(5));
    
    int count = 0;
    tree.traverseInOrder([&](int value) 
    {
        if (value == 5) count++;
    });

    EXPECT_EQ(count, 1);
}

TEST(BinaryTreeTest, RemoveLeafNode) 
{
    // Проверяет: Удаление листового узла из дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(7));
    
    // Проверяем что дерево корректно перестроилось
    EXPECT_NE(tree.getRoot(), nullptr);
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, RemoveNodeWithOneLeftChild) 
{
    // Проверяет: Удаление узла с одним левым потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(2);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(5));
    
    EXPECT_EQ(tree.getRoot()->left->data, 2);
}

TEST(BinaryTreeTest, RemoveNodeWithOneRightChild) 
{
    // Проверяет: Удаление узла с одним правым потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(4);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(5));
}

TEST(BinaryTreeTest, RemoveNodeWithTwoChildren) 
{
    // Проверяет: Удаление узла с двумя потомками
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    tree.remove(5);
    
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(6));
    EXPECT_TRUE(tree.contains(8));
    
    EXPECT_NE(tree.getRoot(), nullptr);
}

TEST(BinaryTreeTest, RemoveRootWithOneChild) 
{
    // Проверяет: Удаление корня с одним потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    
    tree.remove(5);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_EQ(tree.getRoot()->data, 3);
}

TEST(BinaryTreeTest, RemoveNonExistentElement) 
{
    // Проверяет: Удаление несуществующего элемента из дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    EXPECT_NO_THROW(tree.remove(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
}

TEST(BinaryTreeTest, TraversePreOrder) 
{
    // Проверяет: Прямой обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    
    std::vector<int> result;
    tree.traversePreOrder([&](int value) {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 5);

    EXPECT_EQ(result[0], 5);
}

TEST(BinaryTreeTest, TraverseInOrderSorted) 
{
    // Проверяет: Симметричный обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    std::vector<int> result;
    tree.traverseInOrder([&](int value) {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 7);
    for (size_t i = 1; i < result.size(); i++) 
    {
        EXPECT_LE(result[i-1], result[i]);
    }
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[6], 8);
}

TEST(BinaryTreeTest, TraversePostOrder) 
{
    // Проверяет: Обратный обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    std::vector<int> result;
    tree.traversePostOrder([&](int value) 
    {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 3);

    EXPECT_EQ(result[2], 5);
}

TEST(BinaryTreeTest, TreeBalanceSimple) 
{
    // Проверяет: Балансировку дерева
    BinaryTree<int> tree;

    for (int i = 1; i <= 5; i++) 
    {
        tree.insert(i);
    }
    
    tree.balance();
    
    for (int i = 1; i <= 5; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
    
    std::vector<int> inOrder;
    tree.traverseInOrder([&](int value) 
    {
        inOrder.push_back(value);
    });
    EXPECT_EQ(inOrder.size(), 5);
    EXPECT_EQ(inOrder[0], 1);
    EXPECT_EQ(inOrder[4], 5);
}

TEST(BinaryTreeTest, TreeBalanceLarge) 
{
    // Проверяет: Балансировку большого дерева
    BinaryTree<int> tree;
    for (int i = 1; i <= 10; i++) 
    {
        tree.insert(i);
    }
    
    tree.balance();
    
    for (int i = 1; i <= 10; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
}

TEST(BinaryTreeTest, TreeBalanceEmpty) 
{
    // Проверяет: Балансировку пустого дерева
    BinaryTree<int> tree;

    EXPECT_NO_THROW(tree.balance());
    EXPECT_TRUE(tree.isEmpty());
}

TEST(BinaryTreeTest, TreeCopyConstructor) 
{
    // Проверяет: Конструктор копирования для дерева
    BinaryTree<int> tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);
    tree1.insert(1);
    tree1.insert(4);
    
    BinaryTree<int> tree2(tree1);
    
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(3));
    EXPECT_TRUE(tree2.contains(7));
    EXPECT_TRUE(tree2.contains(1));
    EXPECT_TRUE(tree2.contains(4));
    
    tree1.insert(10);
    EXPECT_FALSE(tree2.contains(10));
}

TEST(BinaryTreeTest, TreeAssignmentOperator) 
{
    // Проверяет: Оператор присваивания для дерева
    BinaryTree<int> tree1;
    tree1.insert(10);
    tree1.insert(5);
    tree1.insert(15);
    
    BinaryTree<int> tree2;
    tree2.insert(100);
    
    tree2 = tree1;
    
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(15));
    EXPECT_FALSE(tree2.contains(100));
    
    // Проверяем что это глубокое копирование
    tree1.insert(20);
    EXPECT_FALSE(tree2.contains(20));
}

TEST(BinaryTreeTest, TreeSelfAssignment) 
{
    // Проверяет: Само-присваивание для дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    tree = tree;
    
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, TreeClear) 
{
    // Проверяет: Очистку дерева
    BinaryTree<int> tree;
    for (int i = 0; i < 10; i++) 
    {
        tree.insert(i);
    }
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_NE(tree.getRoot(), nullptr);
    
    tree.clear();
    
    EXPECT_TRUE(tree.isEmpty());
    EXPECT_EQ(tree.getRoot(), nullptr);
    
    // Проверяем что после очистки можно снова добавлять элементы
    tree.insert(100);
    EXPECT_TRUE(tree.contains(100));
    EXPECT_FALSE(tree.isEmpty());
}

TEST(BinaryTreeTest, TreeMapOperation) 
{
    // Проверяет: Операцию Map для дерева
    BinaryTree<int> tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    
    auto newTree = tree.map([](int x) { return x * 2; });
    
    EXPECT_FALSE(newTree.isEmpty());
    EXPECT_TRUE(newTree.contains(2));
    EXPECT_TRUE(newTree.contains(4));
    EXPECT_TRUE(newTree.contains(6));
    EXPECT_FALSE(newTree.contains(1));
    
    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
}

TEST(BinaryTreeTest, TreeWhereOperation) 
{
    // Проверяет: Операцию Where для дерева
    BinaryTree<int> tree;
    for (int i = 1; i <= 10; i++) 
    {
        tree.insert(i);
    }
    
    auto evenTree = tree.where([](int x) { return x % 2 == 0; });
    
    EXPECT_FALSE(evenTree.isEmpty());
    EXPECT_TRUE(evenTree.contains(2));
    EXPECT_TRUE(evenTree.contains(4));
    EXPECT_TRUE(evenTree.contains(6));
    EXPECT_TRUE(evenTree.contains(8));
    EXPECT_TRUE(evenTree.contains(10));
    EXPECT_FALSE(evenTree.contains(1));
    EXPECT_FALSE(evenTree.contains(3));
    EXPECT_FALSE(evenTree.contains(5));
    

    for (int i = 1; i <= 10; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
}

TEST(BinaryTreeTest, TreeMergeOperation) 
{
    // Проверяет: Операцию Merge для дерева
    BinaryTree<int> tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    BinaryTree<int> tree2;
    tree2.insert(4);
    tree2.insert(5);
    tree2.insert(6);
    
    tree1.merge(tree2);
    
    EXPECT_TRUE(tree1.contains(1));
    EXPECT_TRUE(tree1.contains(2));
    EXPECT_TRUE(tree1.contains(3));
    EXPECT_TRUE(tree1.contains(4));
    EXPECT_TRUE(tree1.contains(5));
    EXPECT_TRUE(tree1.contains(6));
    
    EXPECT_TRUE(tree2.contains(4));
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(6));
}

TEST(BinaryTreeTest, TreeMergeWithOverlap) 
{
    // Проверяет: Объединение деревьев с перекрывающимися элементами
    BinaryTree<int> tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    BinaryTree<int> tree2;
    tree2.insert(2);
    tree2.insert(3);
    tree2.insert(4);
    
    tree1.merge(tree2);
    
    EXPECT_TRUE(tree1.contains(1));
    EXPECT_TRUE(tree1.contains(2));
    EXPECT_TRUE(tree1.contains(3));
    EXPECT_TRUE(tree1.contains(4));
}

TEST(BinaryTreeTest, ExtractSubtree) 
{
    // Проверяет: Извлечение поддерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    auto subtree = tree.extractSubtree(7);
    
    EXPECT_FALSE(subtree.isEmpty());
    EXPECT_TRUE(subtree.contains(7));
    EXPECT_TRUE(subtree.contains(6));
    EXPECT_TRUE(subtree.contains(8));
    EXPECT_FALSE(subtree.contains(5));
    EXPECT_FALSE(subtree.contains(3));
    
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(7));
}

TEST(BinaryTreeTest, ExtractSubtreeNonExistent) 
{
    // Проверяет: Извлечение несуществующего поддерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    
    auto subtree = tree.extractSubtree(10);
    
    EXPECT_TRUE(subtree.isEmpty());
}

TEST(BinaryTreeTest, FindNode) 
{
    // Проверяет: Поиск узла в дереве
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    
    auto node = tree.findNode(3);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, 3);
    
    node = tree.findNode(1);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, 1);
    
    node = tree.findNode(10);
    EXPECT_EQ(node, nullptr);
}

TEST(BinaryTreeTest, ContainsSubtree) 
{
    // Проверяет: Проверку содержания поддерева
    BinaryTree<int> tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);
    tree1.insert(2);
    tree1.insert(4);
    tree1.insert(6);
    tree1.insert(8);
    
    BinaryTree<int> subtree;
    subtree.insert(3);
    subtree.insert(2);
    subtree.insert(4);
    
    EXPECT_TRUE(tree1.containsSubtree(subtree));
    
    BinaryTree<int> notSubtree;
    notSubtree.insert(3);
    notSubtree.insert(2);
    notSubtree.insert(5);
    
    EXPECT_FALSE(tree1.containsSubtree(notSubtree));
}

TEST(BinaryTreeTest, TreeIteratorBasic) 
{
    // Проверяет: Базовые операции итератора дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    auto iterator = tree.CreateIterator();
    EXPECT_TRUE(iterator->HasNext());
    
    // Собираем все элементы
    std::vector<int> elements;
    while (iterator->HasNext()) 
    {
        elements.push_back(iterator->GetCurrentItem());
        iterator->Next();
    }
    
    EXPECT_EQ(elements.size(), 3);
    
    iterator->Reset();
    EXPECT_TRUE(iterator->HasNext());
    
    delete iterator;
}

TEST(BinaryTreeTest, TreeIteratorEmptyTree) 
{
    // Проверяет: Итератор для пустого дерева
    BinaryTree<int> tree;
    auto iterator = tree.CreateIterator();
    
    EXPECT_FALSE(iterator->HasNext());
    EXPECT_THROW(iterator->GetCurrentItem(), std::out_of_range);
    
    iterator->Reset();
    EXPECT_FALSE(iterator->HasNext());
    
    delete iterator;
}

TEST(BinaryTreeTest, TreeIteratorReset) 
{
    // Проверяет: Сброс итератора дерева
    BinaryTree<int> tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    
    auto iterator = tree.CreateIterator();
    
    while (iterator->HasNext()) 
    {
        iterator->Next();
    }
    
    EXPECT_FALSE(iterator->HasNext());
    
    iterator->Reset();
    EXPECT_TRUE(iterator->HasNext());
    EXPECT_EQ(iterator->GetCurrentItem(), 1);
    
    delete iterator;
}

TEST(BinaryTreeTest, TreeSerializeDeserialize) 
{
    // Проверяет: Сериализацию и десериализацию дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    std::string serialized = tree.serialize("KLP");
    
    BinaryTree<int> newTree;
    newTree.deserialize(serialized, "KLP");
    
    EXPECT_TRUE(newTree.contains(5));
    EXPECT_TRUE(newTree.contains(3));
    EXPECT_TRUE(newTree.contains(7));
    EXPECT_TRUE(newTree.contains(1));
    EXPECT_TRUE(newTree.contains(4));
    EXPECT_TRUE(newTree.contains(6));
    EXPECT_TRUE(newTree.contains(8));
}

TEST(BinaryTreeTest, TreeSerializeInvalidFormat) 
{
    // Проверяет: Обработку неверного формата сериализации
    BinaryTree<int> tree;
    tree.insert(5);
    
    EXPECT_THROW(tree.serialize("INVALID"), std::invalid_argument);
}

TEST(BinaryTreeTest, StringBinaryTreeOperations) 
{
    // Проверяет: Операции бинарного дерева со строковыми типами
    BinaryTree<std::string> tree;
    tree.insert("apple");
    tree.insert("banana");
    tree.insert("cherry");
    
    EXPECT_TRUE(tree.contains("banana"));
    EXPECT_FALSE(tree.contains("grape"));
    
    auto uppercaseTree = tree.map([](const std::string& s) 
    {
        std::string result = s;
        for (char& c : result) c = toupper(c);
        return result;
    });
    
    EXPECT_TRUE(uppercaseTree.contains("APPLE"));
    EXPECT_TRUE(uppercaseTree.contains("BANANA"));
    EXPECT_TRUE(uppercaseTree.contains("CHERRY"));
    EXPECT_FALSE(uppercaseTree.contains("apple"));
}

// 5. Тесты Dictionary с простыми типами

TEST(DictionaryTest, EmptyDictionaryOperations) 
{
    // Проверяет: Операции с пустым словарем
    Dictionary<int, std::string> dict;
    EXPECT_TRUE(dict.IsEmpty());
    EXPECT_EQ(dict.GetCount(), 0);
    EXPECT_FALSE(dict.ContainsKey(1));
    EXPECT_THROW(dict.Get(1), std::runtime_error);
}

TEST(DictionaryTest, BasicAddAndGet) 
{
    // Проверяет: Базовое добавление и получение элементов
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    
    EXPECT_EQ(dict.GetCount(), 2);
    EXPECT_TRUE(dict.ContainsKey(1));
    EXPECT_TRUE(dict.ContainsKey(2));
    EXPECT_EQ(dict.Get(1), "one");
    EXPECT_EQ(dict.Get(2), "two");
}

TEST(DictionaryTest, AddDuplicateKey) 
{
    // Проверяет: Добавление дублирующегося ключа
    Dictionary<int, std::string> dict;
    dict.Add(1, "old");
    dict.Add(1, "new");
    
    EXPECT_EQ(dict.GetCount(), 1);
    EXPECT_EQ(dict.Get(1), "new");
}

TEST(DictionaryTest, RemoveOperations) 
{
    // Проверяет: Удаление элементов из словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "a");
    dict.Add(2, "b");
    dict.Add(3, "c");
    
    dict.Remove(2);
    EXPECT_FALSE(dict.ContainsKey(2));
    EXPECT_EQ(dict.GetCount(), 2);
    EXPECT_TRUE(dict.ContainsKey(1));
    EXPECT_TRUE(dict.ContainsKey(3));
    
    dict.Remove(10);
    EXPECT_EQ(dict.GetCount(), 2);
}

TEST(DictionaryTest, ClearDictionary) 
{
    // Проверяет: Очистку словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "a");
    dict.Add(2, "b");
    
    EXPECT_EQ(dict.GetCount(), 2);
    
    dict.Clear();
    EXPECT_TRUE(dict.IsEmpty());
    EXPECT_EQ(dict.GetCount(), 0);
    EXPECT_FALSE(dict.ContainsKey(1));
}

TEST(DictionaryTest, GetKeysAndValues) 
{
    // Проверяет: Получение ключей и значений словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    dict.Add(3, "three");
    
    auto keys = dict.GetKeys();
    auto values = dict.GetValues();
    
    EXPECT_EQ(keys->GetLength(), 3);
    EXPECT_EQ(values->GetLength(), 3);
    
    // Проверяем что ключи корректны
    bool found1 = false, found2 = false, found3 = false;
    for (int i = 0; i < keys->GetLength(); i++) 
    {
        int key = keys->Get(i);
        if (key == 1) found1 = true;
        if (key == 2) found2 = true;
        if (key == 3) found3 = true;
    }
    EXPECT_TRUE(found1 && found2 && found3);
    
    delete keys;
    delete values;
}

TEST(DictionaryTest, ForEachOperations) 
{
    // Проверяет: Операции ForEach для словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    dict.Add(3, "three");
    
    int keySum = 0;
    dict.ForEachKey([&](int key) { keySum += key; });
    EXPECT_EQ(keySum, 6);
    
    int valueCount = 0;
    dict.ForEachValue([&](const std::string&) { valueCount++; });
    EXPECT_EQ(valueCount, 3);
    
    std::string allValues;
    dict.ForEach([&](int key, const std::string& value) 
    {
        allValues += value;
    });
    EXPECT_FALSE(allValues.empty());
}

TEST(DictionaryTest, ContainsValue) 
{
    // Проверяет: Проверку содержания значения в словаре
    Dictionary<int, std::string> dict;
    dict.Add(1, "apple");
    dict.Add(2, "banana");
    dict.Add(3, "apple");

    EXPECT_TRUE(dict.ContainsValue("apple"));
    EXPECT_TRUE(dict.ContainsValue("banana"));
    EXPECT_FALSE(dict.ContainsValue("cherry"));
}

TEST(DictionaryTest, BalanceDictionary) 
{
    // Проверяет: Балансировку словаря
    Dictionary<int, std::string> dict;
    for (int i = 0; i < 100; i++) 
    {
        dict.Add(i, "value" + std::to_string(i));
    }
    
    dict.Balance();
    EXPECT_EQ(dict.GetCount(), 100);
    
    for (int i = 0; i < 100; i++) 
    {
        EXPECT_TRUE(dict.ContainsKey(i));
        EXPECT_EQ(dict.Get(i), "value" + std::to_string(i));
    }
}

TEST(DictionaryTest, StringKeyDictionary) 
{
    // Проверяет: Словарь со строковыми ключами
    Dictionary<std::string, int> dict;
    dict.Add("apple", 5);
    dict.Add("banana", 3);
    dict.Add("cherry", 7);
    
    EXPECT_TRUE(dict.ContainsKey("banana"));
    EXPECT_EQ(dict.Get("apple"), 5);
    
    dict.Remove("banana");
    EXPECT_FALSE(dict.ContainsKey("banana"));
}

TEST(DictionaryTest, LargeDictionaryOperations) 
{
    // Проверяет: Работу с большим словарем
    Dictionary<int, int> dict;
    const int COUNT = 1000;
    
    for (int i = 0; i < COUNT; i++) 
    {
        dict.Add(i, i * i);
    }
    
    EXPECT_EQ(dict.GetCount(), COUNT);
    
    for (int i = 0; i < COUNT; i++) 
    {
        EXPECT_TRUE(dict.ContainsKey(i));
        EXPECT_EQ(dict.Get(i), i * i);
    }
    
    // Удаляем каждое второе значение
    for (int i = 0; i < COUNT; i += 2) 
    {
        dict.Remove(i);
    }
    
    EXPECT_EQ(dict.GetCount(), COUNT / 2);
    
    for (int i = 1; i < COUNT; i += 2)
    {
        EXPECT_TRUE(dict.ContainsKey(i));
    }
    
    for (int i = 0; i < COUNT; i += 2) 
    {
        EXPECT_FALSE(dict.ContainsKey(i));
    }
}

// 6. Тесты для графов

TEST(GraphTest, DirectedGraphBasicOperations) 
{
    // Проверяет: Базовые операции ориентированного графа
    DirectedGraph<int> graph;
    
    graph.AddVertex(1);
    graph.AddVertex(2);
    graph.AddVertex(3);
    
    EXPECT_TRUE(graph.HasVertex(1));
    EXPECT_TRUE(graph.HasVertex(2));
    EXPECT_TRUE(graph.HasVertex(3));
    EXPECT_FALSE(graph.HasVertex(4));
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 3);
    
    EXPECT_TRUE(graph.HasEdge(1, 2));
    EXPECT_TRUE(graph.HasEdge(1, 3));
    EXPECT_TRUE(graph.HasEdge(2, 3));
    EXPECT_FALSE(graph.HasEdge(2, 1));
    EXPECT_FALSE(graph.HasEdge(3, 1));
    
    EXPECT_EQ(graph.GetVertexCount(), 3);
    EXPECT_EQ(graph.GetEdgeCount(), 3);
    
    auto adj1 = graph.GetAdjacentVertices(1);
    EXPECT_EQ(adj1->GetLength(), 2);
    delete adj1;
    
    EXPECT_TRUE(graph.IsDirected());
}

TEST(GraphTest, DirectedGraphEdgeWeights) 
{
    // Проверяет: Веса ребер в ориентированном графе
    DirectedGraph<std::string> graph;
    
    graph.AddVertex("A");
    graph.AddVertex("B");
    graph.AddVertex("C");
    
    graph.AddEdge("A", "B", 5.0);
    graph.AddEdge("B", "C", 3.0);
    graph.AddEdge("A", "C", 10.0);
    
    EXPECT_EQ(graph.GetEdgeWeight("A", "B"), 5.0);
    EXPECT_EQ(graph.GetEdgeWeight("B", "C"), 3.0);
    EXPECT_EQ(graph.GetEdgeWeight("A", "C"), 10.0);
    
    graph.SetEdgeWeight("A", "B", 7.0);
    EXPECT_EQ(graph.GetEdgeWeight("A", "B"), 7.0);
    
    EXPECT_THROW(graph.GetEdgeWeight("C", "A"), std::runtime_error);
}

TEST(GraphTest, DirectedGraphRemoveOperations) 
{
    // Проверяет: Удаление вершин и ребер в ориентированном графе
    DirectedGraph<int> graph;
    
    graph.AddVertex(1);
    graph.AddVertex(2);
    graph.AddVertex(3);
    graph.AddVertex(4);
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 3);
    graph.AddEdge(3, 4);
    
    EXPECT_EQ(graph.GetEdgeCount(), 4);
    
    graph.RemoveEdge(1, 3);
    EXPECT_FALSE(graph.HasEdge(1, 3));
    EXPECT_EQ(graph.GetEdgeCount(), 3);
    
    graph.RemoveVertex(3);
    EXPECT_FALSE(graph.HasVertex(3));
    EXPECT_FALSE(graph.HasEdge(2, 3));
    EXPECT_FALSE(graph.HasEdge(3, 4));
    EXPECT_EQ(graph.GetEdgeCount(), 1);
    EXPECT_EQ(graph.GetVertexCount(), 3);
}

TEST(GraphTest, UndirectedGraphBasicOperations) 
{
    // Проверяет: Базовые операции неориентированного графа
    UndirectedGraph<int> graph;
    
    graph.AddVertex(1);
    graph.AddVertex(2);
    graph.AddVertex(3);
    
    graph.AddEdge(1, 2);
    graph.AddEdge(2, 3);
    
    EXPECT_TRUE(graph.HasEdge(1, 2));
    EXPECT_TRUE(graph.HasEdge(2, 1));
    EXPECT_TRUE(graph.HasEdge(2, 3));
    EXPECT_TRUE(graph.HasEdge(3, 2));
    EXPECT_FALSE(graph.HasEdge(1, 3));
    
    EXPECT_EQ(graph.GetVertexCount(), 3);
    EXPECT_EQ(graph.GetEdgeCount(), 2);
    
    EXPECT_FALSE(graph.IsDirected());
}

TEST(GraphTest, UndirectedGraphEdgeWeights) 
{
    // Проверяет: Веса ребер в неориентированном графе
    UndirectedGraph<std::string> graph;
    
    graph.AddVertex("A");
    graph.AddVertex("B");
    graph.AddVertex("C");
    
    graph.AddEdge("A", "B", 5.0);
    graph.AddEdge("B", "C", 3.0);
    
    EXPECT_EQ(graph.GetEdgeWeight("A", "B"), 5.0);
    EXPECT_EQ(graph.GetEdgeWeight("B", "A"), 5.0);
    EXPECT_EQ(graph.GetEdgeWeight("B", "C"), 3.0);
    EXPECT_EQ(graph.GetEdgeWeight("C", "B"), 3.0);
    
    graph.SetEdgeWeight("A", "B", 7.0);
    EXPECT_EQ(graph.GetEdgeWeight("B", "A"), 7.0);
}

TEST(GraphTest, UndirectedGraphRemoveOperations) 
{
    // Проверяет: Удаление вершин и ребер в неориентированном графе
    UndirectedGraph<int> graph;
    
    graph.AddVertex(1);
    graph.AddVertex(2);
    graph.AddVertex(3);
    graph.AddVertex(4);
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 3);
    graph.AddEdge(3, 4);
    
    EXPECT_EQ(graph.GetEdgeCount(), 4);
    
    graph.RemoveEdge(1, 3);
    EXPECT_FALSE(graph.HasEdge(1, 3));
    EXPECT_FALSE(graph.HasEdge(3, 1));
    EXPECT_EQ(graph.GetEdgeCount(), 3);
    
    graph.RemoveVertex(3);
    EXPECT_FALSE(graph.HasVertex(3));
    EXPECT_FALSE(graph.HasEdge(2, 3));
    EXPECT_FALSE(graph.HasEdge(3, 4));
    EXPECT_EQ(graph.GetEdgeCount(), 1);
    EXPECT_EQ(graph.GetVertexCount(), 3);
}

TEST(GraphTest, GraphSerializationDirected) 
{
    // Проверяет: Сериализацию и десериализацию ориентированного графа
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2, 1.0);
    graph.AddEdge(1, 3, 2.0);
    graph.AddEdge(2, 3, 3.0);
    
    std::stringstream ss;
    graph.SerializeToStream(ss);
    
    auto deserialized = DirectedGraph<int>::DeserializeFromStream(ss);
    
    EXPECT_EQ(deserialized->GetVertexCount(), 3);
    EXPECT_EQ(deserialized->GetEdgeCount(), 3);
    EXPECT_TRUE(deserialized->HasEdge(1, 2));
    EXPECT_TRUE(deserialized->HasEdge(1, 3));
    EXPECT_TRUE(deserialized->HasEdge(2, 3));
    EXPECT_EQ(deserialized->GetEdgeWeight(2, 3), 3.0);
    
    delete deserialized;
}

TEST(GraphTest, GraphSerializationUndirected) 
{
    // Проверяет: Сериализацию и десериализацию неориентированного графа
    UndirectedGraph<int> graph;
    
    graph.AddEdge(1, 2, 1.0);
    graph.AddEdge(1, 3, 2.0);
    graph.AddEdge(2, 3, 3.0);
    
    std::stringstream ss;
    graph.SerializeToStream(ss);
    
    auto deserialized = UndirectedGraph<int>::DeserializeFromStream(ss);
    
    EXPECT_EQ(deserialized->GetVertexCount(), 3);
    EXPECT_EQ(deserialized->GetEdgeCount(), 3);
    EXPECT_TRUE(deserialized->HasEdge(1, 2));
    EXPECT_TRUE(deserialized->HasEdge(1, 3));
    EXPECT_TRUE(deserialized->HasEdge(2, 3));
    EXPECT_EQ(deserialized->GetEdgeWeight(2, 3), 3.0);
    
    delete deserialized;
}

// 7. Тесты для ConnectedComponents

TEST(ConnectedComponentsTest, EmptyGraph) 
{
    // Проверяет: Компоненты связности пустого графа
    UndirectedGraph<int> graph;
    
    EXPECT_TRUE(ConnectedComponents<int>::IsConnected(graph));
    EXPECT_EQ(ConnectedComponents<int>::CountComponents(graph), 0);
}

TEST(ConnectedComponentsTest, SingleVertexGraph) 
{
    // Проверяет: Компоненты связности графа с одной вершиной
    UndirectedGraph<int> graph;
    graph.AddVertex(1);
    
    EXPECT_TRUE(ConnectedComponents<int>::IsConnected(graph));
    EXPECT_EQ(ConnectedComponents<int>::CountComponents(graph), 1);
    
    auto component = ConnectedComponents<int>::GetComponentForVertex(graph, 1);
    EXPECT_EQ(component->GetLength(), 1);
    EXPECT_EQ(component->Get(0), 1);
    delete component;
}

TEST(ConnectedComponentsTest, TwoConnectedVertices) 
{
    // Проверяет: Компоненты связности двух связанных вершин
    UndirectedGraph<int> graph;
    graph.AddEdge(1, 2);
    
    EXPECT_TRUE(ConnectedComponents<int>::IsConnected(graph));
    EXPECT_EQ(ConnectedComponents<int>::CountComponents(graph), 1);
    EXPECT_TRUE(ConnectedComponents<int>::AreConnected(graph, 1, 2));
    
    auto componentsBFS = ConnectedComponents<int>::FindComponentsBFS(graph);
    EXPECT_EQ(componentsBFS->GetLength(), 1);
    EXPECT_EQ(componentsBFS->Get(0)->GetLength(), 2);
    delete componentsBFS->Get(0);
    delete componentsBFS;
}

TEST(ConnectedComponentsTest, TwoDisconnectedComponents) 
{
    // Проверяет: Две несвязанные компоненты связности
    UndirectedGraph<int> graph;
    graph.AddEdge(1, 2);
    graph.AddEdge(3, 4);
    graph.AddEdge(4, 5);
    
    EXPECT_FALSE(ConnectedComponents<int>::IsConnected(graph));
    EXPECT_EQ(ConnectedComponents<int>::CountComponents(graph), 2);
    EXPECT_TRUE(ConnectedComponents<int>::AreConnected(graph, 3, 5));
    EXPECT_FALSE(ConnectedComponents<int>::AreConnected(graph, 1, 3));
    
    auto componentsDFS = ConnectedComponents<int>::FindComponentsDFS(graph);
    EXPECT_EQ(componentsDFS->GetLength(), 2);
    
    int component1Size = componentsDFS->Get(0)->GetLength();
    int component2Size = componentsDFS->Get(1)->GetLength();
    EXPECT_TRUE((component1Size == 2 && component2Size == 3) || 
                (component1Size == 3 && component2Size == 2));
    
    delete componentsDFS->Get(0);
    delete componentsDFS->Get(1);
    delete componentsDFS;
}

TEST(ConnectedComponentsTest, FindLargestAndSmallestComponents) 
{
    // Проверяет: Поиск наибольшей и наименьшей компонент связности
    UndirectedGraph<int> graph;
    graph.AddEdge(1, 2);
    graph.AddEdge(2, 3);
    graph.AddEdge(4, 5);
    graph.AddVertex(6);
    
    auto largest = ConnectedComponents<int>::FindLargestComponent(graph);
    auto smallest = ConnectedComponents<int>::FindSmallestComponent(graph);
    
    EXPECT_GE(largest->GetLength(), 2);
    EXPECT_LE(smallest->GetLength(), 1);
    
    delete largest;
    delete smallest;
}

// 8. Тесты для TopologicalSort

TEST(TopologicalSortTest, EmptyGraph) 
{
    // Проверяет: Топологическую сортировку пустого графа
    DirectedGraph<int> graph;
    
    auto resultKahn = TopologicalSort<int>::KahnSort(graph);
    EXPECT_EQ(resultKahn->GetLength(), 0);
    delete resultKahn;
    
    auto resultDFS = TopologicalSort<int>::DFSSort(graph);
    EXPECT_EQ(resultDFS->GetLength(), 0);
    delete resultDFS;
}

TEST(TopologicalSortTest, SimpleDAG) 
{
    // Проверяет: Топологическую сортировку простого ациклического графа
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 4);
    
    EXPECT_TRUE(TopologicalSort<int>::IsAcyclic(graph));
    
    auto sources = TopologicalSort<int>::FindSources(graph);
    EXPECT_EQ(sources->GetLength(), 1);
    EXPECT_EQ(sources->Get(0), 1);
    delete sources;
    
    auto sinks = TopologicalSort<int>::FindSinks(graph);
    EXPECT_EQ(sinks->GetLength(), 1);
    EXPECT_EQ(sinks->Get(0), 4);
    delete sinks;
    
    auto sortedKahn = TopologicalSort<int>::KahnSort(graph);
    EXPECT_EQ(sortedKahn->GetLength(), 4);
    
    std::unordered_set<int> visited;
    for (int i = 0; i < sortedKahn->GetLength(); i++) {
        int vertex = sortedKahn->Get(i);
        visited.insert(vertex);
        
        auto adj = graph.GetAdjacentVertices(vertex);
        for (int j = 0; j < adj->GetLength(); j++) {
            EXPECT_TRUE(visited.find(adj->Get(j)) == visited.end());
        }
        delete adj;
    }
    delete sortedKahn;
}

TEST(TopologicalSortTest, GraphWithCycle) 
{
    // Проверяет: Обработку графа с циклом при топологической сортировке
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(2, 3);
    graph.AddEdge(3, 1);
    
    EXPECT_FALSE(TopologicalSort<int>::IsAcyclic(graph));
    
    EXPECT_THROW(TopologicalSort<int>::KahnSort(graph), std::runtime_error);
    EXPECT_THROW(TopologicalSort<int>::DFSSort(graph), std::runtime_error);
}

TEST(TopologicalSortTest, ComplexDAG) 
{
    // Проверяет: Топологическую сортировку сложного ациклического графа
    DirectedGraph<std::string> graph;
    
    graph.AddEdge("A", "B");
    graph.AddEdge("A", "C");
    graph.AddEdge("B", "D");
    graph.AddEdge("C", "D");
    graph.AddEdge("D", "E");
    graph.AddEdge("D", "F");
    
    auto sorted = TopologicalSort<std::string>::KahnSort(graph);
    EXPECT_EQ(sorted->GetLength(), 6);
    
    bool foundA = false, foundE = false;
    for (int i = 0; i < sorted->GetLength(); i++) {
        if (sorted->Get(i) == "A") foundA = true;
        if (sorted->Get(i) == "E") foundE = true;
    }
    EXPECT_TRUE(foundA);
    EXPECT_TRUE(foundE);
    
    delete sorted;
}

// 9. Тесты для ShortestPath

TEST(ShortestPathTest, SingleVertexGraph) 
{
    // Проверяет: Кратчайшие пути в графе с одной вершиной
    DirectedGraph<int> graph;
    graph.AddVertex(1);
    
    auto distances = ShortestPath<int>::Dijkstra(graph, 1);
    EXPECT_EQ(distances->GetCount(), 1);
    EXPECT_EQ(distances->Get(1), 0.0);
    
    auto path = ShortestPath<int>::FindShortestPath(graph, 1, 1);
    EXPECT_EQ(path->GetLength(), 1);
    EXPECT_EQ(path->Get(0), 1);
    
    delete distances;
    delete path;
}

TEST(ShortestPathTest, SimpleWeightedGraph) 
{
    // Проверяет: Кратчайшие пути во взвешенном графе
    DirectedGraph<std::string> graph;
    
    graph.AddEdge("A", "B", 1.0);
    graph.AddEdge("A", "C", 4.0);
    graph.AddEdge("B", "C", 2.0);
    graph.AddEdge("B", "D", 5.0);
    graph.AddEdge("C", "D", 1.0);
    
    auto distances = ShortestPath<std::string>::Dijkstra(graph, "A");
    
    EXPECT_EQ(distances->Get("A"), 0.0);
    EXPECT_EQ(distances->Get("B"), 1.0);
    EXPECT_EQ(distances->Get("C"), 3.0);
    EXPECT_EQ(distances->Get("D"), 4.0);
    
    auto path = ShortestPath<std::string>::FindShortestPath(graph, "A", "D");
    EXPECT_EQ(path->GetLength(), 4);
    EXPECT_EQ(path->Get(0), "A");
    EXPECT_EQ(path->Get(1), "B");
    EXPECT_EQ(path->Get(2), "C");
    EXPECT_EQ(path->Get(3), "D");
    
    delete distances;
    delete path;
}

TEST(ShortestPathTest, BFSUnweightedGraph) 
{
    // Проверяет: Кратчайшие пути в невзвешенном графе с помощью BFS
    UndirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 4);
    graph.AddEdge(4, 5);
    
    auto distances = ShortestPath<int>::BFSShortestPath(graph, 1);
    
    EXPECT_EQ(distances->Get(1), 0.0);
    EXPECT_EQ(distances->Get(2), 1.0);
    EXPECT_EQ(distances->Get(3), 1.0);
    EXPECT_EQ(distances->Get(4), 2.0);
    EXPECT_EQ(distances->Get(5), 3.0);
    
    delete distances;
}

TEST(ShortestPathTest, BellmanFordNegativeWeights) 
{
    // Проверяет: Алгоритм Беллмана-Форда для графов с отрицательными весами
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2, 6.0);
    graph.AddEdge(1, 3, 5.0);
    graph.AddEdge(1, 4, 5.0);
    graph.AddEdge(2, 5, -1.0);
    graph.AddEdge(3, 2, -2.0);
    graph.AddEdge(3, 5, 1.0);
    graph.AddEdge(4, 3, -2.0);
    graph.AddEdge(4, 6, -1.0);
    graph.AddEdge(5, 7, 3.0);
    graph.AddEdge(6, 7, 3.0);
    
    auto distances = ShortestPath<int>::BellmanFord(graph, 1);
    
    EXPECT_EQ(distances->Get(1), 0.0);
    EXPECT_EQ(distances->Get(2), 1.0);
    EXPECT_EQ(distances->Get(3), 3.0);
    EXPECT_EQ(distances->Get(4), 5.0);
    EXPECT_EQ(distances->Get(5), 0.0);
    EXPECT_EQ(distances->Get(6), 4.0);
    EXPECT_EQ(distances->Get(7), 3.0);
    
    delete distances;
}

TEST(ShortestPathTest, GraphDiameterAndRadius) 
{
    // Проверяет: Диаметр и радиус графа
    UndirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(2, 3);
    graph.AddEdge(3, 4);
    graph.AddEdge(4, 5);
    
    double diameter = ShortestPath<int>::GetGraphDiameter(graph);
    double radius = ShortestPath<int>::GetGraphRadius(graph);
    
    EXPECT_GT(diameter, 0.0);
    EXPECT_GT(radius, 0.0);
    EXPECT_GE(diameter, radius);
    
    auto center = ShortestPath<int>::FindGraphCenter(graph);
    EXPECT_GT(center->GetLength(), 0);
    delete center;
}

// 10. Тесты для PartialOrder

TEST(PartialOrderTest, SimplePartialOrder) 
{
    // Проверяет: Базовые операции частичного порядка
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 4);
    
    EXPECT_TRUE(PartialOrder<int>::IsPartialOrder(graph));
    
    auto minimal = PartialOrder<int>::FindMinimalElements(graph);
    auto maximal = PartialOrder<int>::FindMaximalElements(graph);
    
    EXPECT_EQ(minimal->GetLength(), 1);
    EXPECT_EQ(minimal->Get(0), 1);
    
    EXPECT_EQ(maximal->GetLength(), 1);
    EXPECT_EQ(maximal->Get(0), 4);
    
    EXPECT_TRUE(PartialOrder<int>::IsMinimalElement(graph, 1));
    EXPECT_FALSE(PartialOrder<int>::IsMinimalElement(graph, 2));
    EXPECT_TRUE(PartialOrder<int>::IsMaximalElement(graph, 4));
    EXPECT_FALSE(PartialOrder<int>::IsMaximalElement(graph, 3));
    
    delete minimal;
    delete maximal;
}

TEST(PartialOrderTest, HasseDiagram) 
{
    // Проверяет: Построение диаграммы Хассе
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 4);
    graph.AddEdge(1, 4);
    
    auto hasse = PartialOrder<int>::BuildHasseDiagram(graph);
    
    EXPECT_TRUE(hasse->HasEdge(1, 2));
    EXPECT_TRUE(hasse->HasEdge(1, 3));
    EXPECT_TRUE(hasse->HasEdge(2, 4));
    EXPECT_TRUE(hasse->HasEdge(3, 4));
    EXPECT_FALSE(hasse->HasEdge(1, 4));
    
    delete hasse;
}

TEST(PartialOrderTest, InfimumAndSupremum) 
{
    // Проверяет: Поиск инфимума и супремума
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 3);
    graph.AddEdge(1, 4);
    graph.AddEdge(2, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 5);
    graph.AddEdge(4, 5);
    
    EXPECT_TRUE(PartialOrder<int>::IsPartialOrder(graph));
    
    auto inf = PartialOrder<int>::FindInfimum(graph, 3, 4);
    auto sup = PartialOrder<int>::FindSupremum(graph, 1, 2);
    
    EXPECT_GT(inf->GetLength(), 0);
    EXPECT_GT(sup->GetLength(), 0);
    
    delete inf;
    delete sup;
}

TEST(PartialOrderTest, LatticeCheck) 
{
    // Проверяет: Проверку является ли порядок решеткой
    DirectedGraph<int> lattice;
    
    lattice.AddEdge(1, 2);
    lattice.AddEdge(1, 3);
    lattice.AddEdge(2, 4);
    lattice.AddEdge(3, 4);
    
    EXPECT_TRUE(PartialOrder<int>::IsLattice(lattice));
    
    DirectedGraph<int> nonLattice;
    
    nonLattice.AddEdge(1, 2);
    nonLattice.AddEdge(1, 3);
    nonLattice.AddEdge(2, 4);
    nonLattice.AddEdge(3, 5);
    
    EXPECT_FALSE(PartialOrder<int>::IsLattice(nonLattice));
}

TEST(PartialOrderTest, PartialOrderLevels) 
{
    // Проверяет: Определение уровней частичного порядка
    DirectedGraph<int> graph;
    
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(2, 4);
    graph.AddEdge(3, 4);
    graph.AddEdge(4, 5);
    
    auto levels = PartialOrder<int>::GetLevels(graph);
    
    EXPECT_GT(levels->GetLength(), 0);
    
    for (int i = 0; i < levels->GetLength(); i++) {
        auto level = levels->Get(i);
        EXPECT_GT(level->GetLength(), 0);
    }
    
    for (int i = 0; i < levels->GetLength(); i++) {
        delete levels->Get(i);
    }
    delete levels;
}

// 11. Интеграционные тесты

TEST(IntegrationTest, BinaryTreeWithSet) 
{
    // Проверяет: Интеграцию бинарного дерева и множества
    BinaryTree<int> tree;
    Set<int> set;
    
    for (int i = 1; i <= 5; i++) 
    {
        tree.insert(i);
        set.Add(i);
    }
    
    for (int i = 1; i <= 5; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
        EXPECT_TRUE(set.Contains(i));
    }
    
    Set<int> set2;
    set2.Add(3);
    set2.Add(4);
    set2.Add(5);
    set2.Add(6);
    
    Set<int> unionSet = set.Union(set2);
    EXPECT_GT(unionSet.GetCount(), 0);
    
    Set<int> intersectionSet = set.Intersection(set2);
    EXPECT_TRUE(intersectionSet.Contains(3));
    EXPECT_TRUE(intersectionSet.Contains(4));
    EXPECT_TRUE(intersectionSet.Contains(5));
    EXPECT_FALSE(intersectionSet.Contains(1));
    EXPECT_FALSE(intersectionSet.Contains(6));
}

TEST(IntegrationTest, DictionaryAndBinaryTreeIntegration) 
{
    // Проверяет: Интеграцию словаря и бинарного дерева
    Dictionary<int, std::string> dict;
    
    for (int i = 0; i < 10; i++) 
    {
        dict.Add(i, "Value" + std::to_string(i));
    }
    
    EXPECT_EQ(dict.GetCount(), 10);
    
    auto keys = dict.GetKeys();
    auto values = dict.GetValues();
    
    EXPECT_EQ(keys->GetLength(), 10);
    EXPECT_EQ(values->GetLength(), 10);
    
    EXPECT_TRUE(dict.ContainsKey(5));
    EXPECT_EQ(dict.Get(5), "Value5");
    
    dict.Remove(5);
    EXPECT_FALSE(dict.ContainsKey(5));
    EXPECT_EQ(dict.GetCount(), 9);
    
    dict.Balance();
    
    delete keys;
    delete values;
}

TEST(IntegrationTest, GraphAnalysisWorkflow) 
{
    // Проверяет: Полный рабочий процесс анализа графа
    UndirectedGraph<std::string> socialNetwork;
    
    socialNetwork.AddEdge("Alice", "Bob");
    socialNetwork.AddEdge("Alice", "Charlie");
    socialNetwork.AddEdge("Bob", "David");
    socialNetwork.AddEdge("Charlie", "David");
    socialNetwork.AddEdge("David", "Eve");
    socialNetwork.AddEdge("Eve", "Frank");
    
    socialNetwork.AddEdge("Grace", "Helen");
    socialNetwork.AddEdge("Helen", "Ivan");
    
    EXPECT_FALSE(ConnectedComponents<std::string>::IsConnected(socialNetwork));
    
    int componentCount = ConnectedComponents<std::string>::CountComponents(socialNetwork);
    EXPECT_EQ(componentCount, 2);
    
    EXPECT_TRUE(ConnectedComponents<std::string>::AreConnected(socialNetwork, "Alice", "Frank"));
    EXPECT_FALSE(ConnectedComponents<std::string>::AreConnected(socialNetwork, "Alice", "Grace"));
    
    auto largestComponent = ConnectedComponents<std::string>::FindLargestComponent(socialNetwork);
    EXPECT_GT(largestComponent->GetLength(), 0);
    
    auto bfsDistances = ShortestPath<std::string>::BFSShortestPath(socialNetwork, "Alice");
    EXPECT_EQ(bfsDistances->Get("Alice"), 0.0);
    EXPECT_EQ(bfsDistances->Get("Frank"), 4.0);
    
    delete largestComponent;
    delete bfsDistances;
}

TEST(IntegrationTest, TaskSchedulingWorkflow) 
{
    // Проверяет: Рабочий процесс планирования задач
    DirectedGraph<std::string> taskDependencies;
    
    taskDependencies.AddEdge("Design", "Implementation");
    taskDependencies.AddEdge("Requirements", "Design");
    taskDependencies.AddEdge("Implementation", "Testing");
    taskDependencies.AddEdge("Testing", "Deployment");
    taskDependencies.AddEdge("Implementation", "Documentation");
    
    EXPECT_TRUE(TopologicalSort<std::string>::IsAcyclic(taskDependencies));
    
    auto schedule = TopologicalSort<std::string>::KahnSort(taskDependencies);
    EXPECT_EQ(schedule->GetLength(), 6);
    
    std::unordered_map<std::string, int> position;
    for (int i = 0; i < schedule->GetLength(); i++) {
        position[schedule->Get(i)] = i;
    }
    
    EXPECT_LT(position["Requirements"], position["Design"]);
    EXPECT_LT(position["Design"], position["Implementation"]);
    EXPECT_LT(position["Implementation"], position["Testing"]);
    EXPECT_LT(position["Testing"], position["Deployment"]);
    EXPECT_LT(position["Implementation"], position["Documentation"]);
    
    auto sources = TopologicalSort<std::string>::FindSources(taskDependencies);
    EXPECT_EQ(sources->GetLength(), 1);
    EXPECT_EQ(sources->Get(0), "Requirements");
    
    auto sinks = TopologicalSort<std::string>::FindSinks(taskDependencies);
    EXPECT_EQ(sinks->GetLength(), 2);
    
    delete schedule;
    delete sources;
    delete sinks;
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}