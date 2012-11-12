Put `openzone.game` and `games/openzone.game` into your GtkRadiant/ZeroRadiant directory to add
support for OpenZone.

Currently there are some issues with `synapse.config` file, GtkRadiant probably won't work.

Compiler invocation:

    q3map2 -fs_basepath <srcdata> -meta <srcdata>/baseq3/maps/<map>.map

where `<srcdata>` is directory containing source data of a game package.
