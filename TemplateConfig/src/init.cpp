#include <cstdio>
#include "template_config.hpp"

using namespace cfg;


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    typedef ARRAY<SCALAR> CFG_inner_data;
    typedef SCALAR CFG_outer_flag;
    typedef SCALAR CFG_outer_payload;
    typedef SCALAR CFG_a;
    typedef SCALAR CFG_b;
    typedef ARRAY<ARRAY<SCALAR>> CFG_c;
    typedef SCALAR CFG_d;
#else
    typedef PTR<ARRAY<PTR<SCALAR>>> CFG_inner_data;
    typedef PTR<SCALAR> CFG_outer_flag;
    typedef PTR<SCALAR> CFG_outer_payload;
    typedef PTR<PTR<SCALAR>> CFG_a;
    typedef PTR<SCALAR> CFG_b;
    typedef PTR<ARRAY<PTR<ARRAY<PTR<SCALAR>>>>> CFG_c;
    typedef PTR<PTR<SCALAR>> CFG_d;
#endif


void print_result(const char * test_name, bool result)
{
    printf("%s [%s]\n", test_name, result ? "success" : "failure");
}


struct inner
{
    int length;
    configured_array<int, CFG_inner_data, true, 10> data;
};

struct outer
{
    int id;
    configured<bool, CFG_outer_flag> flag;
    configured<inner, CFG_outer_payload> payload;
};


int main()
{
    configured<int, CFG_a> a = 42;
    print_result("Scalar initialization", 42 == a);


    configured<int, CFG_a> ac(a);
    print_result("Copy constructor", a() == ac());


    configured<int, CFG_a> aa;
    aa = a;
    print_result("Assignment operator", a() == aa());


    configured<int, CFG_b> b = a();
    print_result("Value converter constructor", 42 == b);


    b = 64;
    print_result("Assignment from scalar", 64 == b);
    print_result("Assignment is independent", 42 == a);


    decltype(a)::value_of_type& avr = a;
    avr = 32;
    print_result("Value reference assignment", 32 == a);


    decltype(b)& bpr = b;
    bpr = 16;
    print_result("Proxy reference assignment", 16 == b);


    configured_array<double, CFG_c, true, 2, 3> c;
    c[0] = { 1, 2, 3 };
    c[1] = { 4, 0, 6 };
    print_result(
        "Array initializer assignment",
        1 == c[0][0] && 2 == c[0][1] && 3 == c[0][2] &&
        4 == c[1][0] && 0 == c[1][1] && 6 == c[1][2]
    );


    c[1][1] = 5;
    print_result("Multiple indexing assignment", 5 == c[1][1]);


    outer o = { 1, true, (inner){ 5, { 0, 2, 4, 6, 8 } } };
    const auto& opdata = o.payload->data;
    print_result(
        "Nested structure initializer",
        1 == o.id && true == o.flag && 5 == o.payload->length &&
        0 == opdata[0] && 2 == opdata[1] && 4 == opdata[2] && 6 == opdata[3] && 8 == opdata[4]
    );


    configured<outer, CFG_d> d = o;
    const auto& dpdata = d->payload->data;
    print_result(
        "Structure assignment initializer",
        1 == d->id && true == d->flag && 5 == d->payload->length &&
        0 == dpdata[0] && 2 == dpdata[1] && 4 == dpdata[2] && 6 == dpdata[3] && 8 == dpdata[4]
    );


    configured<outer, CFG_d> di = (outer){ 1, true, (inner){ 5, { 0, 2, 4, 6, 8 } } };
    const auto& dipdata = di->payload->data;
    print_result(
        "Nested structure initializer",
        1 == di->id && true == di->flag && 5 == di->payload->length &&
        0 == dipdata[0] && 2 == dipdata[1] && 4 == dipdata[2] && 6 == dipdata[3] && 8 == dipdata[4]
    );


    return 0;
}
