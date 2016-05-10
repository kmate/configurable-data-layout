#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "macro_config.h"


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    #define CFG_t0 SCALAR
    #define CFG_tx SCALAR
    #define CFG_ty ARRAY(SCALAR)
#else
    #define CFG_t0 PTR(SCALAR)
    #define CFG_tx PTR(SCALAR)
    #define CFG_ty PTR(ARRAY(SCALAR))
#endif


#if HAS_PTR_ITEM(CFG_ty) // configuration constraint determined by the algorithm
    #error "The inner element configuration of member 'test_t::y' must be SCALAR as constrained by 'strcpy'."
#endif


typedef struct
{
    int id;
    DECLARE(bool, x, CFG_tx);
    DECLARE_ARRAY(char, y, CFG_ty, 8);
} test_t;

int main()
{
    DECLARE(test_t, t0, CFG_t0);
    test_t t1; // the usage of initializers is forbidden here, for example: { 0, false, "test" }

    ALLOCATE(t0, CFG_t0);
    ALLOCATE(MEMBER_OF(t0, CFG_t0, x), CFG_tx);
    ALLOCATE(MEMBER_OF(t0, CFG_t0, y), CFG_ty);

    MEMBER_OF(t0, CFG_t0, id) = 0;
    VALUE_OF(MEMBER_OF(t0, CFG_t0, x), CFG_tx) = true;
    strcpy(&VALUE_OF(MEMBER_OF(t0, CFG_t0, y), CFG_ty), "test");
    printf("t0: { %d, %d, \"%s\" }\n",
           MEMBER_OF(t0, CFG_t0, id),
           VALUE_OF(MEMBER_OF(t0, CFG_t0, x), CFG_tx),
           VALUE_OF(MEMBER_OF(t0, CFG_t0, y), CFG_ty));

    DEALLOCATE(MEMBER_OF(t0, CFG_t0, y), CFG_ty);
    DEALLOCATE(MEMBER_OF(t0, CFG_t0, x), CFG_tx);
    DEALLOCATE(t0, CFG_t0);


    ALLOCATE(t1.x, CFG_tx);
    ALLOCATE(t1.y, CFG_ty);

    t1.id = 1;
    VALUE_OF(t1.x, CFG_tx) = true;
    strcpy(&VALUE_OF(t1.y, CFG_ty) , "test");
    printf("t1: { %d, %d, \"%s\" }\n",
           t1.id,
           VALUE_OF(t1.x, CFG_tx),
           VALUE_OF(t1.y, CFG_ty));

    DEALLOCATE(t1.y, CFG_ty);
    DEALLOCATE(t1.x, CFG_tx);

    return 0;
}
