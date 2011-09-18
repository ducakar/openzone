/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BSP.hpp"

#include "client/Context.hpp"
#include "client/Compiler.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"
#include "client/Audio.hpp"

#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

namespace oz
{
namespace client
{

#ifdef OZ_SDK
  int BSP::nTextures;
  int BSP::nModels;
  int BSP::nVertices;
  int BSP::nIndices;
  int BSP::nFaces;

  DArray<BSP::QBSPTexture> BSP::textures;
  DArray<BSP::QBSPModel>   BSP::models;
  DArray<BSP::QBSPVertex>  BSP::vertices;
  DArray<int>              BSP::indices;
  DArray<BSP::QBSPFace>    BSP::faces;

  void BSP::loadQBSP( const char* path )
  {
    String rcFile = path + String( ".rc" );
    String bspFile = path + String( ".bsp" );

    Config bspConfig;
    if( !bspConfig.load( rcFile ) ) {
      throw Exception( "BSP config file cannot be read" );
    }

    float scale = bspConfig.get( "scale", 0.01f );
    float maxDim = bspConfig.get( "maxDim", Math::INF );

    translator.shaderIndex( "mesh" );
    translator.shaderIndex( "bigMesh" );

    if( Math::isNaN( scale ) ) {
      throw Exception( "BSP scale is NaN" );
    }

    FILE* file = fopen( bspFile, "rb" );
    if( file == null ) {
      throw Exception( "BSP file not found" );
    }

    QBSPHeader header;
    fread( &header, sizeof( QBSPHeader ), 1, file );

    if( header.id[0] != 'I' || header.id[1] != 'B' || header.id[2] != 'S' || header.id[3] != 'P' ||
        header.version != 46 )
    {
      throw Exception( "BSP file has invalid format/version figerprint" );
    }

    QBSPLump lumps[QBSPLump::MAX];
    fread( lumps, sizeof( QBSPLump ), QBSPLump::MAX, file );

    nTextures = lumps[QBSPLump::TEXTURES].length / int( sizeof( QBSPTexture ) );
    textures.alloc( nTextures );
    fseek( file, lumps[QBSPLump::TEXTURES].offset, SEEK_SET );
    fread( textures, sizeof( QBSPTexture ), size_t( nTextures ), file );

    nModels = lumps[QBSPLump::MODELS].length / int( sizeof( QBSPModel ) );
    models.alloc( nModels );
    fseek( file, lumps[QBSPLump::MODELS].offset, SEEK_SET );
    fread( models, sizeof( QBSPModel ), size_t( nModels ), file );

    nVertices = lumps[QBSPLump::VERTICES].length / int( sizeof( QBSPVertex ) );
    vertices.alloc( nVertices );
    fseek( file, lumps[QBSPLump::VERTICES].offset, SEEK_SET );
    fread( vertices, sizeof( QBSPVertex ), size_t( nVertices ), file );

    foreach( vertex, vertices.iter() ) {
      vertex->p[0] *= scale;
      vertex->p[1] *= scale;
      vertex->p[2] *= scale;
    }

    nIndices = lumps[QBSPLump::INDICES].length / int( sizeof( int ) );
    indices.alloc( nIndices );
    fseek( file, lumps[QBSPLump::INDICES].offset, SEEK_SET );
    fread( indices, sizeof( int ), size_t( nIndices ), file );

    nFaces = lumps[QBSPLump::FACES].length / int( sizeof( QBSPFace ) );
    faces.alloc( nFaces );
    fseek( file, lumps[QBSPLump::FACES].offset, SEEK_SET );
    fread( faces, sizeof( QBSPFace ), size_t( nFaces ), file );

    foreach( face, faces.iter() ) {
      for( int i = 0; i < face->nVertices; ++i ) {
        const QBSPVertex& vertex = vertices[face->firstVertex + i];

        if( vertex.p[0] < -maxDim || vertex.p[0] > +maxDim ||
            vertex.p[1] < -maxDim || vertex.p[1] > +maxDim ||
            vertex.p[2] < -maxDim || vertex.p[2] > +maxDim )
        {
          face->nIndices = 0;
          break;
        }
      }
    }

    fclose( file );

    // to disable warnings
    bspConfig.get( "life", 0.0f );

    if( nModels != 1 ) {
      hard_assert( nModels <= 99 );
      char keyBuffer[] = "model  ";

      for( int i = 0; i < nModels - 1; ++i ) {
        keyBuffer[5] = char( '0' + i / 10 );
        keyBuffer[6] = char( '0' + i % 10 );
        String keyName = keyBuffer;

        bspConfig.get( keyName + ".move.x", 0.0f );
        bspConfig.get( keyName + ".move.y", 0.0f );
        bspConfig.get( keyName + ".move.z", 0.0f );

        bspConfig.get( keyName + ".slideTime", 0.0f );
        bspConfig.get( keyName + ".type", "" );

        bspConfig.get( keyName + ".margin", 0.0f );
        bspConfig.get( keyName + ".timeout", 0.0f );

        bspConfig.get( keyName + ".openSample", "" );
        bspConfig.get( keyName + ".closeSample", "" );
      }
    }
  }

  void BSP::freeQBSP()
  {
    textures.dealloc();
    models.dealloc();
    vertices.dealloc();
    indices.dealloc();
    faces.dealloc();
  }

  void BSP::optimise()
  {
    log.println( "Optimising BSP model {" );
    log.indent();

    // remove faces that lay out of boundaries
    for( int i = 0; i < nFaces; ) {
      hard_assert( faces[i].nVertices > 0 && faces[i].nIndices >= 0 );

      if( faces[i].nIndices != 0 ) {
        ++i;
        continue;
      }

      aRemove<QBSPFace>( faces, i, nFaces );
      --nFaces;
      log.print( "outside face removed " );

      // adjust face references
      for( int j = 0; j < nModels; ++j ) {
        if( i < models[j].firstFace ) {
          --models[j].firstFace;
        }
        else if( i < models[j].firstFace + models[j].nFaces ) {
          hard_assert( models[j].nFaces > 0 );

          --models[j].nFaces;
        }
      }
      log.printEnd();
    }

    log.unindent();
    log.println( "}" );
  }

  void BSP::save( const char* path )
  {
    int flags = 0;

    Vector<MeshData> meshes( nModels );

    for( int i = 0; i < nModels; ++i ) {
      compiler.beginMesh();
      compiler.enable( CAP_UNIQUE );
      compiler.enable( CAP_CW );

      for( int j = 0; j < models[i].nFaces; ++j ) {
        const QBSPFace& face = faces[ models[i].firstFace + j ];

        const QBSPTexture& texture = textures[face.texture];
        String name = texture.name;

        if( name.length() <= 12 || name.equals( "textures/NULL" ) ||
            ( texture.flags & QBSP_LADDER_FLAG_BIT ) )
        {
          name = "";
        }
        else {
          name = name.substring( 12 );
        }

        if( texture.type & QBSP_WATER_TYPE_BIT ) {
          compiler.material( GL_DIFFUSE, 0.75f );
          compiler.material( GL_SPECULAR, 0.5f );
          flags |= Mesh::ALPHA_BIT;
        }
        else if( texture.flags & QBSP_GLASS_FLAG_BIT ) {
          compiler.material( GL_DIFFUSE, 0.25f );
          compiler.material( GL_SPECULAR, 2.0f );
          flags |= Mesh::ALPHA_BIT;
        }
        else {
          compiler.material( GL_DIFFUSE, 1.0f );
          compiler.material( GL_SPECULAR, 0.0f );
          flags |= Mesh::SOLID_BIT;
        }

        compiler.texture( name );
        compiler.begin( GL_TRIANGLES );

        for( int k = 0; k < face.nIndices; ++k ) {
          const QBSPVertex& vertex = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

          compiler.texCoord( vertex.texCoord );
          compiler.normal( face.normal );
          compiler.vertex( vertex.p );
        }

        if( texture.type & QBSP_WATER_TYPE_BIT ) {
          for( int k = face.nIndices - 1; k >= 0; --k ) {
            const QBSPVertex& vertex = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

            compiler.texCoord( vertex.texCoord );
            compiler.normal( -face.normal[0], -face.normal[1], -face.normal[2] );
            compiler.vertex( vertex.p );
          }
        }

        compiler.end();
      }

      compiler.endMesh();

      meshes.add();
      compiler.getMeshData( &meshes.last() );
    }

    OutputStream os = buffer.outputStream();

    os.writeInt( flags );
    os.writeInt( nModels );

    foreach( mesh, meshes.citer() ) {
      mesh->write( &os, false );
    }

    log.print( "Dumping BSP model to '%s' ...", path );

    buffer.write( path, os.length() );

    log.printEnd( " OK" );
  }
#endif

  void BSP::playSound( const Struct::Entity* entity, int sample ) const
  {
    hard_assert( uint( sample ) < uint( translator.sounds.length() ) );

    Bounds bounds = *entity->model;
    Point3 localPos = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );
    Point3 p = entity->str->toAbsoluteCS( localPos + entity->offset );

    uint srcId;

    alGenSources( 1, &srcId );
    if( alGetError() != AL_NO_ERROR ) {
      log.println( "AL: Too many sources" );
      return;
    }

    alSourcei( srcId, AL_BUFFER, int( context.sounds[sample].id ) );
    alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );
    alSourcef( srcId, AL_ROLLOFF_FACTOR, Audio::ROLLOFF_FACTOR );

    alSourcefv( srcId, AL_POSITION, p );
    alSourcef( srcId, AL_GAIN, 1.0f );
    alSourcePlay( srcId );

    context.addSource( srcId, sample );

    OZ_AL_CHECK_ERROR();
  }

  void BSP::playContSound( const Struct::Entity* entity, int sample ) const
  {
    hard_assert( uint( sample ) < uint( translator.sounds.length() ) );

    const Struct* str = entity->str;
    // we can have at most 100 models per BSP, so stride 128 should do
    int key = str->index * 128 + int( entity - str->entities );

    Bounds bounds = *entity->model;
    Point3 localPos = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );
    Point3 p = entity->str->toAbsoluteCS( localPos + entity->offset );

    Context::ContSource* contSource = context.bspSources.find( key );

    if( contSource == null ) {
      uint srcId;

      alGenSources( 1, &srcId );
      if( alGetError() != AL_NO_ERROR ) {
        log.println( "AL: Too many sources" );
        return;
      }

      Bounds bounds = *entity->model;
      Point3 p = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );

      p = entity->str->toAbsoluteCS( p + entity->offset );

      alSourcei( srcId, AL_BUFFER, int( context.sounds[sample].id ) );
      alSourcei( srcId, AL_LOOPING, AL_TRUE );
      alSourcef( srcId, AL_ROLLOFF_FACTOR, 0.25f );

      alSourcefv( srcId, AL_POSITION, p );
      alSourcef( srcId, AL_GAIN, 1.0f );
      alSourcePlay( srcId );

      context.addBSPSource( srcId, sample, key );
    }
    else {
      alSourcefv( contSource->id, AL_POSITION, p );

      contSource->isUpdated = true;
    }

    OZ_AL_CHECK_ERROR();
  }

#ifdef OZ_SDK
  void BSP::prebuild( const char* name_ )
  {
    String name = name_;

    log.println( "Prebuilding Quake 3 BSP model '%s' {", name_ );
    log.indent();

    loadQBSP( "data/maps/" + name );
    optimise();
    save( "bsp/" + name + ".ozcBSP" );
    freeQBSP();

    log.unindent();
    log.println( "}" );
  }
#endif

  BSP::BSP( int id ) : bsp( orbis.bsps[id] ), flags( 0 ), isLoaded( false )
  {}

  BSP::~BSP()
  {
    if( !isLoaded ) {
      return;
    }

    log.println( "Unloading BSP model '%s' {", translator.bsps[bsp->id].name.cstr() );
    log.indent();

    foreach( mesh, meshes.iter() ) {
      mesh->unload();
    }

    for( int i = 0; i < bsp->nModels; ++i ) {
      if( bsp->models[i].openSample != -1 ) {
        context.releaseSound( bsp->models[i].openSample );
      }
      if( bsp->models[i].closeSample != -1 ) {
        context.releaseSound( bsp->models[i].closeSample );
      }
      if( bsp->models[i].frictSample != -1 ) {
        context.releaseSound( bsp->models[i].frictSample );
      }
    }

    log.unindent();
    log.println( "}" );
  }

  void BSP::load()
  {
    hard_assert( bsp != null );

    const String& name = translator.bsps[bsp->id].name;

    log.println( "Loading BSP model '%s' {", name.cstr() );
    log.indent();

    if( !buffer.read( "bsp/" + name + ".ozcBSP" ) ) {
      throw Exception( "BSP loading failed" );
    }

    InputStream is = buffer.inputStream();

    flags = is.readInt();

    int nMeshes = is.readInt();

    meshes.alloc( nMeshes );
    foreach( mesh, meshes.iter() ) {
      mesh->load( &is, GL_STATIC_DRAW );
    }

    for( int i = 0; i < bsp->nModels; ++i ) {
      if( bsp->models[i].openSample != -1 ) {
        context.requestSound( bsp->models[i].openSample );
      }
      if( bsp->models[i].closeSample != -1 ) {
        context.requestSound( bsp->models[i].closeSample );
      }
      if( bsp->models[i].frictSample != -1 ) {
        context.requestSound( bsp->models[i].frictSample );
      }
    }

    log.unindent();
    log.println( "}" );

    isLoaded = true;
  }

  void BSP::draw( const Struct* str, int mask ) const
  {
    mask &= flags;

    if( mask == 0 ) {
      return;
    }

    shader.use( shader.isInWater ? shader.mesh : shader.bigMesh );

    for( int i = 0; i < meshes.length(); ++i ) {
      const Vec3& entityPos = i == 0 ? Vec3::ZERO : str->entities[i - 1].offset;

      tf.push();
      tf.model.translate( entityPos );
      tf.apply();

      meshes[i].draw( mask );

      tf.pop();
    }
  }

  void BSP::play( const Struct* str ) const
  {
    for( int i = 0; i < str->nEntities; ++i ) {
      const Struct::Entity& entity = str->entities[i];

      if( entity.state == Struct::Entity::OPENING ) {
        if( entity.ratio == 0.0f && bsp->models[i].openSample != -1 ) {
          playSound( &entity, bsp->models[i].openSample );
        }
        if( bsp->models[i].frictSample != -1 ) {
          playContSound( &entity, bsp->models[i].frictSample );
        }
      }
      else if( entity.state == Struct::Entity::CLOSING ) {
        if( entity.ratio == 1.0f && bsp->models[i].closeSample != -1 ) {
          playSound( &entity, bsp->models[i].closeSample );
        }
        if( bsp->models[i].frictSample != -1 ) {
          playContSound( &entity, bsp->models[i].frictSample );
        }
      }
    }
  }

}
}
