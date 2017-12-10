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
#include <map>
#include <list>
#include <memory>
#include <iostream>
#include <sstream>

class lookup_error : std::exception {
public:


};

template <class K, class V>
class keyed_queue {
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
    
private:
    
    class queue_data {
    private: 
        std::map<K, V> keys;
        std::list<V> fifo;

    public:
        
        queue_data(): keys(), fifo() {
            
        }
        
        void push(K const &k, V const &v) {
            keys[k] = v;
        }
        
        void pop(K const &k, V const &v) {
            
        }
        
        void pop(K const &k) {
            
        }
        
        void pop() {
            
        }
        
        void move_to_back(K const &k) {
            
        }
        
        std::pair<K const &, V &> front() {
            
        }
        
        std::pair<K const &, V &> back() {
            
        }
        
        std::pair<K const &, V const &> front() const {
            
        }
        
        std::pair<K const &, V const &> back() const {
            
        }
        
        std::pair<K const &, V &> first(K const &key) {
            
        }
        
        std::pair<K const &, V &> last(K const &key) {
            
        }
        
        std::pair<K const &, V const &> first(K const &key) const {
            
        }
        
        std::pair<K const &, V const &> last(K const &key) const {
            
        }
        
        size_t size() const {
            return 0;
        }
        
        bool empty() const {
            return false;
        }
        
        void clear() {
            
        }
        
        size_t count(K const &k) const {
            return 0;
        }
        
        k_iterator k_begin() {
            
        }
        
        k_iterator k_end() {
            
        }
        
        std::string to_string() const {
            std::ostringstream out;
            
            out << "{";
            for(auto e : keys) {
                out << e.first << ": [" << e.second << "] ";
            }
            out << "}";
            
            return out.str();
        }
    };
    
    class queue_shared_data {
    private:
        std::unique_ptr<bool> sharable;
        std::shared_ptr<queue_data> shared_data;
        
    public:
    
        // Create empty non-shared object
        queue_shared_data(): sharable(std::make_unique<bool>(true)), shared_data(std::make_shared<queue_data>(queue_data())) {
            
        }
        
        // Share object
        queue_shared_data(queue_shared_data const& d): sharable(std::make_unique<bool>(true)) {
            if(*d.sharable) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
        }
        
        // Share object
        queue_shared_data(queue_shared_data&& d): sharable(std::make_unique<bool>(true)) {
            if(*d.sharable) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
        }
        
        // Share object
        queue_shared_data& operator= (queue_shared_data d) {
            if(*d.sharable) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
            return *this;
        }
        
        // Unshare object
        queue_shared_data& disjoin() {
            shared_data = std::make_shared<queue_data>(queue_data(*shared_data));
            return *this;
        }
        
        // Access underlying object
        queue_data& read() {
            return *shared_data;
        }
        
        // Access underlying object
        const queue_data& read_const() const {
            return *shared_data;
        }
        
        const queue_shared_data& unsharable() const {
            *sharable = false;
            return *this;
        }
        
        bool is_shared() const {
            return shared_data.use_count() > 1;
        }
    };
    
    queue_shared_data sd;
    
public:

    keyed_queue(): sd() {
         
    }
    
    keyed_queue(keyed_queue const& q): sd(q.sd) {
        
    }
    
    keyed_queue(keyed_queue&& q): sd(std::move(q.sd)) {
        
    }
    
    keyed_queue &operator=(keyed_queue other) {
        sd = other.sd;
        return *this;
    }
    
    void push(K const &k, V const &v) {
        sd.disjoin().read().push(k, v);
    }
    
    void pop() {
        sd.disjoin().read().pop();
    }
    
    void pop(K const &k, V const &v) {
        sd.disjoin().read().pop(k, v);
    }
    
    void pop(K const &k) {
        sd.disjoin().read().pop();
    }
    
    void move_to_back(K const &k) {
        sd.disjoin().read().move_to_back(k);
    }
    
    std::pair<K const &, V &> front() {
        return sd.disjoin().read().front();
    }
    
    std::pair<K const &, V &> back() {
        return sd.disjoin().read().back();
    }
    
    std::pair<K const &, V const &> front() const {
        return sd.unsharable().read_const().front();
    }
    
    std::pair<K const &, V const &> back() const {
        return sd.unsharable().read_const().back();
    }
    
    std::pair<K const &, V &> first(K const &key) {
        return sd.disjoin().read().first(key);
    }
    
    std::pair<K const &, V &> last(K const &key) {
        return sd.disjoin().read().last(key);
    }
    
    std::pair<K const &, V const &> first(K const &key) const {
        return sd.unsharable().read_const().first(key);
    }
    
    std::pair<K const &, V const &> last(K const &key) const {
        return sd.unsharable().read_const().last(key);
    }
    
    size_t size() const {
        return sd.read_const().size();
    }
    
    bool empty() const {
        return sd.read_const().empty();
    }
    
    void clear() {
        sd.disjoin().read().clear();
    }
    
    size_t count(K const &k) const {
        return sd.read_const().count(k);
    }
    
    k_iterator k_begin() {
        return sd.read().k_begin();
    }
    
    k_iterator k_end() {
        return sd.read().k_end();
    }
    
    std::string to_string() const {
        return ((sd.is_shared())?"<  shared  > ":"<standalone> ") + sd.read_const().to_string();
    }
};

#endif // _KEYED_QUEUE_