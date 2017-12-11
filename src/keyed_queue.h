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

/**
 * Enable runtime integrity checks for keyed queue.
 * Note:
 *    Integrity checks should be used only for debugging purposes
 *    because they are of noticeable weight.
 */
constexpr bool ENABLE_INTEGRITY_CHECKS = false;

/**
 * Error thrown when accessing invalid/non-existing keys/values inside keyed_queue.
 */
class lookup_error : std::exception {
private:
    std::string what_message;
public:

    /**
     * Creates new lookup_error with given detailed description string.
     *
     * @param[in] additionalDescription : Additional debug info
     */
    lookup_error(std::string additionalDescription) {
        std::ostringstream out;
        out << "Keyed queue lookup error. ";
        out << additionalDescription;
        what_message = out.str();
    }

    /**
     * Return the human readable error description.
     *
     * @returns const char* description of the error
     */
    const char* what() {
        return what_message.c_str();
    }
};

/**
 * Copy-on-write data container
 *
 * @tparam Type of value that is held by container
 */
template <typename T>
class cow_data {
private:
    
    /** Is the underlying data sharable? */
    std::shared_ptr<bool> shareableState;
    
    /** The underlying data */
    std::shared_ptr<T> shared_data;
    
public:

    /**
     * Creates empty container using default empty constructor of type T.
     * By default container is standalone and sharable.
     */
    cow_data(): shareableState(std::make_shared<bool>(true)), shared_data(std::make_shared<T>(T())) {
    
    }
    
    /**
     * Creates container using another COW container contents.
     * Contents will be shared and sharable.
     */
    cow_data(cow_data<T> const& d): shareableState(std::make_shared<bool>(true)) {
        if(*d.shareableState) {
            shared_data = d.shared_data;
        } else {
            shared_data = std::make_shared<T>(*d.shared_data);
        }
    }
    
    /**
     * Creates container using another COW container contents.
     * Contents will be shared and sharable.
     */
    cow_data(cow_data<T>&& d): shareableState(std::make_shared<bool>(true)) {
        if(*d.shareableState) {
            shared_data = d.shared_data;
        } else {
            shared_data = std::make_shared<T>(*d.shared_data);
        }
    }
    
    /**
     * Creates container using another COW container contents.
     * Contents will be shared and sharable.
     *
     * @param[in] d : another COW data container
     * @returns reference to self
     */
    cow_data& operator= (cow_data<T> d) {
        if(*d.shareableState) {
            shared_data = d.shared_data;
        } else {
            shared_data = std::make_shared<T>(*d.shared_data);
        }
        return *this;
    }
    
    /**
     * If the object is shared then it becomes standalone object.
     * In that scenario method invokes copy construtor of type T and perform full copy.
     * If the object is standalone then nothing happens.
     * 
     * @returns reference to self
     */
    cow_data<T>& disjoin() {
        if(!is_shared()) return *this;
        shared_data = std::make_shared<T>(T(*shared_data));
        return *this;
    }
    
    /**
     * Access underlying data.
     *
     * @returns reference to held data
     * @throws never
     */
    T& read() noexcept {
        return *shared_data;
    }
    
    /**
     * Access unerlying data without modifying contents.
     * 
     * @returns const reference to held data
     * @throws never
     */
    const T& read_const() const noexcept {
        return *shared_data;
    }
    
    /**
     * Mark data as unshareable.
     * Unshareable data will accept no new forks.
     * Each COW container that tries to copy data from unshareable cotainer
     * will always perform full copy.
     *
     * @returns reference to self
     */
    const cow_data<T>& unshareable() const noexcept {
        *shareableState = false;
        return *this;
    }
    
    /**
     * Mark data as unshareable.
     * Unshareable data will accept no new forks.
     * Each COW container that tries to copy data from unshareable cotainer
     * will always perform full copy.
     *
     * @returns reference to self
     */
    cow_data<T>& unshareable() noexcept {
        *shareableState = false;
        return *this;
    }
    
    /**
     * Mark data as shareable.
     * Unshareable data will accept no new forks.
     * Each COW container that tries to copy data from unshareable cotainer
     * will always perform full copy.
     *
     * This method does the reverse of unshareable().
     *
     * @returns reference to self
     */
    const cow_data<T>& shareable() const noexcept {
        *shareableState = true;
        return *this;
    }
    
    /**
     * Mark data as shareable.
     * Unshareable data will accept no new forks.
     * Each COW container that tries to copy data from unshareable cotainer
     * will always perform full copy.
     *
     * This method does the reverse of unshareable().
     *
     * @returns reference to self
     */
    cow_data<T>& shareable() noexcept {
        *shareableState = true;
        return *this;
    }
    
    /**
     * Checks if any COW container is currently sharing the local data.
     *
     * @returns Is the held data shared by any other container?
     */
    bool is_shared() const noexcept {
        return shared_data.use_count() > 1;
    }
};

/**
 * Keyed queue is a FIFO structure that can held pairs of key, value.
 * It offers additional functionality compared to standard queues like
 * accessing sets of equal keys, reading by key etc.
 *
 * @tparam K : Key type
 * @tparam V : Value type
 */
template <class K, class V>
class keyed_queue {
private:

    /** Type of key, value pair */
    using kv_pair = std::pair<K, V>;
    /** List of key, value pairs */
    using kv_list = std::list<kv_pair>;
    /** Iterator to the list of key, value pair list*/
    using kv_list_i = typename kv_list::iterator;
    /** List of key, value iterators */
    using kvi_list = std::list<kv_list_i>;
    /** Iterator to the list of key, value iterator list*/
    using kvi_list_i = typename kvi_list::iterator;
    /** Mapping key -> list of key, value iterators */
    using kvi_map = std::map<K, kvi_list>;
    /** Iterator to the mapping key -> list of key, value iterators */
    using kvi_map_i = typename kvi_map::iterator;

    
public:

    /**
     * Keyed queue iterator.
     */
    class k_iterator {
    private:
        /** Helper iterator - iterating through keys mapping */
        kvi_map_i map_iter;
        /** If the iterator was initialized? */
        bool assigned;
        
    public:
    
        /**
         * Create new interator from keys mapping iterator.
         */
        k_iterator(const kvi_map_i i) {
            map_iter = i;
            assigned = true;
        }
    
        /**
         * Creates empty iterator.
         * @throws never
         */
        k_iterator() noexcept {
            assigned = false;
        }
        
        /**
         * Copy constructor.
         */
        k_iterator(k_iterator const& i) {
            assigned = i.assigned;
            map_iter = i.map_iter;
        }
        
        /**
         * Move to next position.
         * @throws never
         */
        void operator++() noexcept {
            if(assigned) ++map_iter;
        }
        
        /**
         * Compare iterators.
         * @throws never
         */
        bool operator==(k_iterator k) noexcept {
            if(!assigned || !k.assigned) return false;
            return map_iter == k.map_iter;
        }
        
        /**
         * Compare iterators.
         * @throws never
         */
        bool operator!=(k_iterator k) noexcept {
            if(!assigned || !k.assigned) return true;
            return map_iter != k.map_iter;
        }
        
        /**
         * Dereference iterator.
         */
        K operator*() {
            if(!assigned) throw "k_iterator: Dereferencing unassigned iterator.";
            return map_iter->first;
        }
    };
    
private:
    
    /**
     * Internal data of keyed_queue.
     */
    class queue_data {
    private:
        /** Keys mapping: key -> list of iterators pointing to this.fifo */
        kvi_map keys;
        /** List of key, value pairs */
        kv_list fifo;

    public:
      
        /**
         * Copy constructor.
         */
        queue_data(const queue_data& q) {
            //TODO: Validate
            // For all elements in the copied queue
            for(auto i=q.fifo.begin(); i!=q.fifo.end(); ++i) {
                // Push data to queue
                fifo.push_back(*i);
                // Get the iterator to the pushed element
                auto el_i = fifo.end();
                --el_i;
                // Ensure there is at least empty list for the new key
                const auto l = keys.insert({ i->first, {} });
                // Insert iterator to the keys mapping
                (l.first)->second.push_back(el_i);
            }
        }
        
        /**
         * Create empty data set.
         */
        queue_data(): keys(), fifo() {
            
        }
        
        /**
         * Move contructor.
         */
        queue_data(queue_data&& q): keys(std::move(q.keys)),
                                    fifo(std::move(q.fifo)) {
        
        }
        
        /**
         * Keyed queue operation implementation
         */
        void push(K const &k, V const &v) {
            
            //TODO: This do not provide ANY exception guarantee!
            // Idk check this shit out plz
            
            // Helper buffer
            kv_list fifo_delta;
            
            try {
                // Insert new pair to the helper buffer
                fifo_delta.push_back({k, v}); // < This may fail!
            } catch(...) {
                // TODO: This probably does nothing?
                //       The try-catch block should be removed
                // Rethrow error
                throw;
            }
            
            // Import buffer to the actual queue
            fifo.splice(fifo.end(), fifo_delta); // < Safe
            
            // Make sure the keys mapping contains at least empty list for the new key
            const auto l = keys.insert({ k, {} }); // < This may too? Maybe?
            // Obtain the iterator to the inserted element
            auto delta_i = fifo.end();
            --delta_i;
            // Insert the iterator in the mapping
            (l.first)->second.push_back(delta_i); // < Safe
            
        }
        
        /**
         * Keyed queue operation implementation
         */
        void pop(K const &k) {
            // Try to find key in the mapping
            const auto iter = keys.find(k);
            if(iter == keys.end()) {
                throw lookup_error("pop(K): Key not present in the queue.");
            } else {
                // TODO: Validate
                
                // If this condition is true then we failed to clean up keys mapping.
                // Keys mapping should contain no empty iterator list!
                assert(!(iter->second).empty());
                
                // Get the first element of the iterator list
                const auto e = (iter->second).front();
                
                // Erase that element from the queue
                fifo.erase(e); // < TODO: May fail
                
                // Pop the element from the keys mapping
                (iter->second).pop_front();
                
                // Erase the keys mapping list if it's empty
                if((iter->second).size() <= 0) {
                    keys.erase(iter);
                }
            }
        }
        
        /**
         * Keyed queue operation implementation
         */
        void pop() {
            if(fifo.empty()) {
                throw lookup_error("pop(): Queue is empty.");
            }
            
            // TODO: Validate
            
            // Get the iterator to the first queue element
            const auto iter = fifo.begin();
            
            // Get the key of that element
            const auto key = iter->first;
            
            // Find the list in the keys mapping with the given key
            const auto i = keys.find(key);
            
            // The key must be present in the mapping
            assert(i != keys.end());
            
            // Obtain the list of iterators with matching key
            auto& list = i->second;
            
            // If the first element of the list in the mapping
            // is not equal to the iterator of the first element of the queue
            // then something really bad happened and entire data is broken!
            assert(list.front() == iter);
            
            // Pop the element from the mapping
            list.pop_front();
            
            // Remove list from mapping if it's empty
            if(list.size() <= 0) {
                keys.erase(i);
            }
            
            // Finall remove the element from the queue 
            fifo.pop_front();
        }
        
        /**
         * Keyed queue operation implementation
         */
        void move_to_back(K const &k) {
            const auto i = keys.find(k);
            if(i == keys.end()) {
                throw lookup_error("move_to_back(K): There's no such key in the queue.");
            }
            
            // TODO: Validate
            
            // Find the list of iterators with matching keys
            auto& list = i->second;
            // For all iterators
            for(auto e=list.begin();e!=list.end();++e) {
                // Push element to the back of the queue
                fifo.push_back(**e);
                // Get the new iterator
                auto new_e = fifo.end();
                --new_e;
                // Remove element pointed by previous iterator value
                fifo.erase(*e);
                // Save new iterator value
                *e = new_e;
            }
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V &> front() {
            if(fifo.empty()) {
                throw lookup_error("front(): Queue is empty.");
            }
            auto& ref = fifo.front();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V &> back() {
            if(fifo.empty()) {
                throw lookup_error("back(): Queue is empty.");
            }
            auto& ref = fifo.back();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V const &> front() const {
            if(fifo.empty()) {
                throw lookup_error("front(): Queue is empty.");
            }
            auto& ref = fifo.front();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V const &> back() const {
            if(fifo.empty()) {
                throw lookup_error("back(): Queue is empty.");
            }
            auto& ref = fifo.back();
            return { std::get<0>(ref), std::get<1>(ref) };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V &> first(K const &key) {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("first(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.front())->second };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V &> last(K const &key) {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("last(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.back())->second };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V const &> first(K const &key) const {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("first(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.front())->second };
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::pair<K const &, V const &> last(K const &key) const {
            const auto keyloc = keys.find(key);
            if(keyloc == keys.end()) {
                throw lookup_error("last(K): Key not present in the queue.");
            }
            return { key, (keyloc->second.back())->second };
        }
        
        /**
         * Keyed queue operation implementation
         */
        size_t size() const noexcept {
            return fifo.size();
        }
        
        /**
         * Keyed queue operation implementation
         */
        bool empty() const noexcept {
            return fifo.empty();
        }
        
        /**
         * Keyed queue operation implementation
         */
        void clear() noexcept {
            fifo.clear();
            keys.clear();
        }
        
        /**
         * Keyed queue operation implementation
         */
        size_t count(K const &k) const noexcept {
            const auto i = keys.find(k);
            if(i != keys.end()) {
                return i->second.size();
            }
            return 0;
        }
        
        /**
         * Keyed queue operation implementation
         */
        k_iterator k_begin() {
            return k_iterator(keys.begin());
        }
        
        /**
         * Keyed queue operation implementation
         */
        k_iterator k_end() {
            return k_iterator(keys.end());
        }
        
        /**
         * Keyed queue operation implementation
         */
        std::string to_string() const {
            std::ostringstream out;
            
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
        
        /**
         * Perform forced integrity check.
         * The keys mapping and fifo structure is validated.
         * Note that integrity checks are expensive.
         */
        void check_integrity() const {
            //std::cout << " ()\n";
            // Iterate mapping
            int entry_count = 0;
            for(auto keys_e : keys) {
                // Key from the mapping
                const auto key = keys_e.first;
                // List of iterators for the given key
                const auto list = keys_e.second;
                int list_index = 0;
                entry_count += list.size();
                for(auto key_i : list) {
                    // Count keys from the original queue
                    int count = 0;
                    auto el = fifo.end();
                    // Brutal fifo search 
                    for(auto e=fifo.begin();e!=fifo.end();++e) {
                        if(e->first == key) {
                            el = e;
                            if(count == list_index) break;
                            ++count;
                        }
                    }
                    // el is now the iterator to the n-th element with the given key
                    // where n is equal to list_index
                    
                    // Iterator in the mapping must be equal to the one
                    // Determined by brutal fifo search
                    assert(el == key_i);
                    
                    ++list_index;
                }
                
            }
            
            // Eevery fifo element must be placed inside mapping
            assert(entry_count == (signed)fifo.size());
            
        }
        
        /**
         * Perform integrity check only if ENABLE_INTEGRITY_CHECKS is enabled.
         */
        const queue_data& check() const {
            if(ENABLE_INTEGRITY_CHECKS) check_integrity();
            return *this;
        }
        
        /**
         * Perform integrity check only if ENABLE_INTEGRITY_CHECKS is enabled.
         */
        queue_data& check() {
            if(ENABLE_INTEGRITY_CHECKS) check_integrity();
            return *this;
        }
    };
    
    /** Copy-on-write data wrapper */
    cow_data<queue_data> sd;
    
public:

    /**
     * Creates empty keyed queue.
     */
    keyed_queue(): sd() {
         
    }
    
    /**
     * Creates keyed queue from another one.
     */
    keyed_queue(keyed_queue const& q): sd(q.sd) {
        
    }
    
    /**
     * Creates keyed queue from another one.
     */
    keyed_queue(keyed_queue&& q): sd(std::move(q.sd)) {
        
    }
    
    /**
     * Copies keyed queue from another one.
     */
    keyed_queue &operator=(keyed_queue other) {
        sd = other.sd;
        return *this;
    }
    
    /**
     * Push new key, value pair to the front of the queue.
     *
     * @param[in] k : key
     * @param[in] v : value
     */
    void push(K const &k, V const &v) {
        sd.shareable().disjoin().read().check().push(k, v);
    }
    
    /**
     * Pop the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    void pop() {
        sd.shareable().disjoin().read().check().pop();
    }
    
    /**
     * Pop the first element from the queue with matching key.
     *
     * @param[in] k : key
     * @throws lookup_error when there's no element with given key
     */
    void pop(K const &k) {
        sd.shareable().disjoin().read().check().pop(k);
    }
    
    /**
     * Move all elements with matching key to the end of the queue.
     * Operation preserves order of the key, value pairs.
     */
    void move_to_back(K const &k) {
        sd.shareable().disjoin().read().check().move_to_back(k);
    }
    
    /**
     * Get the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V &> front() {
        return sd.disjoin().unshareable().read().check().front();
    }
    
    /**
     * Get the last element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V &> back() {
        return sd.disjoin().unshareable().read().check().back();
    }
    
    /**
     * Get the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V const &> front() const {
        return sd.unshareable().read_const().check().front();
    }
    
    /**
     * Get the last element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V const &> back() const {
        return sd.unshareable().read_const().check().back();
    }
    
    /**
     * Get the first element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V &> first(K const &key) {
        return sd.disjoin().read().check().first(key);
    }
    
    /**
     * Get the last element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V &> last(K const &key) {
        return sd.disjoin().read().check().last(key);
    }
    
    /**
     * Get the first element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V const &> first(K const &key) const {
        return sd.unshareable().read_const().check().first(key);
    }
    
    /**
     * Get the last element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V const &> last(K const &key) const {
        return sd.unshareable().read_const().check().last(key);
    }
    
    /**
     * Gets the number of elements of the queue.
     *
     * @returns nonzero size of the queue
     * @throws never
     */
    size_t size() const noexcept {
        return sd.read_const().check().size();
    }
    
    /**
     * Checks if the queue is empty.
     * This can be implemented via (size() == 0)
     *
     * @returns If the queue is empty?
     * @throws never
     */
    bool empty() const noexcept {
        return sd.read_const().check().empty();
    }
    
    // TODO: Disjoin can potentially throw?
    // If not mark that as noexcept
    /**
     * Clears the queue contents.
     */
    void clear() {
        sd.shareable().disjoin().read().check().clear();
    }
    
    /**
     * Gets the number of elements with the given key.
     *
     * @returns nonzero count of elements with matching key
     * @throws never
     */
    size_t count(K const &k) const noexcept {
        return sd.read_const().check().count(k);
    }
    
    /**
     * Obtains the iterator to the queue first element.
     *
     * @returns keyed queue iterator
     */
    k_iterator k_begin() {
        return sd.read().check().k_begin();
    }
    
    /**
     * Obtains the past-the-end iterator of the queue.
     *
     * @returns keyed queue iterator
     */
    k_iterator k_end() {
        return sd.read().check().k_end();
    }
    
    /**
     * Returns std::string human readable representation of the keyed queue.
     *
     * @returns human readable representation
     */
    std::string to_string() const {
        return sd.read_const().check().to_string();
    }
    
    /**
     * Request data structure to perform forced integrity check.
     * Integrity checks validate internal structure of data.
     * Note that they are taking huge amounts of time!
     */
    void check() { 
        sd.read().integrity_check();
    }
};

#endif // _KEYED_QUEUE_