configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz Alloc.cc Buffer.cc common.cc Config.cc Exception.cc File.cc Log.cc Mat44.cc Math.cc Mutex.cc PFile.cc Point.cc Quat.cc Semaphore.cc StackTrace.cc String.cc System.cc Thread.cc Time.cc Vec3.cc Vec4.cc )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "AABB.hh;Alloc.hh;Array.hh;arrays.hh;Bitset.hh;Bounds.hh;Buffer.hh;BufferStream.hh;clearmacros.hh;common.hh;Config.hh;DArray.hh;DList.hh;Endian.hh;Exception.hh;File.hh;HashIndex.hh;HashString.hh;InputStream.hh;iterables.hh;List.hh;Log.hh;Map.hh;Mat44.hh;Math.hh;Mutex.hh;OutputStream.hh;oz.hh;Pair.hh;PFile.hh;Plane.hh;Point.hh;Pool.hh;Quat.hh;SBitset.hh;Semaphore.hh;Simd.hh;StackTrace.hh;String.hh;SVector.hh;System.hh;Thread.hh;Time.hh;Vec3.hh;Vec4.hh;Vector.hh" )
set_target_properties( oz PROPERTIES VERSION "${OZ_VERSION}" SOVERSION "${OZ_SOVERSION}" )
target_link_libraries( oz ${libs_oz} )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/liboz.pc" COMPONENT liboz
         DESTINATION lib${libSuffix}/pkgconfig )
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh" COMPONENT liboz
         DESTINATION include/oz )
install( TARGETS oz COMPONENT liboz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib${libSuffix}
         ARCHIVE DESTINATION lib${libSuffix}
         PUBLIC_HEADER DESTINATION include/oz )
