#ifndef X1_VIRTUAL_STACK_H
#define X1_VIRTUAL_STACK_H

#include <vector>

//
// Tag to indicate whether the cell contains the address of an integer variable
// or array, the address of a real variable or array, an integer value, or a real value.
//
enum class Cell_Type {
    INTEGER_ADDRESS,
    REAL_ADDRESS,
    INTEGER_VALUE,
    REAL_VALUE,
};

//
// Stack cells of the hypothetic stack machine.
//
struct Stack_Cell {
    Cell_Type type;
    uint64_t value;

    bool is_int_value() const { return type == Cell_Type::INTEGER_VALUE; }
    bool is_int_addr() const { return type == Cell_Type::INTEGER_ADDRESS; }
    bool is_real_value() const { return type == Cell_Type::REAL_VALUE; }
    bool is_real_addr() const { return type == Cell_Type::REAL_ADDRESS; }

    unsigned get_addr() const { return value & BITS(15); }
    Word get_int() const { return value & BITS(27); }
    Real get_real() const { return value & BITS(54); }
};

class Virtual_Stack {
private:
    std::vector<Stack_Cell> storage;

public:
    unsigned count() const { return storage.size(); }

    const Stack_Cell &back() { return storage.back(); }

    void erase()
    {
        // Remove all items but don't deallocate.
        storage.erase(storage.begin(), storage.end());
    }

    Stack_Cell pop()
    {
        if (storage.size() == 0) {
            throw std::runtime_error("Cannot pop empty stack");
        }
        auto item = storage.back();
        storage.pop_back();
        return item;
    }

    long double pop_ieee()
    {
        if (storage.size() == 0) {
            throw std::runtime_error("Cannot pop empty stack");
        }
        auto item = storage.back();
        storage.pop_back();
        if (item.is_int_value()) {
            return x1_to_integer(item.get_int());
        } else if (item.is_real_value()) {
            return x1_to_ieee(item.get_real());
        } else {
            throw std::runtime_error("Cannot convert address to real");
        }
    }

    bool pop_boolean()
    {
        if (storage.size() == 0) {
            throw std::runtime_error("Cannot pop empty stack");
        }
        auto item = storage.back();
        storage.pop_back();
        if (item.is_int_value()) {
            return item.get_int() == X1_TRUE;
        } else if (item.is_real_value()) {
            return item.get_real() == X1_TRUE;
        } else {
            throw std::runtime_error("Cannot convert address to boolean");
        }
    }

    Stack_Cell get(unsigned index) const
    {
        if (index >= storage.size()) {
            throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
        }
        return storage[index];
    }

    void push_int_value(Word value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::INTEGER_VALUE;
        item.value = value;
    }

    void push_int_addr(Word addr)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::INTEGER_ADDRESS;
        item.value = addr;
    }

    void push_real_value(Real value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_VALUE;
        item.value = value;
    }

    void push_real_addr(Word addr)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_ADDRESS;
        item.value = addr;
    }

    void push_ieee(long double value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_VALUE;
        item.value = ieee_to_x1(value);
    }
};

#endif // X1_VIRTUAL_STACK_H
