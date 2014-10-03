#ifndef H_ASSERT
 #define H_ASSERT

#ifdef NDEBUG
#define assert(x)
#else
#define assert(x) do { if (!(x)) puts("\n\n\nASSERTION FAILURE (" #x ")\n\n\n"); } while (0)
#endif

#endif /* H_ASSERT */
