#include <stdio.h>
#include "macro_config.h"


#define ACFG ARRAY(PTR(SCALAR))


#define USE_CORRECT_IMPL // implementation switch

void alloc_wrong(ARRAY_ARGUMENT(int, a, ACFG, 32))
{
    ALLOCATE(a, ACFG);
}

void dealloc_wrong(ARRAY_ARGUMENT(int, a, ACFG, 32))
{
    DEALLOCATE(a, ACFG);
}


void alloc_correct(ARRAY_ARGUMENT(int, a, PTR(ACFG), 32))
{
    ALLOCATE(*a, ACFG);
}

void dealloc_correct(ARRAY_ARGUMENT(int, a, PTR(ACFG), 32))
{
    DEALLOCATE(*a, ACFG);
}


int main()
{
    DECLARE_ARRAY(int, a, ACFG, 32);

#ifdef USE_CORRECT_IMPL
    alloc_correct(&a);
#else
    alloc_wrong(a);
#endif

    for(int i = 0; i < 32; ++i)
    {
        VALUE_AT(a, ACFG, i) = i;
    }

    for(int i = 0; i < 32; ++i)
    {
        printf("Value at %d: %d\n", i, VALUE_AT(a, ACFG, i));
    }

#ifdef USE_CORRECT_IMPL
    dealloc_correct(&a);
#else
    dealloc_wrong(a);
#endif

    return 0;
}
