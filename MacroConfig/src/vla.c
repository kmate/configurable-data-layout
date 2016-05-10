#include <stdio.h>

#define NAME_BASED_CONFIGURATION 1
#include "macro_config.h"


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    #define CONFIGURE__items ARRAY(SCALAR)
#else
    #define CONFIGURE__items PTR(ARRAY(PTR(SCALAR)))
#endif


int main()
{
    int n;

    printf("Item count: ");
    scanf("%d", &n);

    if(n > 0)
    {
        DECLARE_ARRAY(int, items, n);
        ALLOCATE(items);

        for(int i = 0; i < n; ++i)
        {
            scanf("%d", &VALUE_AT(items, i));
        }

        printf("Items:");

        for(int i = 0; i < n; ++i)
        {
            printf(" %d", VALUE_AT(items, i));
        }

        printf("\n");

        DEALLOCATE(items);
    }

    return 0;
}
