0.3.1

- client
    * credits screen
- general
    * Win32 port fixed, bugfixes ...

0.3.0

- oz
    * more C++11 features
        + move semantics
        + defaulted & deleted functions
        + explicit overrides
        + new exception specification
        + limited use of initialiser lists
        + limited use of constexpr (float constants)
    * (D)List renamed to (D)Chain, (S)Vector renamed to (S)List
    * String class has more consistent members, parsing functions added
    * new Time class
    * new StackTrace class (split from System class)
    * System::bell()
    * OutputStream is readable
    * new BufferStream class (OutputStream with dynamic internal storage)
    * File class: file type and size setting changed, added support for NaCl local filesystems
    * new PFile class: version of File class to act as PhysicsFS wrapper
    * Mutex, Semaphore and Thread classes for high-level thread management
    * new JSON class for manipulating JSON files
    * Config class removed
    * Vec3, Vec4, Quat don't inherit from each other or Simd class
    * AABB & Bounds moved to liboz from matrix
    * clearmacros.hh header to undefine liboz macros that might conflict other libraries
    * can be compiled as a shared library
    * additional Linux dependency: PhysicsFS, optional dependency for Linux: libpulse-simple
    * better Win32 support, POSIX API replaced with Win32 API where possible
    * ported to NaCl
    * fully documented
    * ZLIB licence
- common
    * new layer, split from matrix
    * Span and Timer classes moved to common from matrix
    * new Lingua class, gettext-like system, but modularised and suitable for data package system
- matrix
    * BSPs, object classes, fragment pools, client imagos and audios are internally referenced by
      pointers or indices instead by strings, several lookup hashtables replaced by arrays
    * object attributes: tell what special abilities an object can have (nigh vision, binoculars,
      satnav, music player ...)
    * fragment pools: particle replacements; fragments can be configured like object classes, can
      have a graphic model assigned, etc.
    * keys: object can have assigned a key value, so it can lock/unlock matching structure entities
    * lava: can be used both in structures and terrain instead of sea
    * physics
        + object tracks lower entity: when an entity moves horizontally it also moves objects on top
        + sliding damage
        + configurable gravity
        + fine-tuning of constants and many small improvements and bugfixes
    * structures
        + crushing: structures fall into terrain, and crush objects in their way
        + bound objects: objects that are created together with a structure and destroyed with it;
          useful for defence turrets
        + air brushes to tag certain structure areas that should not be filled with water when the
          structure is under sea
        + sea brushes tag areas with the same content as terrain sea (including fog colour)
        + no more need for hull around BSPs
        + triggers: some entities can be activated; they can trigger themselves or other entities
        + entities can be locked/unlocked with appropriate key
        + manual doors
        + elevators
- client
    * smooth loading: model loading split to preloading (mapping file to memory) that is run in a
      new thread and upload to graphic card that has to be synchronous with rendering
    * textures have 3 components: diffuse (base colour), masks (per-pixel specular and emission
      masks) and normal map
    * configurable fog colour for both terrain and structure water/lava
    * Mesh class
        + meshes are not rendered immediately but lists are created for each mesh and all instances
          of a mesh are rendered together
        + MD2-like vertex animation integrated into Mesh class
    * offscreen rendering support
    * postprocessing, bloom postprocess effect
    * Camera class & proxies
        + smoothing of movement and rotation
        + colour transformation matrix, used for visual effects like injuries, night vision,
          black-and-white sequences ...
        + camera bob, walk/run animations and footsteps sounds are synchronised to a counter in Bot
          class
        + CinematicProxy class for scripted camera movements, colours, music track setting ...
    * *Audio classes
        + footstep and swim sounds
        + filtering of high-frequency events
        + velocity is set for sound sources and listener
    * Sound class
        + sounds are played asynchronously, parallel to rendering
        + asynchronous streaming of tracks
        + support for music in MP3 and AAC formats
        + a user-specified directory can be searched for music
    * resizable main window
    * new main menu, mission selection menu
- build
    * prebuilding entirely split from the runtime part of the engine
    * SDL_image replaced by FreeImage, wider range of image formats supported as prebuilding source
    * 16-bit colour components for terrain heightmap
- tools
    * ozGettext tool for extracting structure, entity, object names and strings from mission scripts
      for translation
    * ozManifest for generating package manifests for NaCl port
    * ozSineSample for generating src/oz/bellSample.inc file
- general
    * PhysicsFS used for accessing game data, so they can be loaded directly from archives
    * game data can be modularised into packages
    * Lua API redesigned
    * Lua classes share API code - no more code triplication of Lua API implementations
    * ported to SDL 2.0
    * ported to NaCl
    * PKGBUILD for Arch Linux and RPM spec file
