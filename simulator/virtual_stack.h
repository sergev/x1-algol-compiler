#ifndef X1_VIRTUAL_STACK_H
#define X1_VIRTUAL_STACK_H

#include "x1_arch.h"
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
    bool is_less(const Stack_Cell &another);
    bool is_equal(const Stack_Cell &another);
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
    void erase() { storage.erase(storage.begin(), storage.end()); }

    // Remove one item from stack, and return it as cell.
    Stack_Cell pop();

    // Remove one item from stack, and return it as standard floating point value.
    long double pop_ieee();

    // Remove one item from stack, and return it as standard integer value.
    int pop_integer();

    // Remove one item from stack, and return it as standard boolean value.
    bool pop_boolean();

    // Get item by index.
    Stack_Cell get(unsigned index) const;

    // Push integer value in X1 format.
    void push_int_value(Word value);

    // Push address of integer.
    void push_int_addr(Word addr);

    // Push real value in X1 format.
    void push_real_value(Real value);

    // Push address of real.
    void push_real_addr(Word addr);

    // Push a standard floating point value.
    void push_ieee(long double value);
};

#endif // X1_VIRTUAL_STACK_H