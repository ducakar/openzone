function heal50()
  x, y, z = ozGetP()
  ozRemove()
  ozBindUser()
  ozAddLife( 50.0 )
  ozAddObj( "Smiljan", x, y, z + 5.0 )
end
