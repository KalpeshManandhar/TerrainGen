#define DEBUG

#ifdef DEBUG
#include "./Utils/timeMeasure.h"

#define TIME(x) n_time x = get_current_time()
#define TIME_DIFF(st,end)  t_diff(st, end)

#include <assert.h>

#define ASSERT(condition)   assert(condition)
#else
#define TIME(x) 
#define TIME_DIFF(st,end)  0

#define ASSERT(condition)   

#endif 