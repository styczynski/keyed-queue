# Keyed queue

&nbsp;&nbsp;&nbsp;[@styczynski](https://github.com/styczynski)&nbsp;&nbsp;&nbsp;-&nbsp;code of keyed_queue<br>
&nbsp;&nbsp;&nbsp;[@kowaalczyk](https://github.com/kowaalczyk)&nbsp;-&nbsp;increadibly useful handful of tests

## About

This library was written on the *Faculty of Mathematics, Informathics and Mechanics, Warsaw Univeristy*<br>
as a part of **JNP1** (en. Languages and Tools for Programming) subject during **2017/2018** term.

Class `keyed_queue` implements (copy-on-write) queue with map-like keys support. 

## Interface

**keyed_queue<typename K, typename V>** class implements the following methods:

- **void push(K const &k, V const &v)**<br>
   Inserts value v to the end of the queue assigning the key k. 


- **void pop()**<br>
   Removes first element from the queue. If the queue is empty then `lookup_error` is thrown.


- **void pop(K const &)**<br>
   Removes the first element from the queue that has the given key value. If no element with such key exists in the queue then `lookup_error` is thrown.


- **void move_to_back(K const &k)**<br>
   Moves the elements with the given key to the end of the queue preserving their relative order. If no element with the provided key exists in the queue then `lookup_error` is thrown.


- **std::pair<K const &, V &>  front()**<br>
   Returns key, value pair for element at the queue front. If the queue is empty then `lookup_error` is thrown. This method allows value modification by reference. Any writing operation performed on the queue may invalidate captured references. Value is unmodificable in `const` version of the method.

- **std::pair<K const &, V &>  back()**<br>
   Returns key, value pair for element at the queue back. If the queue is empty then `lookup_error` is thrown. This method allows value modification by reference. Any writing operation performed on the queue may invalidate captured references. Value is unmodificable in `const` version of the method.

- **std::pair<K const &, V &> front(K const&)**<br>
   Returns key, value pair for the first element on the queue with the given key. If the queue is empty or does not contain any element with the given key then `lookup_error` is thrown. This method allows value modification by reference. Any writing operation performed on the queue may invalidate captured references. Value is unmodificable in `const` version of the method.

- **std::pair<K const &, V &> last(K const&)**<br>
   Returns key, value pair for the last element on the queue with the given key. If the queue is empty or does not contain any element with the given key then `lookup_error` is thrown. This method allows value modification by reference. Any writing operation performed on the queue may invalidate captured references. Value is unmodificable in `const` version of the method.

- **size_t size()**<br>
   Returns the number of elements on the queue


- **bool empty()**<br>
   Returns true if the queue is empty


- **void clear()**<br>
   Clears the queue

- **size_t count(K const &)**<br>
   Counts elements with the given key

- **keyed_queue::k_iterator k_begin()**<br>
   Returns iterator to the first element in the queue. Can be used to iterate queue in STL-like style.

- **keyed_queue::k_iterator k_end()**<br>
   Returns past-the-end iterator to the queue. Can be used to iterate queue in STL-like style.

# Building

The examples require the **GCC 7.2.0** compiler to be built.<br> 
Other versions may work but they do not have to.<br>
Library itself is portable but the makefile is not. 

* Use **make all** to build all examples
* Use **make run-basic-usage** and **make run-<name>** to run the compiled examples
* Use **make clean** to clean the build

# Example usage 

```c++
#include "keyed_queue.h"
#include <vector>
#include <memory>
#include <iostream>

int main() {

   keyed_queue<int, int> q1;

   q1.push(1, 1);
   std::cout << q1.to_string() << "\n";

   keyed_queue<int, int> q2 = q1;
   std::cout << q2.to_string() << "\n";

   q1.push(2, 2);
   std::cout << q1.to_string() << "\n";

   q2.push(3, 3);
   std::cout << q2.to_string() << "\n";

   q2.move_to_back(1);
   std::cout << q2.to_string() << "\n";

   return 0;
}
    
```

