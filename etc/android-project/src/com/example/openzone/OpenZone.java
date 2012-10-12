package com.example.openzone;

import android.app.Activity;
import android.os.Bundle;

public class OpenZone extends Activity
{
  private static native void main( String[] args );

  @Override
  public void onCreate( Bundle savedInstanceState )
  {
    super.onCreate( savedInstanceState );

    main( null );
  }

  static
  {
    System.loadLibrary( "openzone" );
  }
}
