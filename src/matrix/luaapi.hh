struct MatrixLuaState
{
  Object*         self;
  Bot*            user;

  Struct*         str;
  Object*         obj;
  Frag*           frag;

  int             strIndex;
  int             objIndex;

  Vector<Struct*> structs;
  Vector<Object*> objects;

  bool            hasUseFailed;
};

static MatrixLuaState ms;

/*
 * General functions
 */

static int ozPrintln( lua_State* l )
{
  ARG( 1 );

  log.println( "M> %s", tostring( 1 ) );
  return 0;
}

static int ozException( lua_State* l )
{
  ARG( 1 );

  const char* message = tostring( 1 );
  throw Exception( message );
}

static int ozUseFailed( lua_State* l )
{
  ARG( 0 );

  ms.hasUseFailed = true;
  return 1;
}

/*
 * Orbis
 */

static int ozOrbisGetGravity( lua_State* l )
{
  ARG( 0 );

  pushfloat( physics.gravity );
  return 1;
}

static int ozOrbisSetGravity( lua_State* l )
{
  ARG( 1 );

  physics.gravity = tofloat( 1 );
  return 0;
}

static int ozOrbisAddStr( lua_State* l )
{
  ARG( 5 );

  const char* name    = tostring( 1 );
  Point3      p       = Point3( tofloat( 2 ), tofloat( 3 ), tofloat( 4 ) );
  Heading     heading = Heading( toint( 5 ) );

  ms.str = synapse.addStruct( name, p, heading );
  pushint( ms.str == null ? -1 : ms.str->index );
  return 1;
}

static int ozOrbisTryAddStr( lua_State* l )
{
  ARG( 5 );

  const char* name    = tostring( 1 );
  Point3      p       = Point3( tofloat( 2 ), tofloat( 3 ), tofloat( 4 ) );
  Heading     heading = Heading( toint( 5 ) );

  Bounds bounds = *library.bsp( name );
  bounds = Struct::rotate( bounds, heading ) + ( p - Point3::ORIGIN );

  if( collider.overlaps( bounds.toAABB() ) ) {
    ms.str = null;
    pushint( -1 );
  }
  else {
    ms.str = synapse.addStruct( name, p, heading );
    pushint( ms.str == null ? -1 : ms.str->index );
  }
  return 1;
}

static int ozOrbisAddObj( lua_State* l )
{
  ARG_VAR( 4 );

  const char* name    = tostring( 1 );
  Point3      p       = Point3( tofloat( 2 ), tofloat( 3 ), tofloat( 4 ) );
  Heading     heading = Heading( gettop() == 5 ? toint( 5 ) : Math::rand( 4 ) );

  ms.obj = synapse.addObject( name, p, heading );
  pushint( ms.obj == null ? -1 : ms.obj->index );
  return 1;
}

static int ozOrbisTryAddObj( lua_State* l )
{
  ARG_VAR( 4 );

  const char* name    = tostring( 1 );
  Point3      p       = Point3( tofloat( 2 ), tofloat( 3 ), tofloat( 4 ) );
  Heading     heading = Heading( gettop() == 5 ? toint( 5 ) : Math::rand( 4 ) );

  const ObjectClass* clazz = library.objClass( name );

  AABB aabb = AABB( p, clazz->dim );

  if( heading & WEST_EAST_MASK ) {
    swap( aabb.dim.x, aabb.dim.y );
  }

  if( collider.overlaps( aabb ) ) {
    ms.obj = null;
    pushint( -1 );
  }
  else {
    ms.obj = synapse.addObject( name, p, heading );
    pushint( ms.obj == null ? -1 : ms.obj->index );
  }
  return 1;
}

static int ozOrbisAddFrag( lua_State* l )
{
  ARG( 7 );

  const char* name     = tostring( 1 );
  Point3      p        = Point3( tofloat( 2 ), tofloat( 3 ), tofloat( 4 ) );
  Vec3        velocity = Vec3( tofloat( 5 ), tofloat( 6 ), tofloat( 7 ) );

  ms.frag = synapse.addFrag( name, p, velocity );
  pushint( ms.frag == null ? -1 : ms.frag->index );
  return 1;
}

static int ozOrbisGenFrags( lua_State* l )
{
  ARG( 11 );

  const char* name     = tostring( 1 );
  int         nFrags   = toint( 2 );
  Bounds      bb       = Bounds( Point3( tofloat( 3 ), tofloat( 4 ), tofloat( 5 ) ),
                                 Point3( tofloat( 6 ), tofloat( 7 ), tofloat( 8 ) ) );
  Vec3        velocity = Vec3( tofloat( 9 ), tofloat( 10 ), tofloat( 11 ) );

  synapse.genFrags( name, nFrags, bb, velocity );
  ms.frag = null;
  return 0;
}

static int ozOrbisBindAllOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  ms.objects.clear();
  ms.structs.clear();
  collider.getOverlaps( aabb, &ms.objects, &ms.structs );
  ms.objIndex = 0;
  ms.strIndex = 0;
  return 0;
}

static int ozOrbisBindStrOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  ms.structs.clear();
  collider.getOverlaps( aabb, null, &ms.structs );
  ms.strIndex = 0;
  return 0;
}

static int ozOrbisBindObjOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  ms.objects.clear();
  collider.getOverlaps( aabb, &ms.objects, null );
  ms.objIndex = 0;
  return 0;
}

/*
 * Terra
 */

static int ozTerraLoad( lua_State* l )
{
  ARG( 1 );

  String name = tostring( 1 );
  int id = library.terraIndex( name );

  orbis.terra.load( id );
  return 0;
}

static int ozTerraHeight( lua_State* l )
{
  ARG( 2 );

  float x = tofloat( 1 );
  float y = tofloat( 2 );

  pushfloat( orbis.terra.height( x, y ) );
  return 1;
}

/*
 * Caelum
 */

static int ozCaelumLoad( lua_State* l )
{
  ARG( 1 );

  String name = tostring( 1 );
  int id = library.caelumIndex( name );

  orbis.caelum.id = id;
  return 0;
}

static int ozCaelumGetHeading( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.heading );
  return 1;
}

static int ozCaelumSetHeading( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.heading = tofloat( 1 );
  return 0;
}

static int ozCaelumGetPeriod( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.period );
  return 1;
}

static int ozCaelumSetPeriod( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.period = tofloat( 1 );
  return 0;
}

static int ozCaelumGetTime( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.time );
  return 1;
}

static int ozCaelumSetTime( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.time = tofloat( 1 );
  return 0;
}

static int ozCaelumAddTime( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.time += tofloat( 1 );
  return 0;
}

/*
 * Structure
 */

static int ozStrBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.structs.length() ) ) {
    ERROR( "invalid structure index" );
  }
  ms.str = orbis.structs[index];
  return 0;
}

static int ozStrBindNext( lua_State* l )
{
  ARG( 0 );

  if( ms.strIndex < ms.structs.length() ) {
    ms.str = ms.structs[ms.strIndex];
    ++ms.strIndex;
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

static int ozStrIsNull( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.str == null );
  return 1;
}

static int ozStrGetIndex( lua_State* l )
{
  ARG( 0 );

  if( ms.str == null ) {
    pushint( -1 );
  }
  else {
    pushint( ms.str->index );
  }
  return 1;
}

static int ozStrGetBounds( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( ms.str->mins.x );
  pushfloat( ms.str->mins.y );
  pushfloat( ms.str->mins.z );
  pushfloat( ms.str->maxs.x );
  pushfloat( ms.str->maxs.y );
  pushfloat( ms.str->maxs.z );
  return 6;
}

static int ozStrGetPos( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( ms.str->p.x );
  pushfloat( ms.str->p.y );
  pushfloat( ms.str->p.z );
  return 3;
}

static int ozStrGetBSP( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushstring( ms.str->bsp->name );
  return 1;
}

static int ozStrGetHeading( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushint( ms.str->heading );
  return 1;
}

static int ozStrGetLife( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( ms.str->life );
  return 1;
}

static int ozStrSetLife( lua_State* l )
{
  ARG( 1 );
  STR_NOT_NULL();

  ms.str->life = tofloat( 1 );
  return 0;
}

static int ozStrAddLife( lua_State* l )
{
  ARG( 1 );
  STR_NOT_NULL();

  ms.str->life += tofloat( 1 );
  return 0;
}

static int ozStrDamage( lua_State* l )
{
  ARG( 1 );
  STR_NOT_NULL();

  ms.str->damage( tofloat( 1 ) );
  return 0;
}

static int ozStrDestroy( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  ms.str->destroy();
  return 0;
}

static int ozStrRemove( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  synapse.remove( ms.str );
  ms.str = null;
  return 0;
}

static int ozStrGetEntityLock( lua_State* l )
{
  ARG( 1 );
  STR_NOT_NULL();

  int entIndex = toint( 1 );

  pushint( ms.str->entities[entIndex].key );
  return 1;
}

static int ozStrSetEntityLock( lua_State* l )
{
  ARG( 2 );
  STR_NOT_NULL();

  int entIndex = toint( 1 );
  int key      = toint( 2 );

  ms.str->entities[entIndex].key = key;
  return 0;
}

static int ozStrVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  Vec3 vec = ms.str->p - ms.self->p;
  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

static int ozStrVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  Vec3 vec = ms.str->p - eye;
  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

static int ozStrDirectionFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  Vec3 dir = ~( ms.str->p - ms.self->p );
  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

static int ozStrDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  Vec3 dir = ~( ms.str->p - eye );
  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

static int ozStrDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( !( ms.str->p - ms.self->p ) );
  return 1;
}

static int ozStrDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  pushfloat( !( ms.str->p - eye ) );
  return 1;
}

static int ozStrRelativeHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  float dx = ms.str->p.x - ms.self->p.x;
  float dy = ms.str->p.y - ms.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

static int ozStrHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  float dx = ms.str->p.x - self->p.x;
  float dy = ms.str->p.y - self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) - self->h ) + 720.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

static int ozStrPitchFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  float dx = ms.str->p.x - ms.self->p.x;
  float dy = ms.str->p.y - ms.self->p.y;
  float dz = ms.str->p.z - ms.self->p.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

static int ozStrPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  float dx = ms.str->p.x - eye.x;
  float dy = ms.str->p.y - eye.y;
  float dz = ms.str->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

static int ozStrBindAllOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( ms.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.objects.clear();
  ms.structs.clear();
  collider.getOverlaps( aabb, &ms.objects, &ms.structs );
  ms.objIndex = 0;
  ms.strIndex = 0;
  return 0;
}

static int ozStrBindStrOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( ms.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.structs.clear();
  collider.getOverlaps( aabb, null, &ms.structs );
  ms.strIndex = 0;
  return 0;
}

static int ozStrBindObjOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( ms.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.objects.clear();
  collider.getOverlaps( aabb, &ms.objects, null );
  ms.objIndex = 0;
  return 0;
}

/*
 * Object
 */

static int ozObjBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }
  ms.obj = orbis.objects[index];
  return 0;
}

static int ozObjBindPilot( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  ms.obj = vehicle->pilot == -1 ? null : orbis.objects[vehicle->pilot];
  return 0;
}

static int ozObjBindSelf( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.self;
  return 0;
}

static int ozObjBindUser( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.user;
  return 0;
}

static int ozObjBindNext( lua_State* l )
{
  ARG( 0 );

  if( ms.objIndex < ms.objects.length() ) {
    ms.obj = ms.objects[ms.objIndex];
    ++ms.objIndex;
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

static int ozObjIsNull( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj == null );
  return 1;
}

static int ozObjIsSelf( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj == ms.self );
  return 1;
}

static int ozObjIsUser( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj == ms.user );
  return 1;
}

static int ozObjIsCut( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ms.obj->cell == null );
  return 1;
}

static int ozObjIsBrowsable( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ( ms.obj->flags & Object::BROWSABLE_BIT ) );
  return 1;
}

static int ozObjIsDynamic( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ( ms.obj->flags & Object::DYNAMIC_BIT ) );
  return 1;
}

static int ozObjIsItem( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ( ms.obj->flags & Object::ITEM_BIT ) );
  return 1;
}

static int ozObjIsWeapon( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ( ms.obj->flags & Object::WEAPON_BIT ) );
  return 1;
}

static int ozObjIsBot( lua_State* l )
{
  ARG( 0 );

  const Bot* bot = static_cast<const Bot*>( ms.obj );
  pushbool( bot != null && ( bot->flags & Object::BOT_BIT ) && !( bot->state & Bot::DEAD_BIT ) );
  return 1;
}

static int ozObjIsVehicle( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.obj != null && ( ms.obj->flags & Object::VEHICLE_BIT ) );
  return 1;
}

static int ozObjGetIndex( lua_State* l )
{
  ARG( 0 );

  if( ms.obj == null ) {
    pushint( -1 );
  }
  else {
    pushint( ms.obj->index );
  }
  return 1;
}

static int ozObjGetPos( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  if( ms.obj->cell == null ) {
    hard_assert( ms.obj->flags & Object::DYNAMIC_BIT );

    const Dynamic* dyn = static_cast<const Dynamic*>( ms.obj );

    if( dyn->parent != -1 ) {
      Object* parent = orbis.objects[dyn->parent];

      if( parent != null ) {
        pushfloat( parent->p.x );
        pushfloat( parent->p.y );
        pushfloat( parent->p.z );
        return 3;
      }
    }
  }

  pushfloat( ms.obj->p.x );
  pushfloat( ms.obj->p.y );
  pushfloat( ms.obj->p.z );
  return 3;
}

static int ozObjSetPos( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  ms.obj->p.x = tofloat( 1 );
  ms.obj->p.y = tofloat( 2 );
  ms.obj->p.z = tofloat( 3 );

  ms.obj->flags &= ~Object::MOVE_CLEAR_MASK;
  return 0;
}

static int ozObjAddPos( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  ms.obj->p.x += tofloat( 1 );
  ms.obj->p.y += tofloat( 2 );
  ms.obj->p.z += tofloat( 3 );

  ms.obj->flags &= ~Object::MOVE_CLEAR_MASK;
  return 0;
}

static int ozObjGetDim( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushfloat( ms.obj->dim.x );
  pushfloat( ms.obj->dim.y );
  pushfloat( ms.obj->dim.z );
  return 3;
}

static int ozObjGetFlags( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  int mask = toint( 1 );
  pushbool( ms.obj->flags & mask );
  return 1;
}

static int ozObjGetHeading( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushint( ms.obj->flags & Object::HEADING_MASK );
  return 1;
}

static int ozObjGetClassName( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushstring( ms.obj->clazz->name );
  return 1;
}

static int ozObjGetLife( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushfloat( ms.obj->life );
  return 1;
}

static int ozObjSetLife( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  ms.obj->life = clamp( tofloat( 1 ), 0.0f, ms.obj->clazz->life );
  return 0;
}

static int ozObjAddLife( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  ms.obj->life = clamp( ms.obj->life + tofloat( 1 ),
                         0.0f,
                         ms.obj->clazz->life );
  return 0;
}

static int ozObjAddEvent( lua_State* l )
{
  ARG( 2 );
  OBJ_NOT_NULL();

  int   id        = toint( 1 );
  float intensity = tofloat( 2 );

  if( id >= 0 && intensity < 0.0f ) {
    ERROR( "event intensity for sounds (id >= 0) has to be > 0.0" );
  }
  ms.obj->addEvent( id, intensity );
  return 0;
}

static int ozObjBindItems( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  ms.objects.clear();
  foreach( item, ms.obj->items.citer() ) {
    ms.objects.add( orbis.objects[*item] );
  }
  ms.objIndex = 0;
  return 0;
}

static int ozObjAddItem( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  if( ms.obj->items.length() == ms.obj->clazz->nItems ) {
    pushbool( false );
    return 1;
  }

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }

  Dynamic* obj = static_cast<Dynamic*>( orbis.objects[index] );
  if( !( obj->flags & Object::ITEM_BIT ) ) {
    ERROR( "object is not an item" );
  }
  if( obj->cell == null ) {
    ERROR( "object is already cut" );
  }

  obj->parent = ms.obj->index;
  ms.obj->items.add( obj->index );
  synapse.cut( obj );

  pushbool( true );
  return 0;
}

static int ozObjRemoveItem( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  int item = toint( 1 );
  if( uint( item ) >= uint( ms.obj->items.length() ) ) {
    ERROR( "invalid item number" );
  }

  int index = ms.obj->items[item];
  Dynamic* dyn = static_cast<Dynamic*>( orbis.objects[index] );

  if( dyn != null ) {
    ms.obj->items.remove( item );
    synapse.remove( dyn );
  }
  return 0;
}

static int ozObjRemoveAllItems( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  foreach( item, ms.obj->items.citer() ) {
    Dynamic* dyn = static_cast<Dynamic*>( orbis.objects[*item] );

    if( dyn != null ) {
      synapse.remove( dyn );
    }
  }

  ms.obj->items.clear();
  return 0;
}

static int ozObjEnableUpdate( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  if( tobool( 1 ) ) {
    ms.obj->flags |= Object::UPDATE_FUNC_BIT;
  }
  else {
    ms.obj->flags &= ~Object::UPDATE_FUNC_BIT;
  }
  return 0;
}

static int ozObjDamage( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  ms.obj->damage( tofloat( 1 ) );
  return 0;
}

static int ozObjDestroy( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  ms.obj->life = 0.0f;
  return 0;
}

static int ozObjQuietDestroy( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  ms.obj->flags |= Object::DESTROYED_BIT;
  return 0;
}

static int ozObjVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Vec3 vec = ms.obj->p - ms.self->p;

  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

static int ozObjVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vec = ms.obj->p - eye;

  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

static int ozObjDirectionFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Vec3 dir = ~( ms.obj->p - ms.self->p );

  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

static int ozObjDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   dir = ~( ms.obj->p - eye );

  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

static int ozObjDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  pushfloat( !( ms.obj->p - ms.self->p ) );
  return 1;
}

static int ozObjDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  pushfloat( !( ms.obj->p - eye ) );
  return 1;
}

static int ozObjHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  float dx = ms.obj->p.x - ms.self->p.x;
  float dy = ms.obj->p.y - ms.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

static int ozObjRelativeHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  float dx = ms.obj->p.x - self->p.x;
  float dy = ms.obj->p.y - self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) - self->h ) + 720.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

static int ozObjPitchFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  float dx = ms.obj->p.x - ms.self->p.x;
  float dy = ms.obj->p.y - ms.self->p.y;
  float dz = ms.obj->p.z - ms.self->p.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

static int ozObjPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  float dx = ms.obj->p.x - eye.x;
  float dy = ms.obj->p.y - eye.y;
  float dz = ms.obj->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

static int ozObjIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Point3 eye    = Point3( ms.self->p.x, ms.self->p.y, ms.self->p.z );
  Vec3   vector = ms.obj->p - eye;

  collider.translate( eye, vector, ms.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

static int ozObjIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye    = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vector = ms.obj->p - eye;

  collider.translate( eye, vector, ms.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

static int ozObjBindAllOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( ms.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.objects.clear();
  ms.structs.clear();
  collider.getOverlaps( aabb, &ms.objects, &ms.structs );
  ms.objIndex = 0;
  ms.strIndex = 0;
  return 0;
}

static int ozObjBindStrOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( ms.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.structs.clear();
  collider.getOverlaps( aabb, null, &ms.structs );
  ms.strIndex = 0;
  return 0;
}

static int ozObjBindObjOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( ms.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  ms.objects.clear();
  collider.getOverlaps( aabb, &ms.objects, null );
  ms.objIndex = 0;
  return 0;
}

/*
 * Dynamic object
 */

static int ozDynBindParent( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  if( dyn->parent != -1 && orbis.objects[dyn->parent] != null ) {
    ms.obj = orbis.objects[dyn->parent];
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

static int ozDynGetVelocity( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->velocity.x );
  pushfloat( dyn->velocity.y );
  pushfloat( dyn->velocity.z );
  return 3;
}

static int ozDynGetMomentum( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->momentum.x );
  pushfloat( dyn->momentum.y );
  pushfloat( dyn->momentum.z );
  return 3;
}

static int ozDynSetMomentum( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  dyn->flags &= ~Object::DISABLED_BIT;
  dyn->momentum.x = tofloat( 1 );
  dyn->momentum.y = tofloat( 2 );
  dyn->momentum.z = tofloat( 3 );
  return 0;
}

static int ozDynAddMomentum( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  dyn->flags &= ~Object::DISABLED_BIT;
  dyn->momentum.x += tofloat( 1 );
  dyn->momentum.y += tofloat( 2 );
  dyn->momentum.z += tofloat( 3 );
  return 0;
}

static int ozDynGetMass( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->mass );
  return 1;
}

static int ozDynGetLift( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->lift );
  return 1;
}

/*
 * Weapon
 */

static int ozWeaponGetDefaultRounds( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  pushint( weaponClazz->nRounds );
  return 1;
}

static int ozWeaponGetRounds( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  pushint( weapon->nRounds );
  return 1;
}

static int ozWeaponSetRounds( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  weapon->nRounds = clamp( toint( 1 ), -1, weaponClazz->nRounds );
  return 1;
}

static int ozWeaponAddRounds( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  if( weapon->nRounds != -1 ) {
    weapon->nRounds = min( weapon->nRounds + toint( 1 ), weaponClazz->nRounds );
  }
  return 1;
}

static int ozWeaponReload( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  weapon->nRounds = weaponClazz->nRounds;
  return 1;
}

/*
 * Bot
 */

static int ozBotBindPilot( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  ms.obj = vehicle->pilot == -1 ? null : orbis.objects[vehicle->pilot];
  return 0;
}

static int ozBotGetName( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushstring( bot->name );
  return 1;
}

static int ozBotSetName( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->name = tostring( 1 );
  return 0;
}

static int ozBotGetMindFunc( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushstring( bot->mindFunc );
  return 1;
}

static int ozBotSetMindFunc( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->mindFunc = tostring( 1 );
  return 0;
}

static int ozBotGetState( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  int mask = toint( 1 );
  pushbool( bot->state & mask );
  return 1;
}

static int ozBotGetEyePos( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( bot->p.x );
  pushfloat( bot->p.y );
  pushfloat( bot->p.z + bot->camZ );
  return 3;
}

static int ozBotGetH( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( Math::deg( bot->h ) );
  return 1;
}

static int ozBotSetH( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->h = Math::rad( tofloat( 1 ) );
  bot->h = Math::fmod( bot->h + Math::TAU, Math::TAU );
  return 0;
}

static int ozBotAddH( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->h += Math::rad( tofloat( 1 ) );
  bot->h = Math::fmod( bot->h + Math::TAU, Math::TAU );
  return 0;
}

static int ozBotGetV( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( Math::deg( bot->v ) );
  return 1;
}

static int ozBotSetV( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->v = Math::rad( tofloat( 1 ) );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );
  return 0;
}

static int ozBotAddV( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->v += Math::rad( tofloat( 1 ) );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );
  return 0;
}

static int ozBotGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
  Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  pushfloat( -hvsc[4] );
  pushfloat(  hvsc[5] );
  pushfloat( -hvsc[3] );

  return 3;
}

static int ozBotGetStamina( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( bot->stamina );
  return 1;
}

static int ozBotSetStamina( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  bot->stamina = clamp( tofloat( 1 ), 0.0f, clazz->stamina );
  return 0;
}

static int ozBotAddStamina( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  bot->stamina = clamp( bot->stamina + tofloat( 1 ), 0.0f, clazz->stamina );
  return 0;
}

static int ozBotActionForward( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_FORWARD;
  return 0;
}

static int ozBotActionBackward( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_BACKWARD;
  return 0;
}

static int ozBotActionRight( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_RIGHT;
  return 0;
}

static int ozBotActionLeft( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_LEFT;
  return 0;
}

static int ozBotActionJump( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_JUMP;
  return 0;
}

static int ozBotActionCrouch( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_CROUCH;
  return 0;
}

static int ozBotActionUse( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_USE;
  return 0;
}

static int ozBotActionTake( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_TAKE;
  return 0;
}

static int ozBotActionGrab( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_GRAB;
  return 0;
}

static int ozBotActionThrow( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_THROW;
  return 0;
}

static int ozBotActionAttack( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_ATTACK;
  return 0;
}

static int ozBotActionExit( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_EXIT;
  return 0;
}

static int ozBotActionEject( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_EJECT;
  return 0;
}

static int ozBotActionSuicide( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->actions |= Bot::ACTION_SUICIDE;
  return 0;
}

static int ozBotIsRunning( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushbool( bot->state & Bot::RUNNING_BIT );
  return 1;
}

static int ozBotSetRunning( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  if( tobool( 1 ) ) {
    bot->state |= Bot::RUNNING_BIT;
  }
  else {
    bot->state &= ~Bot::RUNNING_BIT;
  }
  return 0;
}

static int ozBotToggleRunning( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->state ^= Bot::RUNNING_BIT;
  return 0;
}

static int ozBotSetGesture( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT | Bot::GESTURE4_BIT );
  bot->state |= toint( 1 );
  return 0;
}

static int ozBotSetWeaponItem( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  int item = toint( 1 );
  if( item == -1 ) {
    bot->weapon = -1;
  }
  else {
    if( uint( item ) >= uint( ms.obj->items.length() ) ) {
      ERROR( "invalid item number" );
    }

    int index = ms.obj->items[item];
    Weapon* weapon = static_cast<Weapon*>( orbis.objects[index] );

    if( weapon == null ) {
      pushbool( false );
      return 1;
    }

    if( !( weapon->flags & Object::WEAPON_BIT ) ) {
      ERROR( "object is not a weapon" );
    }

    const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );
    if( clazz->allowedUsers.contains( bot->clazz ) ) {
      bot->weapon = index;
    }
  }

  pushbool( true );
  return 1;
}

static int ozBotHeal( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->heal();
  return 0;
}

static int ozBotRearm( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->rearm();
  return 0;
}

static int ozBotKill( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  bot->kill();
  return 0;
}

static int ozBotIsVisibleFromSelfEyeToEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  OBJ_BOT();
  SELF_BOT();

  Point3 eye    = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vector = Point3( bot->p.x, bot->p.y, bot->p.z + bot->camZ ) - eye;

  collider.translate( eye, vector, ms.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

/*
 * Vehicle
 */

static int ozVehicleGetH( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  pushfloat( Math::deg( vehicle->h ) );
  return 1;
}

static int ozVehicleSetH( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  vehicle->h = Math::rad( tofloat( 1 ) );
  vehicle->h = Math::fmod( vehicle->h + Math::TAU, Math::TAU );

  vehicle->rot = Quat::rotZXZ( vehicle->h, vehicle->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddH( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  vehicle->h += Math::rad( tofloat( 1 ) );
  vehicle->h = Math::fmod( vehicle->h + Math::TAU, Math::TAU );

  vehicle->rot = Quat::rotZXZ( vehicle->h, vehicle->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleGetV( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  pushfloat( Math::deg( vehicle->v ) );
  return 1;
}

static int ozVehicleSetV( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  vehicle->v = Math::rad( tofloat( 1 ) );
  vehicle->v = clamp( vehicle->v, 0.0f, Math::TAU / 2.0f );

  vehicle->rot = Quat::rotZXZ( vehicle->h, vehicle->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddV( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  vehicle->v += Math::rad( tofloat( 1 ) );
  vehicle->v = clamp( vehicle->v, 0.0f, Math::TAU / 2.0f );

  vehicle->rot = Quat::rotZXZ( vehicle->h, vehicle->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( vehicle->h, &hvsc[0], &hvsc[1] );
  Math::sincos( vehicle->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  pushfloat( -hvsc[4] );
  pushfloat(  hvsc[5] );
  pushfloat( -hvsc[3] );

  return 3;
}

static int ozVehicleEmbarkPilot( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  if( vehicle->pilot != -1 ) {
    ERROR( "vehicle already has a pilot" );
  }

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid bot index" );
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[index] );
  if( !( bot->flags & Object::BOT_BIT ) ) {
    ERROR( "object is not a bot" );
  }
  if( bot->cell == null ) {
    ERROR( "bot is already cut" );
  }

  vehicle->pilot = index;
  bot->enter( vehicle->index );
  return 0;
}

static int ozVehicleService( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  vehicle->service();
  return 0;
}

/*
 * Fragment
 */

static int ozFragBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.frags.length() ) ) {
    ERROR( "invalid frag index" );
  }
  ms.frag = orbis.frags[index];
  return 0;
}

static int ozFragIsNull( lua_State* l )
{
  ARG( 0 );

  pushbool( ms.frag == null );
  return 1;
}

static int ozFragGetIndex( lua_State* l )
{
  ARG( 0 );

  if( ms.frag == null ) {
    pushint( -1 );
  }
  else {
    pushint( ms.frag->index );
  }
  return 1;
}

static int ozFragGetPos( lua_State* l )
{
  ARG( 0 );
  FRAG_NOT_NULL();

  pushfloat( ms.frag->p.x );
  pushfloat( ms.frag->p.y );
  pushfloat( ms.frag->p.z );
  return 3;
}

static int ozFragSetPos( lua_State* l )
{
  ARG( 3 );
  FRAG_NOT_NULL();

  ms.frag->p.x = tofloat( 1 );
  ms.frag->p.y = tofloat( 2 );
  ms.frag->p.z = tofloat( 3 );
  return 0;
}

static int ozFragAddPos( lua_State* l )
{
  ARG( 3 );
  FRAG_NOT_NULL();

  ms.frag->p.x += tofloat( 1 );
  ms.frag->p.y += tofloat( 2 );
  ms.frag->p.z += tofloat( 3 );
  return 0;
}

static int ozFragGetVelocity( lua_State* l )
{
  ARG( 0 );
  FRAG_NOT_NULL();

  pushfloat( ms.frag->velocity.x );
  pushfloat( ms.frag->velocity.y );
  pushfloat( ms.frag->velocity.z );
  return 3;
}

static int ozFragSetVelocity( lua_State* l )
{
  ARG( 3 );
  FRAG_NOT_NULL();

  ms.frag->velocity.x = tofloat( 1 );
  ms.frag->velocity.y = tofloat( 2 );
  ms.frag->velocity.z = tofloat( 3 );
  return 0;
}

static int ozFragAddVelocity( lua_State* l )
{
  ARG( 3 );
  FRAG_NOT_NULL();

  ms.frag->velocity.x += tofloat( 1 );
  ms.frag->velocity.y += tofloat( 2 );
  ms.frag->velocity.z += tofloat( 3 );
  return 0;
}

static int ozFragGetLife( lua_State* l )
{
  ARG( 0 );
  FRAG_NOT_NULL();

  pushfloat( ms.frag->life );
  return 1;
}

static int ozFragSetLife( lua_State* l )
{
  ARG( 1 );
  FRAG_NOT_NULL();

  ms.frag->life = tofloat( 1 );
  return 0;
}

static int ozFragAddLife( lua_State* l )
{
  ARG( 1 );
  FRAG_NOT_NULL();

  ms.frag->life += tofloat( 1 );
  return 0;
}

static int ozFragRemove( lua_State* l )
{
  ARG( 0 );
  FRAG_NOT_NULL();

  synapse.remove( ms.frag );
  ms.frag = null;
  return 0;
}
