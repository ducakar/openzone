configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz SHARED AABB.hh Alloc.hh Array.hh arrays.hh Bitset.hh Bounds.hh Buffer.hh BufferStream.hh common.hh Config.hh DArray.hh DList.hh Endian.hh Exception.hh File.hh HashIndex.hh HashString.hh InputStream.hh iterables.hh List.hh Log.hh Map.hh Mat44.hh Math.hh OutputStream.hh oz.hh Pair.hh PhysFile.hh Plane.hh Point3.hh Pool.hh Quat.hh SBitset.hh Simd.hh StackTrace.hh String.hh SVector.hh System.hh Time.hh Vec3.hh Vec4.hh Vector.hh Alloc.cc Buffer.cc common.cc Config.cc Exception.cc File.cc Log.cc Mat44.cc Math.cc PhysFile.cc Point3.cc Quat.cc StackTrace.cc String.cc System.cc Time.cc Vec3.cc Vec4.cc )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "AABB.hh;Alloc.hh;Array.hh;arrays.hh;Bitset.hh;Bounds.hh;Buffer.hh;BufferStream.hh;common.hh;Config.hh;DArray.hh;DList.hh;Endian.hh;Exception.hh;File.hh;HashIndex.hh;HashString.hh;InputStream.hh;iterables.hh;List.hh;Log.hh;Map.hh;Mat44.hh;Math.hh;OutputStream.hh;oz.hh;Pair.hh;PhysFile.hh;Plane.hh;Point3.hh;Pool.hh;Quat.hh;SBitset.hh;Simd.hh;StackTrace.hh;String.hh;SVector.hh;System.hh;Time.hh;Vec3.hh;Vec4.hh;Vector.hh" )
set_target_properties( oz PROPERTIES SOVERSION ${OZ_SOVERSION} )
target_link_libraries( oz LINK_PRIVATE ${libs_oz} )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/liboz.pc COMPONENT liboz
         DESTINATION lib${OZ_LIB_SUFFIX}/pkgconfig )
install( TARGETS oz COMPONENT liboz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib${OZ_LIB_SUFFIX}
         ARCHIVE DESTINATION lib${OZ_LIB_SUFFIX}
         PUBLIC_HEADER DESTINATION include/oz )
