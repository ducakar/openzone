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

#define OZ_DECLARE_CREATE_ERROR(name)                    \
  OZ_PRINTF_FORMAT(1, 2)                                 \
  static Box<Error> name(const char* message = "", ...); \

#define OZ_IMPLEMENT_CREATE_ERROR(name, type)      \
  Box<Error> Error::name(const char* message, ...) \
  {                                                \
    va_list ap;                                    \
    va_start(ap, message);                         \
    Error* error = new Error(type, message, ap);   \
    va_end(ap);                                    \
    return error;                                  \
  }

struct Error
{
  enum Type
  {
    LOGIC,    ///< General logic error.
    LIMIT,    ///< Out of implementation-defined limits.
    CAST,     ///< Invalid cast.
    ARGUMENT, ///< Invalid argument value.
    RANGE,    ///< Index out of bounds.

    RUNTIME,  ///< General runtime error.
    SYSTEM,   ///< System error (general error reported by OS API).
    ALLOC,    ///< Memory allocation error.
    OVERFLOW, ///< Overflow in computation.
    IO,       ///< General I/O error.
    FILE,     ///< Filesystem operation error.
    NETWORK   ///< Network I/O error.
  };

  Type   type_;
  String message_;

  OZ_PRINTF_FORMAT(3, 4)
  explicit Error(Type type, const char* message, ...)
    : type_(type)
  {
    va_list ap;
    va_start(ap, message);

    message_ = String::vformat(message, ap);

    va_end(ap);
  }

  OZ_PRINTF_FORMAT(3, 0)
  explicit Error(Type type, const char* message, va_list ap)
    : type_(type), message_(String::vformat(message, ap))
  {}

  Type type() const noexcept
  {
    return type_;
  }

  const String& message() const noexcept
  {
    return message_;
  }

  OZ_DECLARE_CREATE_ERROR(logic)
  OZ_DECLARE_CREATE_ERROR(limit)
  OZ_DECLARE_CREATE_ERROR(cast)
  OZ_DECLARE_CREATE_ERROR(argument)
  OZ_DECLARE_CREATE_ERROR(range)

  OZ_DECLARE_CREATE_ERROR(runtime)
  OZ_DECLARE_CREATE_ERROR(system)
  OZ_DECLARE_CREATE_ERROR(alloc)
  OZ_DECLARE_CREATE_ERROR(overflow)
  OZ_DECLARE_CREATE_ERROR(io)
  OZ_DECLARE_CREATE_ERROR(file)
  OZ_DECLARE_CREATE_ERROR(network)
};

OZ_IMPLEMENT_CREATE_ERROR(logic, LOGIC)
OZ_IMPLEMENT_CREATE_ERROR(limit, LIMIT)
OZ_IMPLEMENT_CREATE_ERROR(cast, CAST)
OZ_IMPLEMENT_CREATE_ERROR(argument, ARGUMENT)
OZ_IMPLEMENT_CREATE_ERROR(range, RANGE)

OZ_IMPLEMENT_CREATE_ERROR(runtime, RUNTIME)
OZ_IMPLEMENT_CREATE_ERROR(system, SYSTEM)
OZ_IMPLEMENT_CREATE_ERROR(alloc, ALLOC)
OZ_IMPLEMENT_CREATE_ERROR(overflow, OVERFLOW)
OZ_IMPLEMENT_CREATE_ERROR(io, IO)
OZ_IMPLEMENT_CREATE_ERROR(file, FILE)
OZ_IMPLEMENT_CREATE_ERROR(network, NETWORK)

template <typename Type>
struct Result
{
  Type       result;
  Box<Error> error;

  Result(Type value)
    : result(value)
  {}

  Result(Box<Error>&& value)
    : error(static_cast<Box<Error>&&>(value))
  {}

  const Type& unwrap() const
  {
    if (error != nullptr) {
      OZ_ERROR("%s", error->message().c());
    }
    return result;
  }

  Type& unwrap()
  {
    if (error != nullptr) {
      OZ_ERROR("%s", error->message().c());
    }
    return result;
  }
};

static Result<String> read(const File& file)
{
  Stream is = file.read();
  if (is.available() == 0) {
    return Error::io("Read failed");
  }
  else {
    return String(is.begin(), is.available());
  }
}

int main()
{
  System::init();
  Log() << read("/home/davorin/.bashrc").unwrap();

  const int l[] = {1, 2, 3};
  for (const int& i : crange(l)) {
    Log() << i;
  }
  return 0;
}
