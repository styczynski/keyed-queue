/**
 * Univeristy of Warsaw 2017
 *  Task: KEYED QUEUE
 *  Authors:
 *     kk385830  @kowaalczyk-priv
 *     ps386038  @styczynski
 */
#include "keyed_queue.h"
#include <vector>

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
     * Constructor for use in test cases - only way to specify when errors are thrown.
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
        this->val = other.val;
        this->throw_on_copy = other.throw_on_copy;
        this->throw_on_move = other.throw_on_move;
        this->throw_on_assign = other.throw_on_assign;
        this->throw_on_compare = other.throw_on_compare;
        this->throw_on_destruct = other.throw_on_destruct;
        if(throw_on_copy) {
            throw COPY_FAIL;
        }
    }

    /**
     * Move constructor
     * @param other
     * @throws MOVE_FAIL
     */
    comparable_error_thrower(comparable_error_thrower&& other) {
        if(throw_on_move) {
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
    bool operator< (const comparable_error_thrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val < other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator> (const comparable_error_thrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val > other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator<= (const comparable_error_thrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val <= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator>= (const comparable_error_thrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val >= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator== (const comparable_error_thrower& other) {
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
        std::cout << fail_comment << std::endl; // TODO: Not sure this is the best place for error logging
        throw ASSERTION_FAIL;
    }
}


// TEST CASES - Define Your test cases here
// All cases should be boolean functions without arguments, and with noexcept guarantee

auto push_working_without_errors = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.front().first == 1, "Keys do not match");
        custom_assert(kq.front().second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("push_working_without_errors",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("push_working_without_errors",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto copy_fail_does_not_modify_target_queue = []{
    // TODO
    reportCaseFail("copy_fail_does_not_modify_target_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_fail_does_not_modify_target_queue = []{
    // TODO
    reportCaseFail("move_fail_does_not_modify_target_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto assignment_fail_does_not_modify_target_queue = []{
    // TODO
    reportCaseFail("assignment_fail_does_not_modify_target_queue",
                   "",
                   "Unimplemented");
    return false;
};

// TEST RUNNER ----------------------------------------------------------------------------------------

// TODO: Add cases to run here:
bool (*cases_to_run[])(void) = {
        push_working_without_errors,
        copy_fail_does_not_modify_target_queue,
        move_fail_does_not_modify_target_queue,
        assignment_fail_does_not_modify_target_queue
};

int main() {

    int current_case_id = 0;
    int failed_cases = 0;
    for(auto *test_case : cases_to_run) {
        if(test_case()) {
            // do nothing
        } else {
            failed_cases++;
        }
        current_case_id++;
    }

    std::cout << std::endl;
    std::cout << "Test run finished, # of fails: " << failed_cases << std::endl;

    return 0;
}
