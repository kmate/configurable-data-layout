#include <boost/preprocessor.hpp>

#define NAME_BASED_CONFIGURATION 1
// Prevent including malloc.h
#define ALLOCATE_FUNCTION malloc
#define DEALLOCATE_FUNCTION free
#include "macro_config.h"


// Test configurations
#define CONFIGURE__a PTR(SCALAR)
#define CONFIGURE__b PTR(ARRAY(PTR(SCALAR)))

#define CONFIGURE_MEMBER__test_t__x PTR(SCALAR)
#define CONFIGURE_MEMBER__test_t__y PTR(ARRAY(SCALAR))

#define CONFIGURE_RETURN__f PTR(SCALAR)
#define CONFIGURE_PARAM__f__u SCALAR
#define CONFIGURE_PARAM__f__v PTR(ARRAY(PTR(SCALAR)))


// Configurable function return value and arguments
DECLARE_FUNCTION(int, f, (ARGUMENT(int, f, u), ARRAY_ARGUMENT(char, f, v, 2), char param_w));


// Struct member declaration
typedef struct
{
    DECLARE_MEMBER(test_t, int, x);
    // DECLARE_MEMBER_ARRAY(test_t, char, y);
    DECLARE_MEMBER_ARRAY(test_t, char, y, 8);
    // DECLARE_MEMBER_ARRAY(test_t, char, y, 8, 16);
} test_t;


void test_named_config()
{
// Declaration
    DECLARE(test_t, a);
    // DECLARE_ARRAY(char, b);
    DECLARE_ARRAY(char, b, 16);
    // DECLARE_ARRAY(char, b, 16, 32);


// Allocation
    ALLOCATE(a);
    ALLOCATE(b);


// Value of
    VALUE_OF(a);
    VALUE_OF(b);


// Value at a specified array index
    // VALUE_AT(b);
    VALUE_AT(b, 0);
    // VALUE_AT(b, 0, 0);


// Struct member initialization
    ALLOCATE_MEMBER(test_t, VALUE_OF(a), x);
    ALLOCATE_MEMBER(test_t, VALUE_OF(a), y);


// Value of a struct member
    VALUE_OF_MEMBER(test_t, VALUE_OF(a), x);
    VALUE_OF_MEMBER(test_t, VALUE_OF(a), y);


// Value at a specified array index of a struct member
    // VALUE_AT_MEMBER(test_t, a, y);
    VALUE_AT_MEMBER(test_t, VALUE_OF(a), y, 0);
    // VALUE_AT_MEMBER(test_t, a, y, 0, 0);


// Struct member destruction
    DEALLOCATE_MEMBER(test_t, VALUE_OF(a), x);
    DEALLOCATE_MEMBER(test_t, VALUE_OF(a), y);


// Deallocation
    DEALLOCATE(a);
    DEALLOCATE(b);
}
