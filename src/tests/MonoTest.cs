using System;
using System.Runtime.CompilerServices;

namespace OpenZone
{

class Test
{
  [ MethodImplAttribute( MethodImplOptions.InternalCall ) ]
  public extern string getString();
}

}

public class MonoTest
{
  public static void Main( string[] args )
  {
    Console.WriteLine( new OpenZone.Test().getString() );
  }
}
