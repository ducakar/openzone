#include <ozCore/ozCore.hh>

#include "Foo.hh"

using namespace oz;

int main()
{
  int          a[] = {1, 2, 3};
  List<int>    l   = {1, 3, 2};
  HashSet<int> hs  = {1, 2, 3};

  for (auto& i : range(a)) {
    Log() << i;
  }
  return 0;
}
