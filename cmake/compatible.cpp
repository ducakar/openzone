#if defined( __clang__ )
# if __clang_major__ < 3
#  error Only LLVM/Clang >= 3.0 is supported.
# endif
#elif defined( __GNUC__ )
# if __GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ < 4 )
#  error Only GCC >= 4.4 is supported.
# endif
#else
# error GCC >= 4.4 and LLVM/Clang >= 3.0 are the only supported compilers.
#endif

int main()
{
  return 0;
}
