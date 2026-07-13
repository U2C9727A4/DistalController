# Note: This file is for myself (U2C97)
This file is for myself because im a forgetful person, and this project has been going on for a long time.  
Think of it as a sort of CoC for this project and patterns of code, to avoid becoming a soup.  

# "If its something you share, then its a global."  
## Bad code:  

common.c
```c
// <empty>
```

my_source.c
```c
static int my_shared_value = 0;
// <Insert pointer soup for sharing my_shared_value>
```

## Good code:

common.c
```c
int my_shared_value = 0;
```
common.h
```c
extern int my_shared_value;
```

good_source.c
```c
#include "common.h"

static int my_copied_value = my_shared_value;
```