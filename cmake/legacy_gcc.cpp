#if defined( __clang__ ) || __GNUC__ > 4 || __GNUC_MINOR__ >= 6
# error OK, no legacy GCC < 4.6
#endif

int main()
{
  return 0;
}
