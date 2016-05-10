/* Copyright 2016 Karácsony Máté */
#ifndef MACRO_CONFIG_H_INCLUDED
#define MACRO_CONFIG_H_INCLUDED

#include <boost/preprocessor.hpp>


// Checking dependencies
#if BOOST_PP_VARIADICS < 1
    #error "The boost variadic macro support must be enabled! (see BOOST_PP_VARIADICS)"
#else


// Library configuration section
//----------------------------------------------------------------------------------------------------------------------


#ifndef NAME_BASED_CONFIGURATION
    #define NAME_BASED_CONFIGURATION 0
#endif // NAME_BASED_CONFIGURATION

#ifndef GET_OBJECT_CONFIG
    #define GET_OBJECT_CONFIG(id) CONFIGURE__ ## id
#endif // GET_OBJECT_CONFIG

#ifndef GET_MEMBER_CONFIG
    #define GET_MEMBER_CONFIG(type, member) CONFIGURE_MEMBER__ ## type ## __ ## member
#endif // GET_MEMBER_CONFIG

#ifndef GET_RETURN_CONFIG
    #define GET_RETURN_CONFIG(func) CONFIGURE_RETURN__ ## func
#endif // GET_RETURN_CONFIG

#ifndef GET_PARAM_CONFIG
    #define GET_PARAM_CONFIG(func, param) CONFIGURE_PARAM__ ## func ## __ ## param
#endif // GET_PARAM_CONFIG


#ifndef ALLOCATE_FUNCTION
    #define ALLOCATE_FUNCTION malloc
    #include <malloc.h> // for definition of malloc
#endif // ALLOCATE_FUNCTION

#ifndef DEALLOCATE_FUNCTION
    #define DEALLOCATE_FUNCTION free
    #include <malloc.h> // for definition of free
#endif // DEALLOCATE_FUNCTION


// Interface section
//----------------------------------------------------------------------------------------------------------------------


// Token sequence generators
#define SCALAR        (SCALAR)
#define PTR(tokens)   (PTR)tokens
#define ARRAY(tokens) (ARRAY)tokens


// Mapping to the implementation
#if NAME_BASED_CONFIGURATION < 1
    #define DECLARE          DECLARE_IMPL
    #define DECLARE_ARRAY    DECLARE_ARRAY_IMPL
    #define DECLARE_FUNCTION DECLARE_FUNCTION_IMPL
    #define ARGUMENT         DECLARE_IMPL
    #define ARRAY_ARGUMENT   DECLARE_ARRAY_IMPL
    #define ALLOCATE         ALLOCATE_IMPL
    #define DEALLOCATE       DEALLOCATE_IMPL
    #define VALUE_OF         VALUE_OF_IMPL
    #define VALUE_AT         VALUE_AT_IMPL
    #define MEMBER_OF        MEMBER_OF_IMPL
#else
    #define DECLARE(base_type, id) \
        DECLARE_IMPL(base_type, id, GET_OBJECT_CONFIG(id))

    #define DECLARE_ARRAY(base_type, ...) IIF( \
        GREATER(LENGTH(VAR_SEQ(__VA_ARGS__)), 1), \
        DECLARE_ARRAY_VA(base_type, __VA_ARGS__), \
        DECLARE_ARRAY_NO(base_type, HEAD(VAR_SEQ(__VA_ARGS__))) \
    )
    #define DECLARE_ARRAY_NO(base_type, id) \
        DECLARE_ARRAY_IMPL(base_type, id, GET_OBJECT_CONFIG(id))
    #define DECLARE_ARRAY_VA(base_type, id, ...) \
        DECLARE_ARRAY_IMPL(base_type, id, GET_OBJECT_CONFIG(id), __VA_ARGS__)

    #define DECLARE_FUNCTION(base_type, func, args) \
        DECLARE_FUNCTION_IMPL(base_type, func, args, GET_RETURN_CONFIG(func))

    #define ARGUMENT(base_type, func, param) \
        DECLARE_IMPL(base_type, param, GET_PARAM_CONFIG(func, param))

    #define ARRAY_ARGUMENT(base_type, func, ...) IIF( \
        GREATER(LENGTH(VAR_SEQ(__VA_ARGS__)), 1), \
        ARRAY_ARGUMENT_VA(base_type, func, __VA_ARGS__), \
        ARRAY_ARGUMENT_NO(base_type, func, HEAD(VAR_SEQ(__VA_ARGS__))) \
    )
    #define ARRAY_ARGUMENT_NO(base_type, func, param) \
        DECLARE_ARRAY_IMPL(base_type, param, GET_PARAM_CONFIG(func, param))
    #define ARRAY_ARGUMENT_VA(base_type, func, param, ...) \
        DECLARE_ARRAY_IMPL(base_type, param, GET_PARAM_CONFIG(func, param), __VA_ARGS__)

    #define ALLOCATE(id) \
        ALLOCATE_IMPL(id, GET_OBJECT_CONFIG(id))

    #define DEALLOCATE(id) \
        DEALLOCATE_IMPL(id, GET_OBJECT_CONFIG(id))

    #define VALUE_OF(id) \
        VALUE_OF_IMPL(id, GET_OBJECT_CONFIG(id))

    #define VALUE_AT(...) IIF( \
        GREATER(LENGTH(VAR_SEQ(__VA_ARGS__)), 1), \
        VALUE_AT_VA(__VA_ARGS__), \
        VALUE_AT_NO(HEAD(VAR_SEQ(__VA_ARGS__))) \
    )
    #define VALUE_AT_NO(id) \
        VALUE_AT_IMPL(id, GET_OBJECT_CONFIG(id))
    #define VALUE_AT_VA(id, ...) \
        VALUE_AT_IMPL(id, GET_OBJECT_CONFIG(id), __VA_ARGS__)

    #define MEMBER_OF(obj, member) \
        MEMBER_OF_IMPL(obj, GET_OBJECT_CONFIG(obj), member)

    #define DECLARE_MEMBER(type, base_type, member) \
        DECLARE_IMPL(base_type, member, GET_MEMBER_CONFIG(type, member))

    #define DECLARE_MEMBER_ARRAY(type, base_type, ...) IIF( \
        GREATER(LENGTH(VAR_SEQ(__VA_ARGS__)), 1), \
        DECLARE_MEMBER_ARRAY_VA(type, base_type, __VA_ARGS__), \
        DECLARE_MEMBER_ARRAY_NO(type, base_type, HEAD(VAR_SEQ(__VA_ARGS__))) \
    )
    #define DECLARE_MEMBER_ARRAY_NO(type, base_type, member) \
        DECLARE_ARRAY_IMPL(base_type, member, GET_MEMBER_CONFIG(type, member))
    #define DECLARE_MEMBER_ARRAY_VA(type, base_type, member, ...) \
        DECLARE_ARRAY_IMPL(base_type, member, GET_MEMBER_CONFIG(type, member), __VA_ARGS__)

    #define ALLOCATE_MEMBER(type, obj, member) \
        ALLOCATE_IMPL((obj).member, GET_MEMBER_CONFIG(type, member))

    #define DEALLOCATE_MEMBER(type, obj, member) \
        DEALLOCATE_IMPL((obj).member, GET_MEMBER_CONFIG(type, member))

    #define VALUE_OF_MEMBER(type, obj, member) \
        VALUE_OF_IMPL((obj).member, GET_MEMBER_CONFIG(type, member))

    #define VALUE_AT_MEMBER(type, obj, ...) IIF( \
        GREATER(LENGTH(VAR_SEQ(__VA_ARGS__)), 1), \
        VALUE_AT_MEMBER_VA(type, obj, __VA_ARGS__), \
        VALUE_AT_MEMBER_NO(type, obj, HEAD(VAR_SEQ(__VA_ARGS__))) \
    )
    #define VALUE_AT_MEMBER_NO(type, obj, member) \
        VALUE_AT_IMPL((obj).member, GET_MEMBER_CONFIG(type, member))
    #define VALUE_AT_MEMBER_VA(type, obj, member, ...) \
        VALUE_AT_IMPL((obj).member, GET_MEMBER_CONFIG(type, member), __VA_ARGS__)

#endif // NAME_BASED_CONFIGURATION


// Implementation section
//----------------------------------------------------------------------------------------------------------------------


// Error handling
#define ERROR(msg) char msg[-1]


// Macro aliases for shorter code
#define APPLY         BOOST_PP_APPLY
#define CAT           BOOST_PP_CAT

#define AND           BOOST_PP_AND
#define LESS          BOOST_PP_LESS
#define GREATER       BOOST_PP_GREATER
#define IIF           BOOST_PP_IIF
#define EXPR_IIF      BOOST_PP_EXPR_IIF

#define INC           BOOST_PP_INC
#define DEC           BOOST_PP_DEC

#define FST(tuple)    BOOST_PP_TUPLE_ELEM(0, tuple)
#define SND(tuple)    BOOST_PP_TUPLE_ELEM(1, tuple)

#define EMPTY         BOOST_PP_EMPTY()
#define LENGTH        BOOST_PP_SEQ_SIZE
#define HEAD(seq)     BOOST_PP_LIST_FIRST(SEQ_LIST(seq)) // safer than BOOST_PP_SEQ_HEAD
#define TAIL          BOOST_PP_SEQ_TAIL
#define PUSH          BOOST_PP_SEQ_PUSH_BACK
#define FOLDL         BOOST_PP_SEQ_FOLD_LEFT
#define VAR_SEQ       BOOST_PP_VARIADIC_TO_SEQ

#define FOLDL_LIST    BOOST_PP_LIST_FOLD_LEFT
#define SEQ_LIST      BOOST_PP_SEQ_TO_LIST


// Configuration tokens
#define T_SCALAR APPLY(SCALAR)
#define T_PTR    APPLY(PTR())
#define T_ARRAY  APPLY(ARRAY())


// Item configuration
#define ITEM_CONFIG(c) FST(FOLDL(ITEM_CONFIG_, (EMPTY, 0), c))
#define ITEM_CONFIG_(s, state, token) CAT(ITEM_CONFIG_, token) state
#define ITEM_CONFIG_SCALAR(cfg, add) (IIF(add, PUSH(cfg, T_SCALAR), cfg), add)
#define ITEM_CONFIG_PTR(cfg, add)    (IIF(add, PUSH(cfg, T_PTR), cfg), add)
#define ITEM_CONFIG_ARRAY(cfg, add)  (IIF(add, PUSH(cfg, T_ARRAY), cfg), 1)


// Item configuration pointer checking
#define HAS_PTR_ITEM(c) FST(FOLDL(HAS_PTR_ITEM_, (0, 0), c))
#define HAS_PTR_ITEM_(s, state, token) CAT(HAS_PTR_ITEM_, token) state
#define HAS_PTR_ITEM_SCALAR(hpr, inar) (hpr, inar)
#define HAS_PTR_ITEM_PTR(hpr, inar)    (IIF(inar, 1, hpr), inar)
#define HAS_PTR_ITEM_ARRAY(hpr, inar)  (hpr, 1)


// Array rank (the number of dimensions) of a configuration
#define ARRAY_RANK(c) FOLDL(ARRAY_RANK_, 0, c)
#define ARRAY_RANK_(s, state, token) CAT(ARRAY_RANK_, token) (state)
#define ARRAY_RANK_SCALAR(cd) cd
#define ARRAY_RANK_PTR(cd)    cd
#define ARRAY_RANK_ARRAY(cd)  INC(cd)


// Declaration
#define DECLARE_IMPL(base_type, id, c) IIF(\
    GREATER(ARRAY_RANK(c), 0), \
    ERROR(For_array_configurations_use_the_DECLARE_ARRAY_macro), \
    base_type FST(DECLARE_(id, c)) \
)
#define DECLARE_(id, c) FOLDL(DECLARE_APPLY_TOKEN, (EMPTY, id), c)
#define DECLARE_APPLY_TOKEN(s, state, token) CAT(DECLARE_APPLY_TOKEN_, token) state
#define DECLARE_APPLY_TOKEN_SCALAR(code, id) (code id, id)
#define DECLARE_APPLY_TOKEN_PTR(code, id)    (*code, id)

#define DECLARE_ARRAY_IMPL(base_type, id, ...) IIF( \
    GREATER(ARRAY_RANK(HEAD(VAR_SEQ(__VA_ARGS__))), DEC(LENGTH(VAR_SEQ(__VA_ARGS__)))), \
    ERROR(Please_specify_the_size_of_all_array_dimensions_for_DECLARE_ARRAY_macro), \
    IIF( \
        LESS(ARRAY_RANK(HEAD(VAR_SEQ(__VA_ARGS__))), DEC(LENGTH(VAR_SEQ(__VA_ARGS__)))), \
        ERROR(Too_many_dimension_sizes_specified_for_DECLARE_ARRAY_macro), \
        base_type FST(DECLARE_ARRAY_(id, VAR_SEQ(__VA_ARGS__))) \
    ) \
)
#define DECLARE_ARRAY_(id, c_and_dim) FOLDL(DECLARE_ARRAY_APPLY_TOKEN, (EMPTY, id, 1, TAIL(c_and_dim)), HEAD(c_and_dim))
#define DECLARE_ARRAY_APPLY_TOKEN(s, state, token) CAT(DECLARE_ARRAY_APPLY_TOKEN_, token) state
#define DECLARE_ARRAY_APPLY_TOKEN_SCALAR(code, id, aid, dim) (code, id, aid, dim)
#define DECLARE_ARRAY_APPLY_TOKEN_PTR(code, id, aid, dim)    ((*code EXPR_IIF(aid, id)), id, 0, dim)
#define DECLARE_ARRAY_APPLY_TOKEN_ARRAY(code, id, aid, dim)  ((code EXPR_IIF(aid, id)[HEAD(dim)]), id, 0, TAIL(dim))

// for function return types
#define DECLARE_FUNCTION_IMPL(base_type, id, args, c) base_type FST(DECLARE_FUNCTION_(id, args, c))
#define DECLARE_FUNCTION_(id, args, c) FOLDL(DECLARE_FUNCTION_APPLY_TOKEN, (EMPTY, id, args), c)
#define DECLARE_FUNCTION_APPLY_TOKEN(s, state, token) CAT(DECLARE_FUNCTION_APPLY_TOKEN_, token) state
#define DECLARE_FUNCTION_APPLY_TOKEN_SCALAR(code, id, args) (code id args, id, args)
#define DECLARE_FUNCTION_APPLY_TOKEN_PTR(code, id, args)    (*code, id, args)
#define DECLARE_FUNCTION_APPLY_TOKEN_ARRAY(code, id, args)  (*code, id, args)


// Allocation
#define ALLOCATE_IMPL(id, c) do { FST(ALLOCATE_(id, c)) } while(0)
#define ALLOCATE_(id, c) FOLDL(ALLOCATE_APPLY_TOKEN, (EMPTY, EMPTY, EMPTY, id, 0, c), c)
#define ALLOCATE_APPLY_TOKEN(s, state, token) CAT(ALLOCATE_APPLY_TOKEN_, token) state
#define ALLOCATE_APPLY_TOKEN_SCALAR(code, l, r, id, d, c) (l r, l, r, id, d, c)
#define ALLOCATE_APPLY_TOKEN_PTR(code, l, r, id, d, c)    \
    ( code \
    , l id = ALLOCATE_FUNCTION(sizeof(*(id))); \
    , r \
    , *(id) \
    , d \
    , c \
    )
#define ALLOCATE_APPLY_TOKEN_ARRAY(code, l, r, id, d, c)  \
    ( code \
    , l EXPR_IIF(HAS_PTR_ITEM(c), for(int i ## d = 0; i ## d < sizeof(id) / sizeof((id)[0]); ++i ## d) {) \
    , EXPR_IIF(HAS_PTR_ITEM(c), }) r \
    , (id)[i ## d] \
    , INC(d) \
    , ITEM_CONFIG(c) \
    )


// Deallocation
#define DEALLOCATE_IMPL(id, c) do { FST(DEALLOCATE_(id, c)) } while(0)
#define DEALLOCATE_(id, c) FOLDL(DEALLOCATE_APPLY_TOKEN, (EMPTY, EMPTY, EMPTY, id, 0, c), c)
#define DEALLOCATE_APPLY_TOKEN(s, state, token) CAT(DEALLOCATE_APPLY_TOKEN_, token) state
#define DEALLOCATE_APPLY_TOKEN_SCALAR(code, l, r, id, d, c) (l r, l, r, id, d, c)
#define DEALLOCATE_APPLY_TOKEN_PTR(code, l, r, id, d, c)    \
    ( code \
    , l \
    , DEALLOCATE_FUNCTION(id); r \
    , *(id) \
    , d \
    , c \
    )
#define DEALLOCATE_APPLY_TOKEN_ARRAY(code, l, r, id, d, c)  \
    ( code \
    , l EXPR_IIF(HAS_PTR_ITEM(c), for(int i ## d = 0; i ## d < sizeof(id) / sizeof((id)[0]); ++i ## d) {) \
    , EXPR_IIF(HAS_PTR_ITEM(c), }) r \
    , (id)[i ## d] \
    , INC(d) \
    , ITEM_CONFIG(c) \
    )


// Value of
#define VALUE_OF_IMPL(id, c) FST(VALUE_OF_(id, c))
#define VALUE_OF_(id, c) FOLDL(VALUE_OF_APPLY_TOKEN, (EMPTY, id, 1), c)
#define VALUE_OF_APPLY_TOKEN(s, state, token) CAT(VALUE_OF_APPLY_TOKEN_, token) state
#define VALUE_OF_APPLY_TOKEN_SCALAR(code, id, apr) (code(id), id, 0)
#define VALUE_OF_APPLY_TOKEN_PTR(code, id, apr)    (EXPR_IIF(apr, *)code, id, apr)
#define VALUE_OF_APPLY_TOKEN_ARRAY(code, id, apr)  (code, id, 0)


// Value at a specified array index
#define VALUE_AT_IMPL(id, ...) IIF( \
    LESS(LENGTH(VAR_SEQ(__VA_ARGS__)), 2), \
    ERROR(Please_specify_at_least_one_index_for_VALUE_AT_macro), \
    IIF( \
        LESS(ARRAY_RANK(HEAD(VAR_SEQ(__VA_ARGS__))), DEC(LENGTH(VAR_SEQ(__VA_ARGS__)))), \
        ERROR(Too_many_indexes_specified_for_VALUE_AT_macro), \
        VALUE_AT_(id, VAR_SEQ(__VA_ARGS__)) \
    ) \
)
#define VALUE_AT_(id, c_and_idx) VALUE_AT__(id, HEAD(c_and_idx), SEQ_LIST(TAIL(c_and_idx)))
#define VALUE_AT__(id, c, idx) FST(FOLDL_LIST(VALUE_AT_APPLY_TOKEN, (VALUE_OF_IMPL(id, c), ITEM_CONFIG(c)), idx))
#define VALUE_AT_APPLY_TOKEN(s, state, i) (VALUE_OF_IMPL(FST(state), SND(state))[i], ITEM_CONFIG(SND(state)))


// Structure member access
#define MEMBER_OF_IMPL(obj, objc, member) (VALUE_OF_IMPL(obj, objc)).member


#endif // BOOST_PP_VARIADICS

#endif // MACRO_CONFIG_H_INCLUDED
