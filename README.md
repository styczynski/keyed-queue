# Keyed queue

&nbsp;&nbsp;&nbsp;[@styczynski](https://github.com/styczynski)&nbsp;&nbsp;&nbsp;-&nbsp;code of keyed_queue<br>
&nbsp;&nbsp;&nbsp;[@kowaalczyk](https://github.com/kowaalczyk)&nbsp;-&nbsp;increadibly useful handful of tests

## About

This library was written on the Faculty of Mathematics, Informathics and Mechanics, Warsaw Univeristy 
as a part of JNP1 (en. Languages and Tools for Programming) subject in years 2017/2018.

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

# Example usage 

```c++
    #include "keyed_queue.h"
    #include <cassert>
    #include <vector>
    
    auto f(keyed_queue<int, int> q) {
      return q;
    }
    
    int main() {
      
      int keys[] = {3, 1, 2};
      keyed_queue<int, int> kq1 = f({});
    
      for (int i = 0; i < 3; ++i) {
        kq1.push(keys[i], i);
      }
      
      auto &ref = kq1.front().second;
      keyed_queue<int, int> kq2(kq1);
      keyed_queue<int, int> kq3;
      kq3 = kq2;
    
      ref = 10;
      assert(kq1.front().second == 10);
      assert(kq2.front().second != 10);
    
      kq2.pop(); // Obiekt kq2 dokonuje kopii i przestaje współdzielić dane z kq3.
      assert(kq2.size() == 2);
      assert(kq2.count(3) == 0);
      assert(kq2.count(2) == 1);
    
      assert(kq3.size() == 3);
      assert(kq3.count(3) == 1);
    
      kq2.push(1, 3);
      kq2.move_to_back(1);
      assert(kq2.size() == 3);
      assert(kq2.front().second == 2
             && kq2.first(1).second == 1
             && kq2.last(1).second == 3
             && kq2.back().second == 3);
    
      keyed_queue<int, int> const kq4 = kq2;
      assert(kq4.front().second == 2
             && kq4.first(1).second == 1
             && kq4.last(1).second == 3
             && kq4.back().second == 3);
    
      int i = 1;
      for (auto k_it = kq1.k_begin(), k_end = kq1.k_end();
           k_it != k_end;
           ++k_it, ++i) {
        assert(i <= 3 && *k_it == i);
      }
    
      auto kq5 = std::make_unique<keyed_queue<int, int>>();
      kq5->push(4, 0);
      assert(kq5->front().first == 4 && kq5->front().second == 0);
      auto kq6(*kq5);
      kq5.reset();
      assert(kq6.front().first == 4 && kq6.front().second == 0);
    
      std::swap(kq1, kq2);
      std::vector<keyed_queue<int, int>> vec;
      for (int i = 0; i < 100000; i++) {
        kq1.push(i, i);
      }
      for (int i = 0; i < 1000000; i++) {
        vec.push_back(kq1);  // Wszystkie obiekty w vec współdzielą dane.
      }
    
      return 0;
    }
    
```

