#include "virtual_stack.h"

//
// Compare (relatively) this item and another one.
//
bool Stack_Cell::is_less(const Stack_Cell &another)
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

//
// Compare (strictly) this item and another one.
//
bool Stack_Cell::is_equal(const Stack_Cell &another)
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

//
// Remove one item from stack, and return it as cell.
//
Stack_Cell Virtual_Stack::pop()
{
    if (storage.size() == 0) {
        throw std::runtime_error("Cannot pop empty stack");
    }
    auto item = storage.back();
    storage.pop_back();
    return item;
}

//
// Remove one item from stack, and return it as standard floating point value.
//
long double Virtual_Stack::pop_ieee()
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

//
// Remove one item from stack, and return it as standard integer point value.
//
int Virtual_Stack::pop_integer()
{
    if (storage.size() == 0) {
        throw std::runtime_error("Cannot pop empty stack");
    }
    auto item = storage.back();
    storage.pop_back();
    if (item.is_int_value()) {
        return x1_to_integer(item.get_int());
    } else if (item.is_real_value()) {
        throw std::runtime_error("Need integer, got real");
    } else {
        throw std::runtime_error("Need integer, got address");
    }
}

//
// Remove one item from stack, and return it as standard boolean value.
//
bool Virtual_Stack::pop_boolean()
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

//
// Get item by index.
//
Stack_Cell Virtual_Stack::get(unsigned index) const
{
    if (index >= storage.size()) {
        throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
    }
    return storage[index];
}

//
// Push integer value in X1 format.
//
void Virtual_Stack::push_int_value(Word value)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = Cell_Type::INTEGER_VALUE;
    item.value = value;
}

//
// Push address of integer.
//
void Virtual_Stack::push_int_addr(Word addr)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = Cell_Type::INTEGER_ADDRESS;
    item.value = addr;
}

//
// Push real value in X1 format.
//
void Virtual_Stack::push_real_value(Real value)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = Cell_Type::REAL_VALUE;
    item.value = value;
}

//
// Push address of real.
//
void Virtual_Stack::push_real_addr(Word addr)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = Cell_Type::REAL_ADDRESS;
    item.value = addr;
}

//
// Push a standard floating point value.
//
void Virtual_Stack::push_ieee(long double value)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = Cell_Type::REAL_VALUE;
    item.value = ieee_to_x1(value);
}