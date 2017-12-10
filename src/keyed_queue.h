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
#include <assert.h>

class lookup_error : std::exception {
private:
    std::string what_message;
public:
    lookup_error(std::string additionalDescription) {
        std::ostringstream out;
        out << "Keyed queue lookup error. ";
        out << additionalDescription;
        what_message = out.str();
    }

    const char* what() {
        return what_message.c_str();
    }
};

template <class K, class V>
class keyed_queue {
private:
    using kv_pair = std::pair<K, V>;
    using kv_list = std::list<kv_pair>;
    using kv_list_i = typename kv_list::iterator;
    using kvi_list = std::list<kv_list_i>;
    using kvi_list_i = typename kvi_list::iterator;
    using kvi_map = std::map<K, kvi_list>;
    using kvi_map_i = typename kvi_map::iterator;

    
public:

    class k_iterator {
    private:
        kvi_map_i map_iter;
        bool assigned;
        
    public:
    
        k_iterator(const kvi_map_i i) {
            map_iter = i;
            assigned = true;
        }
    
        k_iterator() noexcept {
            assigned = false;
        }
        
        k_iterator(k_iterator const& i) {
            assigned = i.assigned;
            map_iter = i.map_iter;
        }
        
        void operator++() noexcept {
            if(assigned) ++map_iter;
        }
        
        bool operator==(k_iterator k) noexcept {
            if(!assigned || !k.assigned) return false;
            return map_iter == k.map_iter;
        }
        
        bool operator!=(k_iterator k) noexcept {
            if(!assigned || !k.assigned) return true;
            return map_iter != k.map_iter;
        }
        
        K operator*() {
            if(!assigned) throw "k_iterator: Dereferencing unassigned iterator.";
            return map_iter->first;
        }
    };
    
private:
    
    class queue_data {
    private:
        kvi_map keys;
        kv_list fifo;

    public:
      
        queue_data(const queue_data& q) {
            for(auto i=q.fifo.begin(); i!=q.fifo.end(); ++i) {
                fifo.push_back(*i);
                auto el_i = fifo.end();
                --el_i;
                const auto l = keys.insert({ i->first, {} });
                (l.first)->second.push_back(el_i);
            }
        }
        
        queue_data(): keys(), fifo() {
            
        }
        
        queue_data(queue_data&& q): keys(std::move(q.keys)),
                                    fifo(std::move(q.fifo)) {
        
        }
        
        void push(K const &k, V const &v) {
            
            //TODO: This do not provide ANY exception guarantee!
            // Idk check this shit out plz
            
            kv_list fifo_delta;
            
            try {
                fifo_delta.push_back({k, v}); // < This may fail!
            } catch(...) {
                throw;
            }
            
            fifo.splice(fifo.end(), fifo_delta); // < Safe
            
            const auto l = keys.insert({ k, {} }); // < This may too? Maybe?
            auto delta_i = fifo.end();
            --delta_i;
            (l.first)->second.push_back(delta_i); // < Safe
            
        }
        
        void pop(K const &k) {
            const auto iter = keys.find(k);
            if(iter == keys.end()) {
                throw lookup_error("pop(K): Key not present in the queue.");
            } else {
                // TODO: Validate
                const auto e = (iter->second).front();
                fifo.erase(e);
                (iter->second).pop_front();
                if((iter->second).size() <= 0) {
                    keys.erase(iter);
                }
            }
        }
        
        void pop() {
            if(fifo.empty()) {
                throw lookup_error("pop(): Queue is empty.");
            }
            
            // TODO: Validate
            const auto iter = fifo.begin();
            const auto key = iter->first;
            
            const auto i = keys.find(key);
            assert(i != keys.end());
            
            auto& list = i->second;
            list.pop_front();
            
            if(list.size() <= 0) {
                keys.erase(i);
            }
            
            fifo.pop_front();
        }
        
        void move_to_back(K const &k) {
            const auto i = keys.find(k);
            if(i == keys.end()) {
                throw lookup_error("move_to_back(K): There's no such key in the queue.");
            }
            
            // TODO: Validate
            auto& list = i->second;
            for(auto e=list.begin();e!=list.end();++e) {
                fifo.push_back(**e);
                auto new_e = fifo.end();
                --new_e;
                fifo.erase(*e);
                *e = new_e;
            }
        }
        
        std::pair<K const &, V &> front() {
            if(fifo.empty()) {
                throw lookup_error("front(): Queue is empty.");
            }
            auto& ref = fifo.front();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        std::pair<K const &, V &> back() {
            if(fifo.empty()) {
                throw lookup_error("back(): Queue is empty.");
            }
            auto& ref = fifo.back();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        std::pair<K const &, V const &> front() const {
            if(fifo.empty()) {
                throw lookup_error("front(): Queue is empty.");
            }
            auto& ref = fifo.front();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        std::pair<K const &, V const &> back() const {
            if(fifo.empty()) {
                throw lookup_error("back(): Queue is empty.");
            }
            auto& ref = fifo.back();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        std::pair<K const &, V &> first(K const &key) {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("first(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.front())->second };
        }
        
        std::pair<K const &, V &> last(K const &key) {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("last(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.back())->second };
        }
        
        std::pair<K const &, V const &> first(K const &key) const {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("first(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.front())->second };
        }
        
        std::pair<K const &, V const &> last(K const &key) const {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("last(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.back())->second };
        }
        
        size_t size() const noexcept {
            return fifo.size();
        }
        
        bool empty() const noexcept {
            return fifo.empty();
        }
        
        void clear() noexcept {
            fifo.clear();
            keys.clear();
        }
        
        size_t count(K const &k) const noexcept {
            const auto i = keys.find(k);
            if(i != keys.end()) {
                return i->second.size();
            }
            return 0;
        }
        
        k_iterator k_begin() {
            return k_iterator(keys.begin());
        }
        
        k_iterator k_end() {
            return k_iterator(keys.end());
        }
        
        std::string to_string() const noexcept {
            std::ostringstream out;
            
            // Debug section
            /*
            out << "{\n";
            out << " * keys:\n";
            for(auto k : keys) {
              out << "   " << k.first << ": ";
              for(auto e: k.second) {
                out << "     " << e->first << "=> " << e->second << "\n";
              }
              out << "\n";
            }
            out << " * fifo:\n";
            */
            
            out << "{ ";
            bool isFirst = true;
            for(auto e : fifo) {
                if(!isFirst) {
                    out << ", ";
                } else {
                    isFirst = false;
                }
                out << e.first << " => " << e.second;
            }
            out << " }";
            
            return out.str();
        }
    };
    
    class queue_shared_data {
    private:
        std::shared_ptr<bool> shareableState;
        std::shared_ptr<queue_data> shared_data;
        
    public:
    
        // Create empty non-shared object
        queue_shared_data(): shareableState(std::make_shared<bool>(true)), shared_data(std::make_shared<queue_data>(queue_data())) {
        
        }
        
        // Share object
        queue_shared_data(queue_shared_data const& d): shareableState(std::make_shared<bool>(true)) {
            if(*d.shareableState) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
        }
        
        // Share object
        queue_shared_data(queue_shared_data&& d): shareableState(std::make_shared<bool>(true)) {
            if(*d.shareableState) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
        }
        
        // Share object
        queue_shared_data& operator= (queue_shared_data d) {
            if(*d.shareableState) {
                shared_data = d.shared_data;
            } else {
                shared_data = std::make_shared<queue_data>(*d.shared_data);
            }
            return *this;
        }
        
        // Unshare object
        queue_shared_data& disjoin() {
            if(!is_shared()) return *this;
            shared_data = std::make_shared<queue_data>(queue_data(*shared_data));
            return *this;
        }
        
        // Access underlying object
        queue_data& read() noexcept {
            return *shared_data;
        }
        
        // Access underlying object
        const queue_data& read_const() const noexcept {
            return *shared_data;
        }
        
        const queue_shared_data& unshareable() const noexcept {
            *shareableState = false;
            return *this;
        }
        
        queue_shared_data& unshareable() noexcept {
            *shareableState = false;
            return *this;
        }
        
        const queue_shared_data& shareable() const noexcept {
            *shareableState = true;
            return *this;
        }
        
        queue_shared_data& shareable() noexcept {
            *shareableState = true;
            return *this;
        }
        
        bool is_shared() const noexcept {
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
        sd.shareable().disjoin().read().push(k, v);
    }
    
    void pop() {
        sd.shareable().disjoin().read().pop();
    }
    
    void pop(K const &k) {
        sd.shareable().disjoin().read().pop(k);
    }
    
    void move_to_back(K const &k) {
        sd.shareable().disjoin().read().move_to_back(k);
    }
    
    std::pair<K const &, V &> front() {
        return sd.disjoin().unshareable().read().front();
    }
    
    std::pair<K const &, V &> back() {
        return sd.disjoin().unshareable().read().back();
    }
    
    std::pair<K const &, V const &> front() const {
        return sd.unshareable().read_const().front();
    }
    
    std::pair<K const &, V const &> back() const {
        return sd.unshareable().read_const().back();
    }
    
    std::pair<K const &, V &> first(K const &key) {
        return sd.disjoin().read().first(key);
    }
    
    std::pair<K const &, V &> last(K const &key) {
        return sd.disjoin().read().last(key);
    }
    
    std::pair<K const &, V const &> first(K const &key) const {
        return sd.unshareable().read_const().first(key);
    }
    
    std::pair<K const &, V const &> last(K const &key) const {
        return sd.unshareable().read_const().last(key);
    }
    
    size_t size() const noexcept {
        return sd.read_const().size();
    }
    
    bool empty() const noexcept {
        return sd.read_const().empty();
    }
    
    // NOEXCEPT: Disjoin can potentially throw?
    void clear() {
        sd.shareable().disjoin().read().clear();
    }
    
    size_t count(K const &k) const noexcept {
        return sd.read_const().count(k);
    }
    
    k_iterator k_begin() {
        return sd.read().k_begin();
    }
    
    k_iterator k_end() {
        return sd.read().k_end();
    }
    
    std::string to_string() const noexcept {
        /*((sd.is_shared())?"<  shared  > ":"<standalone> ") + */
        return sd.read_const().to_string();
    }
};

#endif // _KEYED_QUEUE_