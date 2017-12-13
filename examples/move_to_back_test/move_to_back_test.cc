#include "keyed_queue.h"
#include <cassert>
#include <iostream>


int main () {
    int keys[] = {1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3};
    int vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int expected[] = {1, 3, 4, 6, 7, 9, 10, 12, 2, 5, 8, 11};

    keyed_queue<int, int> kq;
    for (int i = 0; i < 12; ++i) {
        kq.push(keys[i], vals[i]);
    }

    kq.move_to_back(2);
    for (int i = 0; i < 12; ++i) {
        std::cout << kq.front().second << "\n";
        assert(expected[i] == kq.front().second);
        kq.pop();
    }

    return 0;

}