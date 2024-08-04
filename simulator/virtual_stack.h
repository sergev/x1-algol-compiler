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

    // Quick check of the type of the value.
    bool is_int_value() const { return type == Cell_Type::INTEGER_VALUE; }
    bool is_int_addr() const { return type == Cell_Type::INTEGER_ADDRESS; }
    bool is_real_value() const { return type == Cell_Type::REAL_VALUE; }
    bool is_real_addr() const { return type == Cell_Type::REAL_ADDRESS; }

    // Get value for a given type.
    unsigned get_addr() const { return value & BITS(15); }
    Word get_int() const { return value & BITS(27); }
    Real get_real() const { return value & BITS(54); }

    // Compare this item and another one.
    bool is_less(const Stack_Cell &another)
    {
        switch (type) {
        case Cell_Type::INTEGER_VALUE: {
            auto this_int = x1_to_integer(get_int());
            switch (another.type) {
            case Cell_Type::INTEGER_VALUE:
                return this_int < x1_to_integer(another.get_int());
            case Cell_Type::REAL_VALUE:
                return this_int < x1_to_ieee(another.get_real());
            default:
                throw std::runtime_error("Cannot compare integer with address");
            }
        }
        case Cell_Type::REAL_VALUE: {
            auto this_real = x1_to_ieee(get_real());
            switch (another.type) {
            case Cell_Type::INTEGER_VALUE:
                return this_real < x1_to_integer(another.get_int());
            case Cell_Type::REAL_VALUE:
                return this_real < x1_to_ieee(another.get_real());
            default:
                throw std::runtime_error("Cannot compare real with address");
            }
        }
        default:
            throw std::runtime_error("Cannot compare address");
        }
    }

    bool is_equal(const Stack_Cell &another)
    {
        switch (type) {
        case Cell_Type::INTEGER_VALUE: {
            auto this_int = x1_to_integer(get_int());
            switch (another.type) {
            case Cell_Type::INTEGER_VALUE:
                return this_int == x1_to_integer(another.get_int());
            case Cell_Type::REAL_VALUE:
                return this_int == x1_to_ieee(another.get_real());
            case Cell_Type::INTEGER_ADDRESS:
            case Cell_Type::REAL_ADDRESS:
                return this_int == another.get_addr();
            default:
                throw std::runtime_error("Bad cell type");
            }
        }
        case Cell_Type::REAL_VALUE: {
            auto this_real = x1_to_ieee(get_real());
            switch (another.type) {
            case Cell_Type::INTEGER_VALUE:
                return this_real == x1_to_integer(another.get_int());
            case Cell_Type::REAL_VALUE:
                return this_real == x1_to_ieee(another.get_real());
            default:
                throw std::runtime_error("Cannot compare real with address");
            }
        }
        case Cell_Type::INTEGER_ADDRESS:
        case Cell_Type::REAL_ADDRESS: {
            auto this_addr = get_addr();
            switch (another.type) {
            case Cell_Type::INTEGER_VALUE:
                return this_addr == x1_to_integer(another.get_int());
            case Cell_Type::REAL_VALUE:
                throw std::runtime_error("Cannot compare address with real");
            case Cell_Type::INTEGER_ADDRESS:
            case Cell_Type::REAL_ADDRESS:
                return this_addr == another.get_addr();
            default:
                throw std::runtime_error("Bad cell type");
            }
        }
        default:
            throw std::runtime_error("Bad cell type");
        }
    }
};

class Virtual_Stack {
private:
    std::vector<Stack_Cell> storage;

public:
    // How many items are on stack?
    unsigned count() const { return storage.size(); }

    // Get the top of the stack.
    const Stack_Cell &back() { return storage.back(); }

    // Remove all items (but don't deallocate).
    void erase()
    {
        storage.erase(storage.begin(), storage.end());
    }

    // Remove one item from stack, and return it as cell.
    Stack_Cell pop()
    {
        if (storage.size() == 0) {
            throw std::runtime_error("Cannot pop empty stack");
        }
        auto item = storage.back();
        storage.pop_back();
        return item;
    }

    // Remove one item from stack, and return it as standard floating point value.
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

    // Remove one item from stack, and return it as standard boolean value.
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

    // Get item by index.
    Stack_Cell get(unsigned index) const
    {
        if (index >= storage.size()) {
            throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
        }
        return storage[index];
    }

    // Push integer value in X1 format.
    void push_int_value(Word value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::INTEGER_VALUE;
        item.value = value;
    }

    // Push address of integer.
    void push_int_addr(Word addr)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::INTEGER_ADDRESS;
        item.value = addr;
    }

    // Push real value in X1 format.
    void push_real_value(Real value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_VALUE;
        item.value = value;
    }

    // Push address of real.
    void push_real_addr(Word addr)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_ADDRESS;
        item.value = addr;
    }

    // Push a standard floating point value.
    void push_ieee(long double value)
    {
        storage.push_back({});
        auto &item = storage.back();
        item.type  = Cell_Type::REAL_VALUE;
        item.value = ieee_to_x1(value);
    }
};

#endif // X1_VIRTUAL_STACK_H
