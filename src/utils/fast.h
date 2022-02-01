#ifndef NN_FAST_INCLUDED
#define NN_FAST_INCLUDED

#if defined __GNUC__ || defined __llvm__
#define nn_fast(x) __builtin_expect((x), 1)
#define nn_slow(x) __builtin_expect((x), 0)
#else
#define nn_fast(x) (x)
#define nn_slow(x) (x)
#endif

#endif // !NN_FAST_INCLUDED
