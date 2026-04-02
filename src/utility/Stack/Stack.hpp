#pragma once

#include <vector>
#include <optional>

template <typename T>
class Stack {
private:
  std::vector<T> items;

public:
    Stack();
    Stack(const std::vector<T>& items);

    void push(const T& item);
    std::optional<T> pop();
    
    std::optional<T> peek() const;
    bool isEmpty() const;
    size_t size() const;
};

#include "Stack.tpp"