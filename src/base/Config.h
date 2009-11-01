/*
 *  Config.h
 *
 *  List that reads configuration file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  /**
   * Container for application variables.
   *
   * Variables are key-value pairs. They can be loaded from or saved to an XML file.
   */
  class Config
  {
    private:

      // Hashtable size.
      static const int SIZE = 1023;
      // Size of buffer used when loading from file (maximum key/value length).
      static const int BUFFER_SIZE = 1024;

      // Hashtable of variables.
      HashString<String, SIZE> vars;

    public:

      /**
       * Add variable.
       * @param key variable name
       * @param value variable value
       */
      void add( const char *key, const char *value )
      {
        if( vars.contains( key ) ) {
          vars.cachedValue() = value;
        }
        else {
          vars.add( key, value );
        }
      }

      /**
       * Remove variable.
       * @param key variable name
       */
      void remove( const char *key )
      {
        vars.remove( key );
      }

      /**
       * @param key variable name
       * @return true if config contains the variable
       */
      bool contains( const char *key ) const
      {
        return vars.contains( key );
      }

      /**
       * Get value of variable. Only use this function if you are sure the key exists.
       * @param key variable name
       * @return variable value
       */
      const String &operator [] ( const char *key ) const
      {
        return vars[key];
      }

      /**
       * @param name variable name
       * @param defVal default value, if variable does not exist in configuration
       * @return value of given variable
       */
      bool get( const char *name, bool defVal );

      /**
       * @param name variable name
       * @param defVal default value, if variable does not exist in configuration
       * @return value of given variable
       */
      int get( const char *name, int defVal );

      /**
       * @param name variable name
       * @param defVal default value, if variable does not exist in configuration
       * @return value of given variable
       */
      float get( const char *name, float defVal );

      /**
       * @param name variable name
       * @param defVal default value, if variable does not exist in configuration
       * @return value of given variable
       */
      double get( const char *name, double defVal );

      /**
       * @param name variable name
       * @param defVal default value, if variable does not exist in configuration
       * @return value of given variable
       */
      const char *get( const char *name, const char *defVal );

      /**
       * Load variables from an XML file. It only reads the nodes named "var" that must have the
       * following format:
       * <pre>&lt;var name="varName" value="varValue"/&gt;</pre>
       * Everything else is ignored.
       * @param file file path
       * @return true if successful
       */
      bool load( const char *file );

      /**
       * Write variables to an XML file. The XML file will have the following format:
       * <pre>
       * &lt;?xml version="1.0" encoding="UTF-8"?&gt;
       * &lt;config>
       *   &lt;var name="var1Name" value="var1Value"/&gt;
       *   &lt;var name="var2Name" value="var2Value"/&gt;
       *   ...
       * &lt;/config&gt;</pre>
       * @param file file path
       * @return true if successful
       */
      bool save( const char *path );

      /**
       * Clear variables.
       */
      void clear();

      /**
       * Print variables to a formatted String. It's formatted like
       * <pre>
       * key1 = "value1"
       * key2 = "value2"</pre>
       * @return formatted String
       */
      String toString( const String &indentString = "" );

  };

  extern Config config;

}
