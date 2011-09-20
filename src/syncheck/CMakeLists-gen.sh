#!/bin/sh

cat << EOF > CMakeLists.txt
add_syncheck( syncheck_oz ""
`LC_COLLATE=C ls ../oz/*.cpp | xargs printf '  %s\n'`
)

add_syncheck( syncheck_matrix ""
`LC_COLLATE=C ls ../matrix/*.cpp | xargs printf '  %s\n'`
)

add_syncheck( syncheck_nirvana ""
`LC_COLLATE=C ls ../nirvana/*.cpp | xargs printf '  %s\n'`
)

add_syncheck( syncheck_client ""
`LC_COLLATE=C ls ../client/*.cpp | xargs printf '  %s\n'`
)

add_syncheck( syncheck_matrix-tools "-DOZ_TOOLS"
`LC_COLLATE=C ls ../matrix/*.cpp | xargs printf '  %s\n'`
)

add_syncheck( syncheck_client-tools "-DOZ_TOOLS"
`LC_COLLATE=C ls ../client/*.cpp | xargs printf '  %s\n'`
)

add_custom_target( syncheck_all DEPENDS
  syncheck_oz syncheck_matrix syncheck_nirvana syncheck_client
  syncheck_matrix-tools syncheck_client-tools )
EOF
