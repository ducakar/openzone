#!/bin/sh

export LC_COLLATE=C

cat << EOF > install_files.cmake
#
# info files
#
if( WIN32 )
  install( FILES
    AUTHORS
    COPYING
`ls -Q README* 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION . )
else()
  install( FILES
    AUTHORS
    COPYING
`ls -Q README* 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/doc/openzone )
endif()

#
# oalinst, DLLs
#
if( WIN32 )
  install( FILES
`ls -Q mingw32/{*.exe,*.dll} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION bin )
  install( FILES
`ls -Q mingw32/*.bat 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION . )
endif()

#
# data
#
if( OZ_INSTALL_DATA )

  #
  # share/openzone/class
  #
  install( FILES
`ls -Q share/openzone/class/{*.rc,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/class )

  #
  # share/openzone/ui/font
  #
  install( FILES
`ls -Q share/openzone/ui/font/{*.ttf,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/ui/font )

  #
  # share/openzone/ui/cur
  #
  install( FILES
`ls -Q share/openzone/ui/cur/{*.ozcCur,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/ui/cur )

  #
  # share/openzone/ui/icon
  #
  install( FILES
`ls -Q share/openzone/ui/icon/{*.ozcTex,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/ui/icon )

  #
  # share/openzone/lua/matrix
  #
  install( FILES
`ls -Q share/openzone/lua/matrix/{*.lua,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/lua/matrix )

  #
  # share/openzone/lua/nirvana
  #
  install( FILES
`ls -Q share/openzone/lua/nirvana/{*.lua,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/lua/nirvana )

  #
  # share/openzone/bsp
  #
  install( FILES
`ls -Q share/openzone/bsp/{*.{ozBSP,ozcBSP},*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/bsp )

  #
  # share/openzone/bsp/tex
  #
  install( FILES
`ls -Q share/openzone/bsp/tex/{*.ozcTex,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/bsp/tex )

  #
  # share/openzone/mdl
  #
  install( FILES
`ls -Q share/openzone/mdl/{*.{ozcSMM,ozcMD2},*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/mdl )

  #
  # share/openzone/name
  #
  install( FILES
`ls -Q share/openzone/name/{*.txt,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/name )

  #
  # share/openzone/snd
  #
  install( FILES
`ls -Q share/openzone/snd/{*.wav,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/snd )

  #
  # share/openzone/music
  #
  install( FILES
`ls -Q share/openzone/music/{*.oga,*README*,*COPYING*} 2> /dev/null | xargs printf '    "%s"\n'`
    DESTINATION share/openzone/music )

  #
  # share/openzone/terra
  #
  install( FILES
`ls -Q share/openzone/terra/{*.{ozTerra,ozcTerra},*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/terra )

  #
  # share/openzone/caelum
  #
  install( FILES
`ls -Q share/openzone/caelum/{*.ozcCaelum,*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/caelum )

  #
  # share/openzone/glsl
  #
  install( FILES
`ls -Q share/openzone/glsl/{*.{glsl,vert,frag},*README*,*COPYING*} 2> /dev/null | xargs printf '    %s\n'`
    DESTINATION share/openzone/glsl )

  #
  # share/locale
  #
EOF

for langDir in share/locale/*; do
  cat << EOF >> install_files.cmake
  install( FILES
    $langDir/LC_MESSAGES/openzone.mo
    DESTINATION $langDir/LC_MESSAGES )
EOF
done

cat << EOF >> install_files.cmake

endif()
EOF
