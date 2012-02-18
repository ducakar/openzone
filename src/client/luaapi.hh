struct ClientLuaState
{
  String mission;
  Lingua missionLingua;
};

static ClientLuaState cs;

/*
 * General functions
 */

static int ozGettext( lua_State* l )
{
  ARG( 1 );

  pushstring( cs.missionLingua.get( tostring( 1 ) ) );
  return 1;
}

/*
 * QuestList
 */

static int ozQuestAdd( lua_State* l )
{
  ARG( 5 );

  questList.quests.add( Quest( tostring( 1 ),
                               tostring( 2 ),
                               Point3( tofloat( 3 ), tofloat( 4 ), tofloat( 5 ) ),
                               Quest::PENDING ) );

  pushint( questList.quests.length() - 1 );
  return 1;
}

static int ozQuestEnd( lua_State* l )
{
  ARG( 2 );

  int id = toint( 1 );
  if( uint( id ) >= uint( questList.quests.length() ) ) {
    ERROR( "invalid quest id" );
  }

  questList.quests[id].state = tobool( 2 ) ? Quest::SUCCESSFUL : Quest::FAILED;
  return 0;
}

/*
 * Camera
 */

static int ozCameraGetPos( lua_State* l )
{
  ARG( 0 );

  pushfloat( camera.p.x );
  pushfloat( camera.p.y );
  pushfloat( camera.p.z );

  return 3;
}

static int ozCameraGetDest( lua_State* l )
{
  ARG( 0 );

  pushfloat( camera.newP.x );
  pushfloat( camera.newP.y );
  pushfloat( camera.newP.z );

  return 3;
}

static int ozCameraGetH( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( camera.h ) );
  return 1;
}

static int ozCameraSetH( lua_State* l )
{
  ARG( 1 );

  camera.h = Math::rad( tofloat( 1 ) );
  return 0;
}

static int ozCameraGetV( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( camera.v ) );
  return 1;
}

static int ozCameraSetV( lua_State* l )
{
  ARG( 1 );

  camera.v = Math::rad( tofloat( 1 ) );
  return 0;
}

static int ozCameraMoveTo( lua_State* l )
{
  ARG( 3 );

  Point3 pos = Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) );
  camera.move( pos );

  return 3;
}

static int ozCameraWarpTo( lua_State* l )
{
  ARG( 3 );

  Point3 pos = Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) );
  camera.warp( pos );

  return 3;
}

static int ozCameraIncarnate( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[index] );
  if( bot == null ) {
    ERROR( "object is null" );
  }
  else if( !( bot->flags & Object::BOT_BIT ) ) {
    ERROR( "object is not a bot" );
  }

  camera.setBot( bot );
  camera.setState( Camera::BOT );

  return 0;
}

static int ozCameraAllowReincarnation( lua_State* l )
{
  ARG( 1 );

  camera.allowReincarnation = tobool( 1 );
  return 0;
}

/*
 * Profile
 */

static int ozProfileGetPlayerName( lua_State* l )
{
  ARG( 0 );

  pushstring( profile.playerName );
  return 1;
}
