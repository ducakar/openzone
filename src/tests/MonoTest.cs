using System;
using System.Runtime.CompilerServices;

namespace OpenZone
{

struct Vec3
{
  public float x;
  public float y;
  public float z;

  public Vec3( float x, float y, float z )
  {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  public static Vec3 operator * ( Vec3 v, float k )
  {
    return new Vec3( v.x * k, v.y * k, v.z * k );
  }

  public static Vec3 operator * ( float k, Vec3 v )
  {
    return new Vec3( v.x * k, v.y * k, v.z * k );
  }
}

struct Point
{
  public float x;
  public float y;
  public float z;

  public Point( float x, float y, float z )
  {
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

class Object
{
  private int index;

  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern Point getP( int index, ref float x, ref float y, ref float z );
  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern void setP( int index, float x, float y, float z );

  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern Vec3 getVelocity( int index, ref float x, ref float y, ref float z );
  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern void setVelocity( int index, float x, float y, float z );

  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern float getLife( int index );
  [ MethodImplAttribute(MethodImplOptions.InternalCall) ]
  private extern void setLife( int index, float value );

  public Object( int index )
  {
    this.index = index;
  }

  public Point p {
    get {
      Point p = new Point();
      getP( index, ref p.x, ref p.y, ref p.z );
      return p;
    }
    set {
      setP( index, value.x, value.y, value.z );
    }
  }

  public Vec3 velocity {
    get {
      Vec3 v = new Vec3();
      getVelocity( index, ref v.x, ref v.y, ref v.z );
      return v;
    }
    set {
      setVelocity( index, value.x, value.y, value.z );
    }
  }

  public float life {
    get {
      return getLife( index );
    }
    set {
      setLife( index, value );
    }
  }
}

}

public class MonoTest
{
  public static void Main( string[] args )
  {
    OpenZone.Object o = new OpenZone.Object( 42 );

    o.p = new OpenZone.Point( 5.0f, 6.0f, 7.0f );
    o.velocity = new OpenZone.Vec3( 1.0f, 2.0f, 3.0f );
    o.velocity *= 0.5f;
    o.velocity = 3.0f * o.velocity;
    o.life *= 2.0f;

    Console.WriteLine( "p [{0}, {1}, {2}] velocity ({3}, {4}, {5}) life {6}",
                       o.p.x, o.p.y, o.p.z, o.velocity.x, o.velocity.y, o.velocity.z, o.life );
  }
}
