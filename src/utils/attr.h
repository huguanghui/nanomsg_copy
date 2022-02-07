#ifndef NN_ATTR_INCLUDED
#define NN_ATTR_INCLUDED

#if defined __GNUC__ || defined __llvm__
#define NN_UNUSED __attribute__((unused))
#else
#define NN_UNUSED
#endif

#endif // !NN_ATTR_INCLUDED
