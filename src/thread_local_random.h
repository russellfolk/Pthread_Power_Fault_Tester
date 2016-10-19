/**
  * Thread local is required in order to have a thread safe random number
  * generator. This is used in order to select which record I will write at a
  * given time. The reason this is required is so that each worker thread does
  * not select the same pattern of random numbers. E.g. thread 1 chooses 2, 355,
  * 7204, ...; thread 2 chooses 2, 355, 7204, ...; thread 3 chooses 2, 355,
  * 7204, ...; ...
  */

#ifndef thread_local
# if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && ( \
       defined _MSC_VER || \
       defined __ICL || \
       defined __DMC__ || \
       defined __BORLANDC__ )
#  define thread_local __declspec(thread)
/* note that ICC (linux) and Clang are covered by __GNUC__ */
# elif defined __GNUC__ || \
       defined __SUNPRO_C || \
       defined __xlC__
#  define thread_local __thread
# else
#  error "Cannot define thread_local"
# endif
#endif

#include <random>
#include <thread>