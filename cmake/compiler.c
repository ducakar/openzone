#if !defined( __GNUC__ ) || ( defined( __clang__ ) && __clang_major__ < 3 ) || \
    ( !defined( __clang__ ) && ( __GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ < 4 ) ) )
# error GCC >= 4.4 and LLVM/Clang 3.0 are the only supported compilers
#endif

int main()
{
#if !defined( __clang__ ) && __GNUC__ == 4 && __GNUC_MINOR__ < 6
  return 1;
#else
  return 0;
#endif
}
