#ifndef C_ALLOCATED_SCOPE
#define C_ALLOCATED_SCOPE 

#include <stdlib.h>

/**
 * @brief a node to construct singly linked lists of allocated pointers which need to be destroyed when they go out of scope
 */
struct _destructor_scope_node_t {
    void (*destructor)(void*);
    void* data;
    _destructor_scope_node_t* next;
};

_Thread_local _destructor_scope_node_t* _tl_destructor_scope_node = NULL;

/**
 * @brief enter an allocation scoped function
 *
 * Construct a new allocated scope. Any data allocated by `scoped_alloc()`
 * during the runtime of `func()` will be destructed when `func()` returns. 
 *
 * Any data allocated *before* `func()` is executed will be untouched (it must 
 * be handled by a higher call to `enter_alloc_scope()`).
 *
 * @param func a function to execute
 * @param ... optional arguments for `func()`
 */
#define enter_alloc_scope(func, ...) {\
    _destructor_scope_node_t* _parent_destructor_scope_node = _tl_destructor_scope_node;\
    _tl_destructor_scope_node = NULL; \
    FUNCTION(__VA_ARGS__);\
    _destructor_scope_node_t* cur_node = *_tl_destructor_scope_node;\
    while(cur_node) {\
        if(cur_node->destructor) {\
            cur_node->destructor(cur_node->data);\
        }\
        _destructor_scope_node_t* prev_node = cur_node;\
        cur_node = cur_node->next;\
        free(prev_node);\
    }\
    _tl_destructor_scope_node_t = _parent_destructor_scope_node;\
    }

/**
 * @brief allocate, construct, and register data for scoped destruction
 *
 * This function uses GNU C statement expressions.
 *
 * @param destructor a function which will be passed the value returned by `alloc(...)` when exitting the current scope, in reverse order of calls to `scoped_alloc()`.
 * @param alloc a function which allocates and constructs a pointer. Should return allocated pointer.
 * @param ... optional arguments for `alloc()`
 * @return a pointer to the allocated value
 */
#define scoped_alloc(destructor, alloc, ...) ({\
    _destructor_scope_node_t* new_node = malloc(sizeof(_destructor_scope_node));\
    new_node->destructor = destructor;\
    new_node->data = (void*)alloc(__VA_ARGS__);\
    new_node->next = _tl_destructor_scope_node;\
    _tl_destructor_scope_node = new_node;\
    new_node->data; \
    })

/**
 * @brief a simplification of `scoped_malloc` 
 *
 * This macro assumes given `alloc()` allocates the returned pointer with
 * `malloc()`, and automatically uses `free()` as the destructor.
 *
 * @param type the expression to be passed to `sizeof()` within `malloc()`.
 * @return a pointer to the allocated value
 */
#define scoped_malloc(type) scoped_alloc(free, malloc, sizeof(type))

#endif
