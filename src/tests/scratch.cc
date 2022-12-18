#include <common/common.hh>

#include "Foo.hh"

using namespace oz;

int main()
{
  Log() << 2048 * 2.0f * Math::FLOAT_EPS;
  Log() << EPSILON;
  return 0;
}
