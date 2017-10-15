/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ozCore/ozCore.hh>

using namespace oz;

//struct Error
//{
//  String message;

//  Error() = default;

//  explicit Error(const String& message)
//    : message(message)
//  {}

//  explicit Error(String&& message)
//    : message(static_cast<String&&>(message))
//  {}

//  OZ_PRINTF_FORMAT(2, 3)
//  explicit Error(const char* message, ...)
//  {
//    va_list ap;
//    va_start(ap, message);

//    message = String::vformat(message, ap);

//    va_end(ap);
//  }
//};

//template <typename Type>
//struct Result
//{
//  Type       result;
//  Box<Error> error;

//  Result(Type value)
//    : result(value)
//  {}

//  Result(Box<Error>&& value)
//    : error(static_cast<Box<Error>&&>(value))
//  {}

//  OZ_PRINTF_FORMAT(1, 2)
//  static Result Error(const char* message, ...)
//  {
//    va_list ap;
//    va_start(ap, message);

//    ::Error* error = new ::Error(String::format(message, ap));

//    va_end(ap);

//    return Result(error);
//  }

//  const Type& unwrap() const
//  {
//    if (error != nullptr) {
//      OZ_ERROR(error->message);
//    }
//    return result;
//  }

//  Type& unwrap()
//  {
//    if (error != nullptr) {
//      OZ_ERROR(error->message);
//    }
//    return result;
//  }
//};

int main()
{
  System::init();
  auto json = Json{
    Json
    {"drekec", "pekec"},
    {"1", "2"}
  };
  json.save("/home/davorin/out.txt");
  return 0;
}
