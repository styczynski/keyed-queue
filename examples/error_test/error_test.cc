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
class ComparableErrorThrower {
private:
    int val;
    bool throw_on_copy, throw_on_move, throw_on_assign, throw_on_compare, throw_on_destruct;

public:

    /**
     * Constructor for use in test cases - only way to specify when errors are thrown.
     */
    ComparableErrorThrower(int val=0,
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
    ComparableErrorThrower() {
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
    ComparableErrorThrower(const ComparableErrorThrower& other) {
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
    ComparableErrorThrower(ComparableErrorThrower&& other) {
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
    ComparableErrorThrower& operator= (ComparableErrorThrower& other) {
        if(throw_on_assign) {
            throw ASSIGN_FAIL;
        }
        std::swap(this->val, other.val); // TODO: Make sure this is OK
        return *this;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator< (const ComparableErrorThrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val < other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator> (const ComparableErrorThrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val > other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator<= (const ComparableErrorThrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val <= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator>= (const ComparableErrorThrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val >= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator== (const ComparableErrorThrower& other) {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val == other.val;
    }
};

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


// TEST CASES - Define Your test cases here
// All cases should be boolean functions without arguments, and with noexcept guarantee

bool copyFailDoesNotModifyTargetQueue() noexcept {
    // TODO
    reportCaseFail("Unimplemented");
    return false;
}

// TODO: Add test cases

// TEST RUNNER ----------------------------------------------------------------------------------------

// TODO: Add cases to run here:
bool (*cases_to_run[])(void) = {
        copyFailDoesNotModifyTargetQueue
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