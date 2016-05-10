#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define NAME_BASED_CONFIGURATION 1
#include "macro_config.h"


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    #define CONFIGURE__t0 SCALAR
    #define CONFIGURE_MEMBER__test_t__x SCALAR
    #define CONFIGURE_MEMBER__test_t__y ARRAY(SCALAR)
#else
    #define CONFIGURE__t0 PTR(SCALAR)
    #define CONFIGURE_MEMBER__test_t__x PTR(SCALAR)
    #define CONFIGURE_MEMBER__test_t__y PTR(ARRAY(SCALAR))
#endif


#if HAS_PTR_ITEM(ITEM_CONFIG(CONFIGURE_MEMBER__test_t__y)) // configuration constraint determined by the algorithm
    #error "The inner element configuration of member 'test_t::y' must be SCALAR as constrained by 'strcpy'."
#endif


typedef struct
{
    int id;
    DECLARE_MEMBER(test_t, bool, x);
    DECLARE_MEMBER_ARRAY(test_t, char, y, 8);
} test_t;

int main()
{
    DECLARE(test_t, t0);
    test_t t1; // the usage of initializers is forbidden here, for example: { 0, false, "test" }

    ALLOCATE(t0);
    ALLOCATE_MEMBER(test_t, VALUE_OF(t0), x);
    ALLOCATE_MEMBER(test_t, VALUE_OF(t0), y);

    MEMBER_OF(t0, id) = 0;
    VALUE_OF_MEMBER(test_t, VALUE_OF(t0), x) = true;
    strcpy(&VALUE_OF_MEMBER(test_t, VALUE_OF(t0), y), "test");
    printf("t0: { %d, %d, \"%s\" }\n",
           MEMBER_OF(t0, id),
           VALUE_OF_MEMBER(test_t, VALUE_OF(t0), x),
           VALUE_OF_MEMBER(test_t, VALUE_OF(t0), y));

    DEALLOCATE_MEMBER(test_t, VALUE_OF(t0), y);
    DEALLOCATE_MEMBER(test_t, VALUE_OF(t0), x);
    DEALLOCATE(t0);


    ALLOCATE_MEMBER(test_t, t1, x);
    ALLOCATE_MEMBER(test_t, t1, y);

    t1.id = 1;
    VALUE_OF_MEMBER(test_t, t1, x) = true;
    strcpy(&VALUE_OF_MEMBER(test_t, t1, y) , "test");
    printf("t1: { %d, %d, \"%s\" }\n",
           t1.id,
           VALUE_OF_MEMBER(test_t, t1, x),
           VALUE_OF_MEMBER(test_t, t1, y));

    DEALLOCATE_MEMBER(test_t, t1, x);
    DEALLOCATE_MEMBER(test_t, t1, y);

    return 0;
}
