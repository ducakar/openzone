/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Gettext.hh"

#include <cstdlib>
#include <cstring>

namespace oz
{

static const char* const ENV_VARS[]    = {"LC_ALL", "LC_MESSAGES", "LANG"};
static const uint        GETTEXT_MAGIC = 0x950412de;
static const int         MAX_MESSAGES  = 1 << 16;

static inline uint bucketIndex(const char* message, int size)
{
  return Hash<const char*>()(message) % uint(size);
}

struct Gettext::Message
{
  int      original;
  int      translation;
  Message* next;
};

const char* Gettext::systemLanguage(const char* fallback)
{
  for (const char* envVar : ENV_VARS) {
    const char* lang = getenv(envVar);

    if (lang != nullptr && !String::isEmpty(lang)) {
      return lang;
    }
  }
  return fallback;
}

Gettext::Gettext() = default;

Gettext::~Gettext() = default;

Gettext::Gettext(Gettext&&) noexcept = default;

Gettext& Gettext::operator=(Gettext&&) noexcept = default;

bool Gettext::contains(const char* message) const
{
  if (messages_.isEmpty() || String::isEmpty(message)) {
    return false;
  }

  uint index = bucketIndex(message, buckets_.size());

  for (const Message* m = buckets_[index]; m != nullptr; m = m->next) {
    if (String::equals(&strings_[m->original], message)) {
      return true;
    }
  }
  return false;
}

const char* Gettext::get(const char* message) const
{
  if (messages_.isEmpty() || String::isEmpty(message)) {
    return message;
  }

  uint index = bucketIndex(message, buckets_.size());

  for (const Message* m = buckets_[index]; m != nullptr; m = m->next) {
    if (String::equals(&strings_[m->original], message)) {
      return &strings_[m->translation];
    }
  }
  return message;
}

List<const char*> Gettext::catalogueDescriptions() const
{
  List<const char*> descriptions;

  for (const Message& m : messages_) {
    if (String::isEmpty(&strings_[m.original])) {
      descriptions.add(&strings_[m.translation]);
    }
  }
  return descriptions;
}

// .mo file layout can be found at http://www.gnu.org/software/gettext/manual/gettext.html#MO-Files.
bool Gettext::import(const File& file)
{
  Stream is(0);
  if (!file.read(&is)) {
    return false;
  }

  // Header.
  uint magic = is.readUInt();
  if (magic != GETTEXT_MAGIC) {
    if (Endian::bswap(magic) == GETTEXT_MAGIC) {
      is.setOrder(Endian::Order(1 - is.order()));
    }
    else {
      return false;
    }
  }

  is.readInt();
  int nNewMessages = is.readInt();
  if (nNewMessages <= 0 || nNewMessages > MAX_MESSAGES) {
    return nNewMessages == 0;
  }

  int originalsOffset    = is.readInt();
  int translationsOffset = is.readInt();
  int hashtableOffset    = is.readInt();
  int hashtableSize      = is.readInt();
  int stringsOffset      = hashtableOffset + hashtableSize;
  int newStringsSize     = is.capacity() - stringsOffset;

  // Expand messages and strings arrays.
  int nOldMessages   = messages_.size();
  int oldStringsSize = strings_.size();

  messages_.resize(nOldMessages + nNewMessages, true);
  strings_.resize(oldStringsSize + newStringsSize, true);

  // Add new message entries.
  for (int i = 0; i < nNewMessages; ++i) {
    is.seek(originalsOffset + i * 8 + 4);
    messages_[nOldMessages + i].original = oldStringsSize + (is.readInt() - stringsOffset);

    is.seek(translationsOffset + i * 8 + 4);
    messages_[nOldMessages + i].translation = oldStringsSize + (is.readInt() - stringsOffset);
  }

  // Add new strings.
  is.seek(stringsOffset);
  memcpy(strings_.begin() + oldStringsSize, is.readSkip(newStringsSize), newStringsSize);

  // Rebuild hashtable.
  buckets_ = List<Message*>(messages_.size() * 4 / 3);

  for (Message& message : messages_) {
    const char* original = &strings_[message.original];

    if (String::isEmpty(original)) {
      continue;
    }

    uint index = bucketIndex(original, buckets_.size());

    message.next    = buckets_[index];
    buckets_[index] = &message;
  }

  return true;
}

void Gettext::clear()
{
  buckets_.clear();
  messages_.clear();
  strings_.clear();
}

}
