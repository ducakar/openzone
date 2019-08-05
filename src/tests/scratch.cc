#include <ozCore/ozCore.hh>

#include "Foo.hh"

using namespace oz;

int main()
{
  List<int>    l   = {1, 3, 2};
  int          a[] = {1, 2, 3};
  HashSet<int> hs  = {1, 2, 3};

  l.sort();

  Log() << (crange(l) == crange(a));
  Log() << (crange(a) == crange(a));
  Log() << (crange(hs) == crange(a));

  for (int i : hs) {
    Log() << i;
  }
  return 0;
}
