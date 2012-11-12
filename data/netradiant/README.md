Put `openzone.game` and `games/openzone.game` into your NetRadiant directory to add support for
OpenZone.

Currently q3map2 compiler distributed with NetRadiant produces invalid BSPs -- all faces from
worldspan entity are missing (i.e. whole structure is invisible, except for doors, elevators,
switches etc., while collision works, including for invisible parts.)

One should use some older NetRadiant version or manually invoke BSP compiler from GtkRadiant.

Compiler invocation:

    q3map2 -fs_basepath <srcdata> -meta <srcdata>/baseq3/maps/<map>.map

where `<srcdata>` is directory containing source data of a game package.
