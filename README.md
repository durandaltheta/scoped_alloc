# scoped_alloc
A simple C scoped allocated pointer library 

Enter a new scope with `enter_alloc_scope(user_function, optional_user_args...)`. 

Within `user_function`, any calls to `scope_alloc()` or `scope_malloc()`
(`scope_malloc()` is the simpler version of the two) will call an allocation 
function and register a destructor/deallocator function. When `user_function()`
returns, any pointers created by `scoped_alloc()`/`scoped_malloc()` will have 
their destructors called.

Any calls to `scoped_alloc()`/`scope_malloc()` made before calling 
`enter_alloc_scope()` must be handled by a previous/parent call to
`enter_alloc_scope()`.

```
#include <stdio.h>
#include "scoped_alloc.h"

struct user_struct_t {
    int a;
    int b;
    int c;
} s_init {
    .a = 1,
    .b = 2
};

void scoped_user_function(int c) {
    s_t* p = scope_malloc(user_struct_t_t); // allocate scoped pointer to a user_struct_t
    *p = s_init;
    p->c = c;
    printf("a[%d], b[%d], c[%d]", p->a, p->b, p->c);
    // no manual free
}

int main() {
    enter_alloc_scope(user_function, 4);
    // scoped pointer p is now deallocated
    return 0;
}
```
