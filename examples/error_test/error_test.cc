/**
 * Univeristy of Warsaw 2017
 *  Task: KEYED QUEUE - basic test case runner for checking errors
 *  Usage:
 *     Use comparable_error_thrower class as an object throwing errros.
 *     Errors can be set on construction or later using #set_hooks method (might require const_cast).
 *     Test cases are boolean functions returning true if case is passing.
 *     All errors thrown within a test case must be caught.
 *     Cases to run can be selected in an array just before main() function
 *     - just add implemented test cases there.
 *
 *  Authors:
 *     kk385830  @kowaalczyk-priv
 *     ps386038  @styczynski
 *
 */
#include "keyed_queue.h"
#include <vector>
#include <algorithm>

/*
 * TODO:
 * Uwagi z 2 ćwiczeń dot. programu do sprawdzenia:
 * Exception safety:
 * - push_back wielokrotnie prowadzi do niespójności, lepiej tworzyć nową listę i splice ze starą
 * - map rzuca wyjątki nie tylko przy dodawaniu ale też przy porównywaniu
 * - operator [] na mapie - najlepiej w ogóle nie używać w tym zadaniu (niespójny stan i rzuca wyjątki)
 *
 * Każda operacja która się nie uda ma nie unieważniać żadnych iteratorów:
 * - operacja push - !!!
 *
 * Dziedziczenie publiczne vs prywatne - domyślnie mamy dziedziczyć publicznie
 *
 * #move_to_back:
 * - ma nie kopiować elementów tylko je przesuwać
 */

const int COPY_FAIL = 2137;
const int MOVE_FAIL = 1488;
const int ASSIGN_FAIL = 112;
const int COMPARE_FAIL = 314;
const int DESTRUCT_FAIL = 404;

/**
 * Wrapper for integer,
 * that throws errors depending on booleans specified in its constructor.
 * Compare and assignment operators only affect underlying integer value,
 * not the error-throwing behaviour.
 * It does have strong exception safety: whenever exception is thrown,
 * underlying values are not modified.
 */
class comparable_error_thrower {
private:
    int val;
    bool throw_on_copy, throw_on_move, throw_on_assign, throw_on_compare, throw_on_destruct;

public:

    /**
     * Constructor for use in test cases. Allows to specify throwing behaviour in tests.
     */
    comparable_error_thrower(int val=0,
                             bool throw_on_copy=false,
                             bool throw_on_move=false,
                             bool throw_on_assign=false,
                             bool throw_on_compare=false,
                             bool throw_on_destruct=false) {
        this->val = val;
        this->throw_on_copy = throw_on_copy;
        this->throw_on_move = throw_on_move;
        this->throw_on_assign = throw_on_assign;
        this->throw_on_compare = throw_on_compare;
        this->throw_on_destruct = throw_on_destruct;
    }

    /**
     * Default constructor
     */
    comparable_error_thrower() {
        // TODO: Create a version of this class that throws error on_default_construct
        this->val = 0;
        this->throw_on_copy = false;
        this->throw_on_move = false;
        this->throw_on_assign = false;
        this->throw_on_compare = false;
        this->throw_on_destruct = false;
    }

    /**
     * Copy constructor
     * @param other
     * @throws COPY_FAIL
     */
    comparable_error_thrower(const comparable_error_thrower& other) {
        if(other.throw_on_copy) {
            throw COPY_FAIL;
        }
        this->val = other.val;
        this->throw_on_copy = other.throw_on_copy;
        this->throw_on_move = other.throw_on_move;
        this->throw_on_assign = other.throw_on_assign;
        this->throw_on_compare = other.throw_on_compare;
        this->throw_on_destruct = other.throw_on_destruct;
    }

    /**
     * Move constructor
     * @param other
     * @throws MOVE_FAIL
     */
    comparable_error_thrower(comparable_error_thrower&& other) {
        if(other.throw_on_move) {
            throw MOVE_FAIL;
        }
        this->val = other.val;
        this->throw_on_copy = other.throw_on_copy;
        this->throw_on_move = other.throw_on_move;
        this->throw_on_assign = other.throw_on_assign;
        this->throw_on_compare = other.throw_on_compare;
        this->throw_on_destruct = other.throw_on_destruct;
    }

    /**
     * Allows to modify throwing behaviour in tests.
     */
    void set_hooks(bool throw_on_copy=false,
                    bool throw_on_move=false,
                    bool throw_on_assign=false,
                    bool throw_on_compare=false,
                    bool throw_on_destruct=false) {
        this->throw_on_copy = throw_on_copy;
        this->throw_on_move = throw_on_move;
        this->throw_on_assign = throw_on_assign;
        this->throw_on_compare = throw_on_compare;
        this->throw_on_destruct = throw_on_destruct; // TODO: Not testing this right now
    }

    /**
     * @throws ASSIGN_FAIL
     */
    comparable_error_thrower& operator= (comparable_error_thrower& other) {
        if(throw_on_assign) {
            throw ASSIGN_FAIL;
        }
        std::swap(this->val, other.val); // TODO: Make sure this is OK
        return *this;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator< (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val < other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator> (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val > other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator<= (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val <= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator>= (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val >= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator== (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val == other.val;
    }
};

// HELPER FUNCTIONS for test runner and test cases

void reportCaseFail(const std::string& case_name,
                    const std::string& expected_behaviour="",
                    const std::string& real_behaviour="") {
    std::cout << "Test case failed: " << case_name << std::endl;
    std::cout << "-- Expected behaviour: " << std::endl;
    std::cout << "-- " << expected_behaviour << std::endl;
    std::cout << "-- Real behaviour: " << std::endl;
    std::cout << "-- " << real_behaviour << std::endl;
    std::cout << std::endl;
}

const int ASSERTION_FAIL = 500;

void custom_assert(bool val, std::string fail_comment) {
    if(!val) {
        std::cout << "! ASSERTION FAIL: " << fail_comment << std::endl; // TODO: Not sure this is the best place for error logging
        throw ASSERTION_FAIL;
    }
}


// TEST CASES - Define Your test cases here

// default constructor

auto default_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;
    } catch (...) {
        reportCaseFail("default_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

// copy constructor

auto copy_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        auto kq1_copy1(kq1);
        custom_assert(kq1_copy1.empty(), "Copied queue contains random elements");
        auto kq2_copy1(kq2);
        custom_assert(kq2_copy1.empty(), "Copied queue contains random elements");

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        auto kq1_copy2(kq1);
        custom_assert(kq1_copy2.front().first == kq1.front().first &&
                      kq1_copy2.front().second == kq1.front().second &&
                      kq1_copy2.back().first == kq1.back().first &&
                      kq1_copy2.back().second == kq1.back().second,
                      "Copied queue contains wrong elements");
        auto kq2_copy2(kq2);
        custom_assert(kq2_copy2.front().first == kq2.front().first &&
                      kq2_copy2.front().second == kq2.front().second &&
                      kq2_copy2.back().first == kq2.back().first &&
                      kq2_copy2.back().second == kq2.back().second,
                      "Copied queue contains wrong elements");
    } catch (...) {
        reportCaseFail("copy_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto copy_constructor_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_val_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// move constructor

auto move_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<int, int> kq2;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq3;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq4;

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq3.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq3.push(comparable_error_thrower(3), comparable_error_thrower(4));

        std::swap(kq1, kq2);
        custom_assert(kq2.front().first == 1 &&
                      kq2.front().second == 2 &&
                      kq2.back().first == 3 &&
                      kq2.back().second == 4 &&
                      kq1.empty(),
                      "Swapped queue contains wrong elements");

        std::swap(kq3, kq4);
        custom_assert(kq4.front().first == comparable_error_thrower(1) &&
                      kq4.front().second == comparable_error_thrower(2) &&
                      kq4.back().first == comparable_error_thrower(3) &&
                      kq4.back().second == comparable_error_thrower(4) &&
                      kq3.empty(),
                      "Swapped queue contains wrong elements");
    } catch (...) {
        reportCaseFail("move_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto move_constructor_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// assignment operator

auto assign_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        auto kq1_copy1 = kq1;
        custom_assert(kq1_copy1.empty(), "Copied queue contains random elements");
        auto kq2_copy1 = kq2;
        custom_assert(kq2_copy1.empty(), "Copied queue contains random elements");

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        auto kq1_copy2 = kq1;
        custom_assert(kq1_copy2.front().first == kq1.front().first &&
                      kq1_copy2.front().second == kq1.front().second &&
                      kq1_copy2.back().first == kq1.back().first &&
                      kq1_copy2.back().second == kq1.back().second,
                      "Copied queue contains wrong elements");
        auto kq2_copy2 = kq2;
        custom_assert(kq2_copy2.front().first == kq2.front().first &&
                      kq2_copy2.front().second == kq2.front().second &&
                      kq2_copy2.back().first == kq2.back().first &&
                      kq2_copy2.back().second == kq2.back().second,
                      "Copied queue contains wrong elements");
    } catch (...) {
        reportCaseFail("assign_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto assign_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// push

auto push_working = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.front().first == 1, "Keys do not match");
        custom_assert(kq.front().second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("push_working",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("push_working",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto push_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, true, false, false, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, true, false, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, true, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, false, true, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, false, false, false), comparable_error_thrower(37, true, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// pop

auto pop_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        // this test obviously assumes #push is working :c
        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        kq1.pop();
        kq1.pop();

        kq2.pop();
        kq2.pop();

        custom_assert(kq1.empty() && kq2.empty(),
                      "Swapped queue contains wrong elements");
    } catch (...) {
        reportCaseFail("pop_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto pop_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// pop with key

auto pop_w_key_working = []{
    // TODO
    reportCaseFail("pop_w_key_working",
                   "",
                   "Unimplemented");
    return false;
};

auto pop_w_key_key_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("pop_w_key_key_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto pop_w_key_key_move_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("pop_w_key_key_move_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto pop_w_key_key_assign_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("pop_w_key_key_assign_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto pop_w_key_key_compare_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("pop_w_key_key_compare_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto pop_w_key_val_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("pop_w_key_val_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

// move to back

auto move_to_back_working = []{
    // TODO
    reportCaseFail("move_to_back_working",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_move_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_move_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_assign_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_assign_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_compare_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_compare_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_val_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_val_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

// front and back

auto front_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("front_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto front_throws_err_on_empty_list = []{
    // TODO
    reportCaseFail("front_throws_err_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto end_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("end_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto end_throws_err_on_empty_list = []{
    // TODO
    reportCaseFail("end_throws_err_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

// first and last

auto first_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("first_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto first_throws_err_on_empty_list = []{
    // TODO
    reportCaseFail("first_throws_err_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto last_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("last_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto last_throws_err_on_empty_list = []{
    // TODO
    reportCaseFail("last_throws_err_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

// size and empty

auto size_working = []{
    // TODO
    reportCaseFail("size_working",
                   "",
                   "Unimplemented");
    return false;
};

auto empty_working = []{
    // TODO
    reportCaseFail("empty_working",
                   "",
                   "Unimplemented");
    return false;
};

// clear

auto clear_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("clear_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto clear_working_on_empty_list = []{
    // TODO
    reportCaseFail("clear_working_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

// count

auto count_working_on_non_empty_list = []{
    // TODO
    reportCaseFail("count_working_on_non_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto count_working_on_empty_list = []{
    // TODO
    reportCaseFail("count_working_on_empty_list",
                   "",
                   "Unimplemented");
    return false;
};

auto count_fails_safely_when_key_comparison_fails = []{
    // TODO
    reportCaseFail("count_fails_safely_when_key_comparison_fails",
                   "",
                   "Unimplemented");
    return false;
};


// TODO: Make tests for iterator

// TEST RUNNER ----------------------------------------------------------------------------------------

// TODO: Uncomment implemented cases
bool (*cases_to_run[])(void) = {
        default_constructor_working,
        copy_constructor_working,
        copy_constructor_key_copy_fail_does_not_modify_queue,
        copy_constructor_key_move_fail_does_not_modify_queue,
        copy_constructor_key_assign_fail_does_not_modify_queue,
        copy_constructor_key_compare_fail_does_not_modify_queue,
        copy_constructor_val_copy_fail_does_not_modify_queue,
        move_constructor_working,
        move_constructor_key_copy_fail_does_not_modify_queue,
        move_constructor_key_move_fail_does_not_modify_queue,
        move_constructor_key_assign_fail_does_not_modify_queue,
        move_constructor_key_compare_fail_does_not_modify_queue,
        move_constructor_val_copy_fail_does_not_modify_queue,
        assign_working,
        assign_key_copy_fail_does_not_modify_queue,
        assign_key_move_fail_does_not_modify_queue,
        assign_key_assign_fail_does_not_modify_queue,
        assign_key_compare_fail_does_not_modify_queue,
        assign_val_copy_fail_does_not_modify_queue,
        push_working,
        push_key_copy_fail_does_not_modify_queue,
        push_key_move_fail_does_not_modify_queue,
        push_key_assign_fail_does_not_modify_queue,
        push_key_compare_fail_does_not_modify_queue,
        push_val_copy_fail_does_not_modify_queue,
        pop_working,
        pop_key_copy_fail_does_not_modify_queue,
        pop_key_move_fail_does_not_modify_queue,
        pop_key_assign_fail_does_not_modify_queue,
        pop_key_compare_fail_does_not_modify_queue,
        pop_val_copy_fail_does_not_modify_queue,
//        pop_w_key_working,
//        pop_w_key_key_copy_fail_does_not_modify_queue,
//        pop_w_key_key_move_fail_does_not_modify_queue,
//        pop_w_key_key_assign_fail_does_not_modify_queue,
//        pop_w_key_key_compare_fail_does_not_modify_queue,
//        pop_w_key_val_copy_fail_does_not_modify_queue,
//        move_to_back_working,
//        move_to_back_key_copy_fail_does_not_modify_queue,
//        move_to_back_key_move_fail_does_not_modify_queue,
//        move_to_back_key_assign_fail_does_not_modify_queue,
//        move_to_back_key_compare_fail_does_not_modify_queue,
//        move_to_back_val_copy_fail_does_not_modify_queue,
//        front_working_on_non_empty_list,
//        front_throws_err_on_empty_list,
//        end_working_on_non_empty_list,
//        end_throws_err_on_empty_list,
//        first_working_on_non_empty_list,
//        first_throws_err_on_empty_list,
//        last_working_on_non_empty_list,
//        last_throws_err_on_empty_list,
//        size_working,
//        empty_working,
//        clear_working_on_non_empty_list,
//        clear_working_on_empty_list,
//        count_working_on_non_empty_list,
//        count_working_on_empty_list,
//        count_fails_safely_when_key_comparison_fails
};

int main() {
    int failed_cases = 0;
    int passed_cases = 0;
    for(auto *test_case : cases_to_run) {
        if(test_case()) {
            passed_cases++;
        } else {
            failed_cases++;
        }
    }

    std::cout << std::endl;
    std::cout << "Test run finished, #passed:" << passed_cases << " #failed:" << failed_cases << std::endl;

    return 0;
}
