#include <cstdio>
#include <cstdint>
#include <cstring>
#include "template_config.hpp"

using namespace cfg;


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    typedef SCALAR input_config;
    typedef ARRAY<ARRAY<SCALAR>> table_config;
#else
    typedef PTR<SCALAR> input_config;
    typedef ARRAY<PTR<ARRAY<SCALAR>>> table_config;
#endif


BOOST_MPL_ASSERT_MSG( // configuration constraint determined by the algorithm
    (mpl::not_<has_ptr_item<item_config<table_config>>>::type::value),
    The_inner_element_configuration_of__table__must_be_SCALAR_as_constrained_by__strcpy,
    (table_config)
);


struct input_t
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
};

configured<struct input_t, input_config, false> input;

configured_array<char, table_config, false, 8, 32> table;

void init()
{
    input.allocate();
    table.allocate();
    strcpy(table[0], "Zero");
    strcpy(table[1], "One");
    strcpy(table[2], "Two");
    strcpy(table[3], "Three");
    strcpy(table[4], "Four");
    strcpy(table[5], "Five");
    strcpy(table[6], "Six");
    strcpy(table[7], "Seven");
}

void lookup()
{
    uint32_t key = (input->a + input->b + input->c + input->d) % 8;
    printf("%s\n", table[key]());
}

int main()
{
    init();
    scanf("%d %d %d %d", &input->a, &input->b, &input->c, &input->d);
    lookup();

    input.deallocate();
    table.deallocate();

    return 0;
}
