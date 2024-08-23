#include "virtual_stack.h"
#include "machine.h"

#include <sstream>
#include <cmath>

//
// Compare (relatively) this item and another one.
//
bool Stack_Cell::is_less(const Stack_Cell &another) const
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
bool Stack_Cell::is_equal(const Stack_Cell &another) const
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
            return this_int == (int) another.get_addr();
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
            return (int) this_addr == x1_to_integer(another.get_int());
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
// Convert the value to string.
//
std::string Stack_Cell::to_string() const
{
    std::ostringstream buf;
    switch (type) {
    case Cell_Type::INTEGER_VALUE: {
        buf << "integer " << std::dec;
        if (value & ONEBIT(26)) {
            // Negative.
            buf << '-' << x1_to_integer(value ^ BITS(27));
        } else {
            buf << x1_to_integer(value);
        }
        buf << 'd';
        break;
    }
    case Cell_Type::REAL_VALUE:
        buf << "real " << x1_to_ieee(get_real());
        break;
    case Cell_Type::INTEGER_ADDRESS:
    case Cell_Type::REAL_ADDRESS:
        buf << "addr " << std::oct << get_addr();
        break;
    case Cell_Type::NUL:
        buf << "nan";
        break;
    default:
        throw std::runtime_error("Bad cell type");
    }
    return buf.str();
}

//
// Get sign: +1, 0 or -1.
//
int Stack_Cell::sign() const
{
    switch (type) {
    case Cell_Type::INTEGER_VALUE: {
        auto i = x1_to_integer(get_int());
        return (i > 0) ? 1 : (i < 0) ? -1 : 0;
    }
    case Cell_Type::REAL_VALUE: {
        auto r = x1_to_ieee(get_real());
        return (r > 0.0) ? 1 : (r < 0.0) ? -1 : 0;
    }
    default:
        throw std::runtime_error("Cannot get sign");
    }
}

//
// Raise this item to the power given by another cell.
//
// Writing i for a number of integer type, r for a number of real type,
// and a for a number of either integer or real type,
// the result is given by the following rules:
//
// a↑i  if i>0:  a×a×...×a (i times), of the same type as a.
//      if i=0:  if a≠0: 1, of the same type as a.
//               if a=0: undefined.
//      if i<0,  if a≠0: 1/(a×a×a×...×a) (the denominator has
//                       −i factors), of type real.
//               if a=0: undefined.
// a↑r  if a>0:  exp(r × ln(a)), of type real.
//      if a=0,  if r>0: 0.0, of type real.
//               if r≤0: undefined.
//      if a<0:  always undefined.
//
void Stack_Cell::exponentiate(const Stack_Cell &another)
{
    switch (type) {
    case Cell_Type::INTEGER_VALUE:
        exponentiate_int(x1_to_integer(get_int()), another);
        break;
    case Cell_Type::REAL_VALUE:
        exponentiate_real(x1_to_ieee(get_real()), another);
        break;
    default:
        throw std::runtime_error("Cannot exponentiate address");
    }
}

//
// Multiply this item by another one.
//
void Stack_Cell::multiply(const Stack_Cell &another)
{
    if (is_int_value() && another.is_int_value()) {
        // Multiply two integers with integer result.
        int a = x1_to_integer(get_int());
        int b = x1_to_integer(another.get_int());
        value = integer_to_x1(a * b);
        return;
    }

    // Multiply two values with real result.
    long double a = is_real_value() ? x1_to_ieee(get_real()) :
                                      x1_to_integer(get_int());
    long double b = another.is_real_value() ? x1_to_ieee(another.get_real()) :
                                      x1_to_integer(another.get_int());
    value = ieee_to_x1(a * b);
    type  = Cell_Type::REAL_VALUE;
}

void Stack_Cell::multiply_real(Real another)
{
    // Multiply two values with real result.
    long double a = is_real_value() ? x1_to_ieee(get_real()) :
                                      x1_to_integer(get_int());
    long double b = x1_to_ieee(another);

    value = ieee_to_x1(a * b);
    type  = Cell_Type::REAL_VALUE;
}

//
// Divide this item by another one.
//
void Stack_Cell::divide(const Stack_Cell &another)
{
    // Divide two values with real result.
    long double a = is_real_value() ? x1_to_ieee(get_real()) :
                                      x1_to_integer(get_int());
    long double b = another.is_real_value() ? x1_to_ieee(another.get_real()) :
                                      x1_to_integer(another.get_int());
    if (b == 0) {
        throw std::runtime_error("Divide by zero");
    }
    value = ieee_to_x1(a / b);
    type  = Cell_Type::REAL_VALUE;
}

//
// Add another item to this one.
//
void Stack_Cell::add(const Stack_Cell &another)
{
    if (is_int_value() && another.is_int_value()) {
        // Add two integers with integer result.
        int a = x1_to_integer(get_int());
        int b = x1_to_integer(another.get_int());
        value = integer_to_x1(a + b);
        return;
    }

    // Add two real/integer values with real result.
    long double a = is_real_value() ? x1_to_ieee(get_real()) :
                                      x1_to_integer(get_int());
    long double b = another.is_real_value() ? x1_to_ieee(another.get_real()) :
                                      x1_to_integer(another.get_int());
    value = ieee_to_x1(a + b);
    type  = Cell_Type::REAL_VALUE;
}

//
// Subtract another item to this one.
//
void Stack_Cell::subtract(const Stack_Cell &another)
{
    if (is_int_value() && another.is_int_value()) {
        // Add two integers with integer result.
        int a = x1_to_integer(get_int());
        int b = x1_to_integer(another.get_int());
        value = integer_to_x1(a - b);
        return;
    }

    // Add two real/integer values with real result.
    long double a = is_real_value() ? x1_to_ieee(get_real()) :
                                      x1_to_integer(get_int());
    long double b = another.is_real_value() ? x1_to_ieee(another.get_real()) :
                                      x1_to_integer(another.get_int());
    value = ieee_to_x1(a - b);
    type  = Cell_Type::REAL_VALUE;
}

//
// Iterative function to calculate integer pow(x, n).
//
static int power(int x, unsigned n)
{
    int result = 1;
    while (n) {
        if (n & 1) {
            result *= x;
        }
        n = n >> 1;
        x *= x;
    }
    return result;
}

//
// Iterative function to calculate real pow(x, n).
//
static long double power(long double x, unsigned n)
{
    long double result = 1;
    while (n) {
        if (n & 1) {
            result *= x;
        }
        n = n >> 1;
        x *= x;
    }
    return result;
}

void Stack_Cell::exponentiate_int(int a, const Stack_Cell &another)
{
    switch (another.type) {
    case Cell_Type::INTEGER_VALUE: {
        int i = x1_to_integer(another.get_int());
        if (i > 0) {
            // a×a×...×a (i times), of the same type as a.
            value = integer_to_x1(power(a, i));
        } else if (i == 0) {
            // if a≠0: 1, of the same type as a.
            // if a=0: undefined.
            if (a == 0) {
                throw std::runtime_error("Exponent 0↑0 is undefined");
            }
            value = 1;
        } else {
            // if a≠0: 1/(a×a×a×...×a) (the denominator has
            //         −i factors), of type real.
            // if a=0: undefined.
            if (a == 0) {
                throw std::runtime_error("Division by zero");
            }
            type = Cell_Type::REAL_VALUE;
            value = ieee_to_x1(power(1.0L/a, -i));
        }
        break;
    }
    case Cell_Type::REAL_VALUE: {
        auto r = x1_to_ieee(another.get_real());
        if (a > 0) {
            // exp(r × ln(a)), of type real.
            type = Cell_Type::REAL_VALUE;
            value = ieee_to_x1(expl(r * logl(a)));
        } else if (a == 0) {
            // if r>0: 0.0, of type real.
            // if r≤0: undefined.
            if (r <= 0) {
                throw std::runtime_error("Cannot raise zero to negative or zero power");
            }
            type = Cell_Type::REAL_VALUE;
            value = 0;
        } else {
            // always undefined.
            throw std::runtime_error("Cannot raise negative value to real power");
        }
        break;
    }
    default:
        throw std::runtime_error("Cannot exponentiate to the address");
    }
}

void Stack_Cell::exponentiate_real(long double a, const Stack_Cell &another)
{
    switch (another.type) {
    case Cell_Type::INTEGER_VALUE: {
        int i = x1_to_integer(another.get_int());
        if (i > 0) {
            // a×a×...×a (i times), of the same type as a.
            value = ieee_to_x1(power(a, i));
        } else if (i == 0) {
            // if a≠0: 1, of the same type as a.
            // if a=0: undefined.
            if (a == 0) {
                throw std::runtime_error("Exponent 0↑0 is undefined");
            }
            value = ieee_to_x1(1.0);
        } else {
            // if a≠0: 1/(a×a×a×...×a) (the denominator has
            //         −i factors), of type real.
            // if a=0: undefined.
            if (a == 0) {
                throw std::runtime_error("Division by zero");
            }
            value = ieee_to_x1(power(1/a, -i));
        }
        break;
    }
    case Cell_Type::REAL_VALUE: {
        auto r = x1_to_ieee(another.get_real());
        if (a > 0) {
            // exp(r × ln(a)), of type real.
            value = ieee_to_x1(expl(r * logl(a)));
        } else if (a == 0) {
            // if r>0: 0.0, of type real.
            // if r≤0: undefined.
            if (r <= 0) {
                throw std::runtime_error("Cannot raise zero to negative or zero power");
            }
            value = 0;
        } else {
            // always undefined.
            throw std::runtime_error("Cannot raise negative value to real power");
        }
        break;
    }
    default:
        throw std::runtime_error("Cannot exponentiate to the address");
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
    Machine::trace_stack(storage.size(), item.to_string(), "Pop");
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
    Machine::trace_stack(storage.size(), item.to_string(), "Pop");
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
    Machine::trace_stack(storage.size(), item.to_string(), "Pop");
    if (item.is_int_value()) {
        return x1_to_integer(item.get_int());
    } else if (item.is_real_value()) {
        throw std::runtime_error("Need integer, got real");
    } else {
        throw std::runtime_error("Need integer, got address");
    }
}

//
// Remove one item from stack, and return it as memory address.
//
unsigned Virtual_Stack::pop_addr()
{
    if (storage.size() == 0) {
        throw std::runtime_error("Cannot pop empty stack");
    }
    auto item = storage.back();
    storage.pop_back();
    Machine::trace_stack(storage.size(), item.to_string(), "Pop");
    if (item.is_int_value()) {
        throw std::runtime_error("Need address, got integer");
    } else if (item.is_real_value()) {
        throw std::runtime_error("Need address, got real");
    } else {
        return item.get_addr();
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
    Machine::trace_stack(storage.size(), item.to_string(), "Pop");
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
Stack_Cell &Virtual_Stack::get(unsigned index)
{
    if (index >= storage.size()) {
        throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
    }
    return storage[index];
}

const Stack_Cell &Virtual_Stack::get(unsigned index) const
{
    if (index >= storage.size()) {
        throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
    }
    return storage[index];
}

//
// Set item by index.
//
void Virtual_Stack::set(unsigned index, const Stack_Cell &item)
{
    if (index >= storage.size()) {
        throw std::runtime_error("No item #" + std::to_string(index) + " in stack");
    }
    storage[index] = item;
    Machine::trace_stack(index, item.to_string(), "Write");
}

//
// Push any item on stack.
//
void Virtual_Stack::push(const Stack_Cell &item)
{
    if (storage.size() >= SIZE_LIMIT) {
        throw std::runtime_error("Stack overflow");
    }
    storage.push_back(item);
    Machine::trace_stack(storage.size() - 1, storage.back().to_string(), "Push");
}

void Virtual_Stack::push(Cell_Type type, uint64_t value)
{
    storage.push_back({});
    auto &item = storage.back();
    item.type  = type;
    item.value = value;
    Machine::trace_stack(storage.size() - 1, item.to_string(), "Push");
}
