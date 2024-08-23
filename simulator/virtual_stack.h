#ifndef X1_VIRTUAL_STACK_H
#define X1_VIRTUAL_STACK_H

#include <vector>

#include "x1_arch.h"

//
// Tag to indicate whether the cell contains the address of an integer variable
// or array, the address of a real variable or array, an integer value, or a real value.
//
enum class Cell_Type {
    NUL,             // Empty value
    INTEGER_ADDRESS, // Address of integer value
    REAL_ADDRESS,    // Address of real value
    INTEGER_VALUE,   // Integer value in bits 26:0
    REAL_VALUE,      // Real value in bits 53:0
};

//
// Stack cells of the hypothetic stack machine.
//
struct Stack_Cell {
    Cell_Type type;
    uint64_t value;

    // Quick check of the type of the value.
    bool is_null() const { return type == Cell_Type::NUL; }
    bool is_int_value() const { return type == Cell_Type::INTEGER_VALUE; }
    bool is_int_addr() const { return type == Cell_Type::INTEGER_ADDRESS; }
    bool is_real_value() const { return type == Cell_Type::REAL_VALUE; }
    bool is_real_addr() const { return type == Cell_Type::REAL_ADDRESS; }

    // Get value for a given type.
    unsigned get_addr() const { return value & BITS(27); }
    Word get_int() const { return value & BITS(27); }
    Real get_real() const { return value & BITS(54); }

    // Get sign: +1, 0 or -1.
    int sign() const;

    void set(uint64_t v) { value = v; }

    // Convert the value to string.
    std::string to_string() const;

    // Compare this item and another one.
    bool is_less(const Stack_Cell &another) const;
    bool is_equal(const Stack_Cell &another) const;

    // Raise this item to the power given by another cell.
    void exponentiate(const Stack_Cell &another);
    void exponentiate_int(int a, const Stack_Cell &another);
    void exponentiate_real(long double a, const Stack_Cell &another);

    // Multiply this item by another one.
    void multiply(const Stack_Cell &another);
    void multiply_real(Real another);

    // Divide this item by another one.
    void divide(const Stack_Cell &another);

    // Add another item to this one.
    void add(const Stack_Cell &another);
    void subtract(const Stack_Cell &another);
};

class Virtual_Stack {
private:
    std::vector<Stack_Cell> storage;

    // Some arbitrary limit on stack size.
    static unsigned const SIZE_LIMIT = 0100000;

public:
    // How many items are on stack?
    unsigned count() const { return storage.size(); }

    // Get the top of the stack.
    Stack_Cell &top() { return storage.back(); }

    // Remove all items (but don't deallocate).
    void erase(unsigned ptr) { storage.erase(storage.begin() + ptr, storage.end()); }

    // Remove one item from stack, and return it as cell.
    Stack_Cell pop();

    // Put an item on stack.
    void push(const Stack_Cell &item);
    void push(Cell_Type type, uint64_t value);

    // Remove one item from stack, and return it as standard floating point value.
    long double pop_ieee();

    // Remove one item from stack, and return it as standard integer value.
    int pop_integer();

    // Remove one item from stack, and return it as memory address.
    unsigned pop_addr();

    // Remove one item from stack, and return it as standard boolean value.
    bool pop_boolean();

    // Get item by index.
    Stack_Cell &get(unsigned index);
    const Stack_Cell &get(unsigned index) const;

    // Push any item on stack.

    // Push integer value in X1 format.
    void push_int_value(Word value) { push(Cell_Type::INTEGER_VALUE, value); }

    // Push address of integer.
    void push_int_addr(Word addr) { push(Cell_Type::INTEGER_ADDRESS, addr); }

    // Push real value in X1 format.
    void push_real_value(Real value) { push(Cell_Type::REAL_VALUE, value); }

    // Push address of real.
    void push_real_addr(Word addr) { push(Cell_Type::REAL_ADDRESS, addr); }

    // Push a standard floating point value.
    void push_ieee(long double value) { push(Cell_Type::REAL_VALUE, ieee_to_x1(value)); }

    // Push null item.
    void push_null() { push(Cell_Type::NUL, 0); }

    // Set item by index.
    void set(unsigned index, const Stack_Cell &item);
    void set_int_value(unsigned index, Word value)
    {
        set(index, { Cell_Type::INTEGER_VALUE, value });
    }
    void set_int_addr(unsigned index, Word addr)
    {
        set(index, { Cell_Type::INTEGER_ADDRESS, addr });
    }
    void set_real_value(unsigned index, Real value)
    {
        set(index, { Cell_Type::REAL_VALUE, value });
    }
    void set_real_addr(unsigned index, Word addr)
    {
        set(index, { Cell_Type::REAL_ADDRESS, addr });
    }
    void set_null(unsigned index)
    {
        set(index, { Cell_Type::NUL, 0 });
    }
};

#endif // X1_VIRTUAL_STACK_H
