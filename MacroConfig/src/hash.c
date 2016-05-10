#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NAME_BASED_CONFIGURATION 1
#include "macro_config.h"


// #define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    #define CONFIGURE__input SCALAR
    #define CONFIGURE__table ARRAY(ARRAY(SCALAR))
#else
    #define CONFIGURE__input PTR(SCALAR)
    #define CONFIGURE__table ARRAY(PTR(ARRAY(SCALAR)))
#endif


#if HAS_PTR_ITEM(ITEM_CONFIG(CONFIGURE__table)) // configuration constraint determined by the algorithm
    #error "The inner element configuration of 'table' must be SCALAR as constrained by 'strcpy'."
#endif


struct input_t
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
};

DECLARE(struct input_t, input);

DECLARE_ARRAY(char, table, 8, 32);

void init()
{
    ALLOCATE(input);
    ALLOCATE(table);
    strcpy(VALUE_AT(table, 0), "Zero");
    strcpy(VALUE_AT(table, 1), "One");
    strcpy(VALUE_AT(table, 2), "Two");
    strcpy(VALUE_AT(table, 3), "Three");
    strcpy(VALUE_AT(table, 4), "Four");
    strcpy(VALUE_AT(table, 5), "Five");
    strcpy(VALUE_AT(table, 6), "Six");
    strcpy(VALUE_AT(table, 7), "Seven");
}

void lookup()
{
    uint32_t key = (MEMBER_OF(input, a) + MEMBER_OF(input, b) + MEMBER_OF(input, c) + MEMBER_OF(input, d)) % 8;
    printf("%s\n", VALUE_AT(table, key));
}

int main()
{
    init();
    scanf("%d %d %d %d", &MEMBER_OF(input, a), &MEMBER_OF(input, b), &MEMBER_OF(input, c), &MEMBER_OF(input, d));
    lookup();

    DEALLOCATE(input);
    DEALLOCATE(table);

    return 0;
}
