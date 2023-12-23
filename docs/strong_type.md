# `gw::strong_type`

The class template [`gw::strong_type`](../include/gw/strong_type.hpp) is a simple wrapper around a value of some type `T`. It is intended to be used to create a new type that is distinct from `T`, but which has the same representation and semantics as `T`. This is useful to distinguish between values that are conceptually different, but which have the same underlying type. For example, a `std::string` can be used to represent a person's name, but it can also be used to represent a person's address. Using `gw::strong_type` to create a `name_t` and an `address_t` allows the compiler to catch errors where a `name_t` is used where an `address_t` is expected, and vice versa.

## Example

```cpp
#include <gw/strong_type.hpp>
#include <iostream>
#include <string>

using name_t = gw::strong_type<struct name_tag, std::string>;
using address_t = gw::strong_type<struct address_tag, std::string>;

void print_name(const name_t& name) {
  std::cout << name << '\n';
}

void print_address(const address_t& address) {
  std::cout << address << '\n';
}

int main() {
  auto name = name_t{"John Doe"};
  auto address = address_t{"123 Main St."};

  print_name(name);
  print_address(address);

  // The following lines will not compile, because the types are distinct.
  // print_name(address);
  // print_address(name);
}
```

## Interface

### Template parameters

| Parameter | Description |
| --------- | ----------- |
| `T` | the type of the wrapped value |
| `Tag` | tag type used to distinguish between different instantiations of `gw::strong_type` |

### Member types

| Member type | Definition |
| ----------- | ----------- |
| `tag_type` | `Tag` |
| `value_type` | `T` |

### Member functions

| Member function | Description |
| --------------- | ----------- |
| (constructor) | constructs the `gw::strong_type` |
| (destructor) | destroys the `gw::strong_type` |

#### Observers

|   |   |
|---|---|
| `operator->` | returns a pointer to the wrapped value |
| `operator*` | returns a reference to the wrapped value |
| `value` | returns a reference to the wrapped value |

#### Monadic operations

|   |   |
|---|---|
| `transform` | returns a `gw::strong_type` containing the transformed contained value |

#### Modifiers

|   |   |
|---|---|
| `swap` | exchanges the contents |
| `reset` | destroys any contained value |
| `emplace` | constructs the contained value in-place |

#### Comparison operators

|   |   |
|---|---|
| `operator<=>` | compares `gw::strong_type` objects |

#### Conversion operators

|   |   |
|---|---|
| `operator T` | returns a reference to the wrapped value |

#### Increment and decrement operators

|   |   |
|---|---|
| `operator++` | increments the wrapped value |
| `operator--` | decrements the wrapped value |
| `operator++(int)` | increments the wrapped value |
| `operator--(int)` | decrements the wrapped value |

#### Arithmetic operators

|   |   |
|---|---|
| `operator+` | adds the wrapped value to another value |
| `operator-` | subtracts another value from the wrapped value |
| `operator*` | multiplies the wrapped value by another value |
| `operator/` | divides the wrapped value by another value |
| `operator%` | computes the remainder of the wrapped value divided by another value |
| `operator+=` | adds another value to the wrapped value |
| `operator-=` | subtracts another value from the wrapped value |
| `operator*=` | multiplies the wrapped value by another value |
| `operator/=` | divides the wrapped value by another value |
| `operator%=` | computes the remainder of the wrapped value divided by another value |

#### Bitwise operators

|   |   |
|---|---|
| `operator~` | bitwise negation of the wrapped value |
| `operator&` | bitwise AND of the wrapped value and another value |
| `operator\|` | bitwise OR of the wrapped value and another value |
| `operator^` | bitwise XOR of the wrapped value and another value |
| `operator<<` | bitwise left shift of the wrapped value by another value |
| `operator>>` | bitwise right shift of the wrapped value by another value |
| `operator&=` | bitwise AND of the wrapped value and another value |
| `operator\|=` | bitwise OR of the wrapped value and another value |
| `operator^=` | bitwise XOR of the wrapped value and another value |
| `operator<<=` | bitwise left shift of the wrapped value by another value |
| `operator>>=` | bitwise right shift of the wrapped value by another value |

#### Stream operators

|   |   |
|---|---|
| `operator<<` | writes the wrapped value to an output stream |
| `operator>>` | reads the wrapped value from an input stream |

### Non-member functions

| Non-member function | Description |
| --------------- | ----------- |
| `gw::make_strong_type` | creates a `gw::strong_type` object |
| `std::to_string` | converts a `gw::strong_type` object to a `std::string` |

### Helper classes

| Helper class | Description |
| ------------ | ----------- |
| `std::hash<gw::strong_type>` | hash support for `gw::strong_type` |
