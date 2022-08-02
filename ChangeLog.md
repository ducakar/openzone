# ChangeLog #

0.4.0

- ozCore
  * full use of C++17
    + null replaced by `nullptr` keyword
    + `constexpr` used for functions too
    + removed storage for `constexpr` constants
    + member initialisers used wherever possible
    + initialiser list constructors for containers
    + different `begin()` and `end()` iterator types
    + folded namespace declarations
  * larger and non-performance-critical inline functions moved to .cc files
  * global `abs()`, `min()`, `max()` and `clamp()`
  * new STL-like `Less` and `Hash` template classes used for sorting and hashing
  * container improvements
    + improved iterators: far less code duplication, no more `key()`/`value()` members
    + removed generic container functions (`iEquals`, `iCopy`, `iIndex` ...)
    + moved array functions (`aEquals`, `aCopy`, `aIndex` ...) to `Arrays` class
    + STL-like parameter order in `Arrays` functions
    + STL-like `begin()` and `end()` members on containers and iterators
    + `(C)Range` class to hold begin-end pair and respective `(c)range` functions to construct it
      from containers, arrays etc.
    + removed `Array` and `DArray` template classes
    + new `Set` template class: `Map` with arbitrary elements instead of key-value pairs
    + new `HashMap` template class: merger of `HashIndex` & `HashString` with an arbitrary key type
    + new `HashSet` template class: hash table of arbitrary elements instead of key-value pairs
    + new `Heap` template class implementing binary min-heap
    + more inheritance between classes and far less code duplication (e.g. `List` -> `Set` -> `Map`)
  * remove `Alloc` class
  * new `PoolAlloc` class, non-template version of `Pool` which now extends `PoolAlloc`
  * `String`
    + buffer and baseBuffer in union, `String` class size 48 -> 32 bytes
    + functions for non-constant access (`operator []`, non-constant `begin()`, `end()` ...)
    + construction from numbers, new number parsing functions
  * exact SLERP implementation in `Quat`
  * new `Mat3` class for 3x3 matrices
  * SIMD support for linear algebra classes
  * new `SpinLock` and `CallOnce` classes added for threading
  * removed `Mutex` class, replaced by a new `Monitor` class representing a mutex + condition
    variable pair
  * new `Monitor` class
  * OpenSL (Android) back-end for `System::bell()`
  * linear algebra classes added as primitives to streams, `Log` and `Json`
  * `InputStream`, `OutputStream` and `BufferStream` merged into `Stream`
  * `Buffer` extends `List<char>`, supports zlib compression
  * `File`
    + redesigned, extends `String` class
    + removed metadata, now returned as `File::Info` by `stat()` call
    + remove mapping to memory
    + renamed `cp()` -> `copyTo()`, `mv()` -> `moveTo()` ... and made non-static
    + operators `/` and `/=` to concatenate paths
    + determines special user directories and executable path
  * `PFile` class merged into `File` class (VFS paths begin with `@`)
  * new `EnumMap` class for enum <-> string conversions
  * new `SharedLib` class for run-time linking
  * new `Profiler` class for measuring time spent in various code segments
  * new `Gettext` class for loading gettext translations
  * `Json` is more forgiving and simplified (implicit ctors, `get<NumType>()`, removed `asType()`)
  * new `Pepper` class that provides basic interfaces to PPAPI on NaCl
- ozEngine: new engine building blocks library
  * `GL` class for OpenGL utilities: error checks, DDS texture loader
  * `AL` class for OpenAL utilities: error checks, WAVE & Vorbis loader/decoder, Vorbis streamer
  * `Window` class based on `client::Window`
  * `Input` class for reading input from devices and key bindings
  * Shader compilation utility, understands `#include` directive
  * `Lua` class inspired by Selene: access to variables via `operator []`, function calls via `()`
- ozFactory: new builder building blocks library
  * `ImageBuilder` class for building DDS textures
  * `ModelBuilder` class for building OpenZone models using Assimp library
  * `TerraBuilder` class generates random terrain heightmaps and corresponding textures
- common
  * common Lua functionality split into `LuaCommon` which is base for `LuaMatrix`, `LuaNirvana` ...
  * common Lua scripts included by all VMs
  * Lua <-> JSON value interoperability
  * new Automaton class to facilitate implementations of finite-state machines
- matrix
  * `Orbis`: 1-based lists internally, proxy functions for accessing objects per index
  * negative `Object::Event` intensities for continuous sounds
  * melee attack
  * new vehicle types: turret, mech warrior
  * reverted from binary stream to JSON format for object classes and fragment pools
  * name lists replaced by name generator scripts
- nirvana
  * Technology graph
- ui
  * UI colours, fonts and layouts can be configured in `ui/style.json`
  * UI sounds
  * DDS format used for icons, X11 libxcursor format for cursors
  * buttons triggered on release
  * new `CheckBox` and `Slider` components
  * new `ModelField` area for rendering models in UI
  * new `BuildMenu` frame unit/building placement
  * `StrategicArea` group selection
  * status bars under health bars
- client
  * `Mesh`, `SMM` -> `Model`, tree-based model hierarchy
  * Models can be rendered at BSP entities
  * better lighting: bump mapping, environment maps, improved specular lighting
  * skybox
  * DDS format used for all textures, every texture is in its own file
  * switched over to ozEngine classes wherever possible
  * layout files, can be edited in built-in editor (`-e` option) or loaded by missions
  * camera flash and shake effects, camera has an auxiliary thread for event extraction
  * switching weapons with number keys
  * text-to-speech using eSpeak library
  * `.ozState` file compression
  * `ozPersistent` table in Lua scripts for persistence across missions and load/save
- builder
  * Context generates mipmaps and S3TC textures (using libsquish) without initialising OpenGL
  * Terrain can be generated based on config file settings
  * Asset Importer integration, can build Collada models
  * major `Compiler` class updates for tree-based models
  * `Compiler` class generates surface tangents and binormals
- scripting
  * finite automaton implementation, basic building blocks for AIs
  * all complex AIs rewritten as finite automata
- general
  * common, matrix and nirvana sub-namespaces removed
  * LuaJIT can be optionally used instead of official Lua

0.3.1

- client
  * credits screen
- general
  * Win32 port fixed, bugfixes, polishing ...

0.3.0

- oz
  * more C++11 features
    + move semantics
    + defaulted & deleted functions
    + explicit overrides
    + new exception specification
    + limited use of initialiser lists
    + limited use of `constexpr` (float constants)
  * `(D)List` renamed to `(D)Chain`, `(S)Vector` renamed to `(S)List`
  * `String` class has more consistent members, parsing functions added
  * `Vec3`, `Vec4`, `Quat` don't inherit from each other or `Simd` class
  * `AABB` & `Bounds` moved to liboz from matrix
  * `Mutex`, `Semaphore` and `Thread` classes for high-level thread management
  * new `Time` class
  * new `StackTrace` class (split from System class)
  * `System::bell()`
  * `OutputStream` is readable
  * new `BufferStream` class (`OutputStream` with dynamic internal storage)
  * `File` class: file type and size setting changed, added support for NaCl local filesystems
  * new `PFile` class: version of `File` class to act as PhysicsFS wrapper
  * `Log` supports `operator <<` for printing streams
  * new `JSON` class for manipulating JSON files
  * `Config` class removed
  * `clearmacros.hh` header to undefine liboz macros that might conflict other libraries
  * can be compiled as a shared library
  * additional Linux dependency: PhysicsFS, optional dependency for Linux: libpulse-simple
  * better Win32 support, POSIX API replaced with Win32 API where possible
  * ported to NaCl
  * fully documented
  * zlib licence
- common
  * new layer, split from matrix
  * `Span` and `Timer` classes moved to common from matrix
  * new `Lingua` class, gettext-like system, but modularised and suitable for data package system
- matrix
  * BSPs, object classes, fragment pools, client imagos and audios are internally referenced by
    pointers or indices instead by strings, several lookup hashtables replaced by arrays
  * object attributes: tell what special abilities an object can have (nigh vision, binoculars,
    satnav, music player ...)
  * fragment pools: particle replacements; fragments can be configured like object classes, can
    have a graphic model assigned, etc.
  * binary format is used for object classes and fragment pools
  * keys: object can have assigned a key value, so it can lock/unlock matching structure entities
  * lava: can be used both in structures and terrain instead of sea
  * objects can be rotated around z axis in 90° steps
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
  * objects are filtered from rendering if aspect angle from eye is too small
  * configurable fog colour for both terrain and structure water/lava
  * `Mesh` class
    + meshes are not rendered immediately but lists are created for each mesh and all instances
      of a mesh are rendered together
    + MD2-like vertex animation integrated into `Mesh` class
  * offscreen rendering support
  * postprocessing, bloom postprocess effect
  * `Camera` class & proxies
    + smoothing of movement and rotation
    + colour transformation matrix, used for visual effects like injuries, night vision,
      black-and-white sequences ...
    + camera bob, walk/run animations and footsteps sounds are synchronised to a counter in Bot
      class
    + CinematicProxy class for scripted camera movements, colours, music track setting ...
  * `Audio` classes
    + footstep and swim sounds
    + filtering of high-frequency events
    + velocity is set for sound sources and listener
  * `Sound` class
    + sounds are played asynchronously, parallel to rendering
    + asynchronous streaming of tracks
    + support for music in MP3 and AAC formats
    + a user-specified directory can be searched for music
  * new `Input` class
    + key-to-action mappings, key/mouse sensitivity etc.
    + `input.json` configuration file
  * resizable main window
  * new main menu, mission selection menu
- builder
  * prebuilding entirely split from the runtime part of the engine
  * SDL_image replaced by FreeImage, wider range of image formats supported as prebuilding source
  * 16-bit colour components for terrain heightmap
  * no more encapsulating a BSP with a box
  * fragment pools and object classes are compiled
- tools
  * `ozGettext` tool for extracting structure, entity, object names and strings from mission scripts
    for translation
  * `ozManifest` for generating package manifests for NaCl port
  * `ozSineSample` for generating `src/oz/bellSample.inc`
- general
  * PhysicsFS used for accessing game data, so they can be loaded directly from archives
  * game data can be modularised into packages
  * Lua API redesigned
  * Lua classes share API code - no more code triplication of Lua API implementations
  * ported to SDL 2.0
  * ported to NaCl
  * PKGBUILD for Arch Linux and RPM spec file
