/**
 * Univeristy of Warsaw 2017
 *  Task: KEYED QUEUE
 *  Authors:
 *     kk385830  @kowaalczyk-priv
 *     ps386038  @styczynski
 */
#include "keyed_queue.h"
#include <cassert>
#include <vector>
#include <memory>

int main()
{
  keyed_queue<int, int> q1;
  q1.push(1, 1);
  std::cout << q1.to_string() << "\n";
  
  keyed_queue<int, int> q2 = q1;
  std::cout << q2.to_string() << "\n";
  q1.push(2, 2);
  std::cout << q1.to_string() << "\n";
  q2.push(3, 3);
  std::cout << q2.to_string() << "\n";
    
  return 0;
}
