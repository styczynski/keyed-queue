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
class lookup_error : public std::exception {
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

    class cow_writer {
    private:
        cow_data<T>& ref;
        std::shared_ptr<T> buffer;
        
    public:
        
        cow_writer(cow_data<T>& data): ref(data) {
            if(!is_shared()) {
                buffer = ref.shared_data;
            } else {
                buffer = std::make_shared<T>(T(*ref.shared_data));
            }
        }
        
        ~cow_writer() {
            
        }
        
        /**
         * Checks if any COW container is currently sharing the local data.
         *
         * @returns Is the held data shared by any other container?
         */
        bool is_shared() const noexcept {
            return ref.shared_data.use_count() > 1;
        }
        
        T& operator*() {
            return *buffer;
        }
        
        T* operator->() {
            return &(*buffer);
        }
        
        void commit() {
            *ref.shareableState = true;
            std::swap(ref.shared_data, buffer);
        }
    };
    
    class cow_reader {
    private:
        const cow_data<T>& ref;
        
    public:
        
        cow_reader(const cow_data<T>& data): ref(data) {
           
        }
        
        ~cow_reader() {
            
        }
        
        /**
         * Checks if any COW container is currently sharing the local data.
         *
         * @returns Is the held data shared by any other container?
         */
        bool is_shared() const noexcept {
            return ref.shared_data.use_count() > 1;
        }
        
        const T& operator*() const {
            return *ref.shared_data;
        }
        
        const T * operator->() const {
            return &(*ref.shared_data);
        }
    };
    
    class cow_persistent_reader {
    private:
        const cow_data<T>& ref;
        
    public:
        
        cow_persistent_reader(const cow_data<T>& data): ref(data) {
            *ref.shareableState = false;
        }
        
        ~cow_persistent_reader() {
            
        }
        
        /**
         * Checks if any COW container is currently sharing the local data.
         *
         * @returns Is the held data shared by any other container?
         */
        bool is_shared() const noexcept {
            return ref.shared_data.use_count() > 1;
        }
        
        const T& operator*() const {
            return *ref.shared_data;
        }
        
        const T * operator->() const {
            return &(*ref.shared_data);
        }
    };
    
    class cow_persistent_writer {
    private:
        cow_data<T>& ref;
        
    public:
        
        cow_persistent_writer(cow_data<T>& data): ref(data) {
            *ref.shareableState = false;
        }
        
        ~cow_persistent_writer() {
            
        }
        
        /**
         * Checks if any COW container is currently sharing the local data.
         *
         * @returns Is the held data shared by any other container?
         */
        bool is_shared() const noexcept {
            return ref.shared_data.use_count() > 1;
        }
        
        T& operator*() {
            return *ref.shared_data;
        }
        
        T * operator->() {
            return &(*ref.shared_data);
        }
    };

    /**
     * Creates empty container using default empty constructor of type T.
     * By default container is standalone and sharable.
     */
    cow_data(T value): shareableState(std::make_shared<bool>(true)), shared_data(std::make_shared<T>(T())) {
        shared_data = std::make_shared<T>(value);
    }
    
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
    
    cow_reader read() const {
        return cow_reader(*this);
    }
    
    cow_persistent_writer writePersistent() {
        return cow_persistent_writer(*this);
    }
    
    cow_persistent_reader readPersistent() const {
        return cow_persistent_reader(*this);
    }
    
    cow_writer write() {
        return cow_writer(*this);
    }
    
    const T& operator*() const {
        return *shared_data;
    }
    
    const T* operator->() const {
        return &(*shared_data);
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
    /** Const iterator to the mapping key -> list of key, value iterators */
    using kvi_map_ic = typename kvi_map::const_iterator;
    
public:

    /**
     * Keyed queue iterator.
     */
    class k_iterator {
    private:
        /** Helper iterator - iterating through keys mapping */
        kvi_map_ic map_iter;
        /** If the iterator was initialized? */
        bool assigned;
        
    public:
    
        /**
         * Create new interator from keys mapping iterator.
         */
        k_iterator(kvi_map_ic i) {
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
    public:
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
        
    };
    
    /** Copy-on-write data wrapper */
    cow_data<queue_data> sd;
    
public:

    /**
     * Creates empty keyed queue.
     */
    keyed_queue(): sd() {
         check();
    }
    
    /**
     * Creates keyed queue from another one.
     */
    keyed_queue(keyed_queue const& q): sd(q.sd) {
        check();
    }
    
    /**
     * Creates keyed queue from another one.
     */
    keyed_queue(keyed_queue&& q): sd(std::move(q.sd)) {
        check();
    }
    
    /**
     * Copies keyed queue from another one.
     */
    keyed_queue &operator=(keyed_queue other) {
        check();
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
        check();
        
        auto writer = sd.write();
        
        //TODO: This do not provide ANY exception guarantee!
        // Idk check this shit out plz
        
        // Buffers to store moved elements
        kv_list fifo_delta;
        kvi_list keys_delta;
        
        fifo_delta.push_back({k, v}); // < MAY FAIL
        
        // Make sure the keys mapping contains at least empty list for the new key
        const auto l = writer->keys.insert({ k, {} }); // < MAY FAIL
        
        // Obtain the iterator to the inserted element
        auto delta_i = fifo_delta.end();
        --delta_i;
        keys_delta.push_back(delta_i); // < MAY FAIL
        
        // Import buffer to the actual queue
        writer->fifo.splice(writer->fifo.end(), fifo_delta);
        
        
        // Insert the iterator in the mapping
        (l.first)->second.splice((l.first)->second.end(), keys_delta);
        
        writer.commit();
    }


    /**
     * Pop the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    void pop(K const &k) {
        check();
        
        auto writer = sd.write();
        
        // Try to find key in the mapping
        const auto iter = writer->keys.find(k);
        if(iter == writer->keys.end()) {
            throw lookup_error("pop(K): Key not present in the queue.");
        } else {
            
            // Buffers to store moved elements
            kv_list fifo_delta;
            kvi_list keys_delta;
            
            // If this condition is true then we failed to clean up keys mapping.
            // Keys mapping should contain no empty iterator list!
            assert(!(iter->second).empty());
            
            // Get the first element of the iterator list
            const auto e = (iter->second).front();
            
            // Move out element from the queue
            fifo_delta.splice(fifo_delta.begin(),  writer->fifo, e);
            
            // Move out the element iterator from the mapping
            keys_delta.splice(keys_delta.begin(), iter->second, (iter->second).begin());
            
            // Remove the elements
            fifo_delta.clear(); // < MAY FAIL
            keys_delta.clear(); // < MAY FAIL
            
            // Erase the keys mapping list if it's empty
            if((iter->second).size() <= 0) {
                writer->keys.erase(iter); // < MAY FAIL
            }
        }
        
        writer.commit();
    }

     /**
     * Pop the first element from the queue with matching key.
     *
     * @param[in] k : key
     * @throws lookup_error when there's no element with given key
     */
    void pop() {
        check();
        
        auto writer = sd.write();
        
        if(writer->fifo.empty()) {
            throw lookup_error("pop(): Queue is empty.");
        }
        
        // Buffers to store moved elements
        kv_list fifo_delta;
        kvi_list keys_delta;
        
        // Get the iterator to the first queue element
        const auto iter = writer->fifo.begin();
        
        // Get the key of that element
        const auto key = iter->first;
        
        // Find the list in the keys mapping with the given key
        const auto i = writer->keys.find(key); // < MAY FAIL
        
        // The key must be present in the mapping
        assert(i != writer->keys.end());
        
        // Obtain the list of iterators with matching key
        auto& list = i->second;
        
        // If the first element of the list in the mapping
        // is not equal to the iterator of the first element of the queue
        // then something really bad happened and entire data is broken!
        assert(list.front() == iter);
        
        // Pop the element from the mapping
        keys_delta.splice(keys_delta.begin(), list, list.begin());
        
        // Pop the element from the queue
        fifo_delta.splice(fifo_delta.begin(), writer->fifo, writer->fifo.begin());
        
        // Remove the elements
        fifo_delta.clear(); // < MAY FAIL
        keys_delta.clear(); // < MAY FAIL
        
        // Remove list from mapping if it's empty
        if(list.size() <= 0) {
            writer->keys.erase(i); // < MAY FAIL
        }
        
        writer.commit();
    }

    /**
     * Move all elements with matching key to the end of the queue.
     * Operation preserves order of the key, value pairs.
     */
    void move_to_back(K const &k) {
        check();
        auto writer = sd.write();
        
        const auto i = writer->keys.find(k);
        if(i == writer->keys.end()) {
            throw lookup_error("move_to_back(K): There's no such key in the queue.");
        }
        
        // Find the list of iterators with matching keys
        auto& list = i->second;
        // For all iterators
        for(auto e=list.begin();e!=list.end();++e) {
            // Move element to the back of the queue (do not perform copy)!
            writer->fifo.splice( writer->fifo.end(), writer->fifo, *e );
        }
        
        writer.commit();
    }

    /**
     * Get the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V &> front() {
        check();
        auto reader = sd.writePersistent();
        
        if(reader->fifo.empty()) {
            throw lookup_error("front(): Queue is empty.");
        }
        auto& ref = reader->fifo.front();
        return {std::get<0>(ref), std::get<1>(ref)};
    }

    /**
     * Get the last element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V &> back() {
        check();
        auto reader = sd.writePersistent();
        
        if(reader->fifo.empty()) {
            throw lookup_error("back(): Queue is empty.");
        }
        auto& ref = reader->fifo.back();
        return { std::get<0>(ref), std::get<1>(ref) };
    }

    /**
     * Get the first element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V const &> front() const {
        check();
        auto reader = sd.readPersistent();
        
        if(reader->fifo.empty()) {
            throw lookup_error("front(): Queue is empty.");
        }
        auto& ref = reader->fifo.front();
        return { std::get<0>(ref), std::get<1>(ref) };
    }

    /**
     * Get the last element from the queue.
     *
     * @throws lookup_error when the queue is empty
     */
    std::pair<K const &, V const &> back() const {
        check();
        auto reader = sd.readPersistent();
        
        if(reader->fifo.empty()) {
            throw lookup_error("back(): Queue is empty.");
        }
        auto& ref = reader->fifo.back();
        return { std::get<0>(ref), std::get<1>(ref) };
    }

   /**
     * Get the first element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V &> first(K const &key) {
        check();
        auto reader = sd.writePersistent();
        
        const auto keyloc = reader->keys.find(key);
        if(keyloc == reader->keys.end()) {
            throw lookup_error("first(K): Key not present in the queue.");
        }
        return { key, (keyloc->second.front())->second };
    }

    /**
     * Get the last element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V &> last(K const &key) {
        check();
        auto reader = sd.writePersistent();
        
        const auto keyloc = reader->keys.find(key);
        if(keyloc == reader->keys.end()) {
            throw lookup_error("last(K): Key not present in the queue.");
        }
        return { key, (keyloc->second.back())->second };
    }

    /**
     * Get the first element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V const &> first(K const &key) const {
        check();
        auto reader = sd.readPersistent();
        
        const auto keyloc = reader->keys.find(key);
        if(keyloc == reader->keys.end()) {
            throw lookup_error("first(K): Key not present in the queue.");
        }
        return { key, (keyloc->second.front())->second };
    }

    /**
     * Get the last element from the queue with matching key.
     *
     * @throws lookup_error when the queue does not contain element with given key
     */
    std::pair<K const &, V const &> last(K const &key) const {
        check();
        auto reader = sd.readPersistent();
        
        const auto keyloc = reader->keys.find(key);
        if(keyloc == reader->keys.end()) {
            throw lookup_error("last(K): Key not present in the queue.");
        }
        return { key, (keyloc->second.back())->second };
    }

    /**
     * Gets the number of elements of the queue.
     *
     * @returns nonzero size of the queue
     * @throws never
     */
    size_t size() const noexcept {
        check();
        auto reader = sd.read();
        
        return reader->fifo.size();
    }

    /**
     * Checks if the queue is empty.
     * This can be implemented via (size() == 0)
     *
     * @returns If the queue is empty?
     * @throws never
     */
    bool empty() const noexcept {
        check();
        auto reader = sd.read();
        
        return reader->fifo.empty();
    }

    /**
     * Clears the queue contents.
     */
    void clear() noexcept {
        check();
        auto writer = sd.write();
        
        writer->fifo.clear(); // < MAY FAIL
        writer->keys.clear(); // < MAY FAIL
        
        writer.commit();
    }

    /**
     * Gets the number of elements with the given key.
     *
     * @returns nonzero count of elements with matching key
     * @throws never
     */
    size_t count(K const &k) const {
        check();
        auto reader = sd.read();
        
        const auto i = reader->keys.find(k); // < MAY FAIL
        if(i != reader->keys.end()) {
            return i->second.size();
        }
        return 0;
    }

    /**
     * Obtains the iterator to the queue first element.
     *
     * @returns keyed queue iterator
     */
    k_iterator k_begin() {
        check();
        auto reader = sd.read();
        
        return k_iterator(reader->keys.begin());
    }

    /**
     * Obtains the past-the-end iterator of the queue.
     *
     * @returns keyed queue iterator
     */
    k_iterator k_end() {
        check();
        auto reader = sd.read();
        
        return k_iterator(reader->keys.end());
    }

    /**
     * Returns std::string human readable representation of the keyed queue.
     *
     * @returns human readable representation
     */
    std::string to_string() const {
        auto reader = sd.read();
        
        std::ostringstream out;
        
        out << "{ ";
        bool isFirst = true;
        for(auto e : reader->fifo) {
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
        auto reader = sd.read();
        
        //std::cout << " ()\n";
        // Iterate mapping
        int entry_count = 0;
        for(auto keys_e : reader->keys) {
            // Key from the mapping
            const auto key = keys_e.first;
            // List of iterators for the given key
            const auto list = keys_e.second;
            int list_index = 0;
            entry_count += list.size();
            for(auto key_i : list) {
                // Count keys from the original queue
                int count = 0;
                auto el = reader->fifo.end();
                // Brutal fifo search 
                for(auto e=reader->fifo.begin();e!=reader->fifo.end();++e) {
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
        assert(entry_count == (signed)(reader->fifo.size()));
        
    }

    /**
     * Perform integrity check only if ENABLE_INTEGRITY_CHECKS is enabled.
     */
    void check() const {
        if(ENABLE_INTEGRITY_CHECKS) check_integrity();
    }

    /**
     * Perform integrity check only if ENABLE_INTEGRITY_CHECKS is enabled.
     */
    void check() {
        if(ENABLE_INTEGRITY_CHECKS) check_integrity();
    }
};

#endif // _KEYED_QUEUE_