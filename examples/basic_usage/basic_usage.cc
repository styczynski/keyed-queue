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
  std::cout << "A\n";
  keyed_queue<int, int> q1;
  
  std::cout << "B\n";
  q1.push(1, 1);
  
  std::cout << "C\n";
  std::cout << q1.to_string() << "\n";
  
  std::cout << "D\n";
  keyed_queue<int, int> q2 = q1;
  
  std::cout << "E\n";
  std::cout << q2.to_string() << "\n";
  
  std::cout << "F\n";
  q1.push(2, 2);
  
  std::cout << "G\n";
  std::cout << q1.to_string() << "\n";
  
  std::cout << "H\n";
  q2.push(3, 3);
  std::cout << q2.to_string() << "\n";
    
  std::cout << "I\n";
  q2.move_to_back(1);
  std::cout << q2.to_string() << "\n";
  
  std::cout << "J\n";
    
  return 0;
}
