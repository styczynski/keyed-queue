/**
 * Univeristy of Warsaw 2017
 *  Task: KEYED QUEUE
 *  Authors:
 *     kk385830  @kowaalczyk-priv
 *     ps386038  @styczynski
 */
#ifndef _KEYED_QUEUE_
#define _KEYED_QUEUE_

#include <utility>
#include <exception>

class lookup_error : std::exception {
public:


};

template <class K, class V> class keyed_queue {
public:

    class k_iterator {
    public:
        k_iterator() {
            //TODO: Implement
            throw "Not implemented";
        }
        
        k_iterator(k_iterator const& i) {
            //TODO: Implement
            throw "Not implemented";
        }
        
        void operator++() {
            //TODO: Implement
            throw "Not implemented";
        }
        
        bool operator==(k_iterator k) {
            //TODO: Implement
            throw "Not implemented";
        }
        
        bool operator!=(k_iterator k) {
            //TODO: Implement
            throw "Not implemented";
        }
        
        K operator*() {
            //TODO: Implement
            throw "Not implemented";
        }
    };

    keyed_queue() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    keyed_queue(keyed_queue const& q) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    keyed_queue(keyed_queue&& q) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    keyed_queue &operator=(keyed_queue other) {
        //TODO: Implement
        throw "Not implemented";
        return *this;
    }
    
    void push(K const &k, V const &v) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    void pop() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    void pop(K const &k, V const &v) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    void pop(K const &k) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    void move_to_back(K const &k) {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V &> front() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V &> back() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V const &> front() const {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V const &> back() const {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V &> first(K const &key)  {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V &> last(K const &key)  {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V const &> first(K const &key) const  {
        //TODO: Implement
        throw "Not implemented";
    }
    
    std::pair<K const &, V const &> last(K const &key) const  {
        //TODO: Implement
        throw "Not implemented";
    }
    
    size_t size() const {
        //TODO: Implement
        throw "Not implemented";
        return 0;
    }
    
    bool empty() const {
        //TODO: Implement
        throw "Not implemented";
        return true;
    }
    
    void clear() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    size_t count(K const &) const {
        //TODO: Implement
        throw "Not implemented";
        return 0;
    }
    
    k_iterator k_begin() {
        //TODO: Implement
        throw "Not implemented";
    }
    
    k_iterator k_end() {
        //TODO: Implement
        throw "Not implemented";
    }
};

#endif // _KEYED_QUEUE_