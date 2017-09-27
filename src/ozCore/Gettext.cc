/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

Gettext::~Gettext()
{
  clear();
}

Gettext::Gettext(Gettext&& other)
  : buckets_(other.buckets_), messages_(other.messages_), strings_(other.strings_),
    nBuckets_(other.nBuckets_), nMessages_(other.nMessages_), stringsSize_(other.stringsSize_)
{
  other.buckets_     = nullptr;
  other.messages_    = nullptr;
  other.strings_     = nullptr;
  other.nBuckets_    = 0;
  other.nMessages_   = 0;
  other.stringsSize_ = 0;
}

Gettext& Gettext::operator=(Gettext&& other)
{
  if (&other != this) {
    clear();

    buckets_     = other.buckets_;
    messages_    = other.messages_;
    strings_     = other.strings_;
    nBuckets_    = other.nBuckets_;
    nMessages_   = other.nMessages_;
    stringsSize_ = other.stringsSize_;

    other.buckets_     = nullptr;
    other.messages_    = nullptr;
    other.strings_     = nullptr;
    other.nBuckets_    = 0;
    other.nMessages_   = 0;
    other.stringsSize_ = 0;
  }
  return *this;
}

bool Gettext::contains(const char* message) const
{
  if (nMessages_ == 0 || String::isEmpty(message)) {
    return false;
  }

  uint index = uint(Hash<const char*>()(message)) % uint(nBuckets_);

  for (const Message* m = buckets_[index]; m != nullptr; m = m->next) {
    if (String::equals(strings_ + m->original, message)) {
      return true;
    }
  }
  return false;
}

const char* Gettext::get(const char* message) const
{
  if (nMessages_ == 0 || String::isEmpty(message)) {
    return message;
  }

  uint index = uint(Hash<const char*>()(message)) % uint(nBuckets_);

  for (const Message* m = buckets_[index]; m != nullptr; m = m->next) {
    if (String::equals(strings_ + m->original, message)) {
      return strings_ + m->translation;
    }
  }
  return message;
}

List<const char*> Gettext::catalogueDescriptions() const
{
  List<const char*> descriptions;

  for (int i = 0; i < nMessages_; ++i) {
    if (String::isEmpty(strings_ + messages_[i].original)) {
      descriptions.add(strings_ + messages_[i].translation);
    }
  }
  return descriptions;
}

// .mo file layout can be found at http://www.gnu.org/software/gettext/manual/gettext.html#MO-Files.
bool Gettext::import(const File& file)
{
  Stream is = file.read();
  if (is.available() == 0) {
    return false;
  }

  // Header.
  uint magic = is.readUInt();
  if (magic != GETTEXT_MAGIC) {
    if (Endian::bswap(magic) == GETTEXT_MAGIC) {
      is.setOrder(Endian::Order(!is.order()));
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
  messages_ = Arrays::reallocate<Message>(messages_, nMessages_, nMessages_ + nNewMessages);
  strings_  = Arrays::reallocate<char>(strings_, stringsSize_, stringsSize_ + newStringsSize);

  // Add new message entries.
  for (int i = 0; i < nNewMessages; ++i) {
    is.seek(originalsOffset + i * 8 + 4);
    messages_[nMessages_ + i].original = stringsSize_ + (is.readInt() - stringsOffset);

    is.seek(translationsOffset + i * 8 + 4);
    messages_[nMessages_ + i].translation = stringsSize_ + (is.readInt() - stringsOffset);
  }

  // Add new strings.
  is.seek(stringsOffset);
  memcpy(strings_ + stringsSize_, is.readSkip(newStringsSize), newStringsSize);

  nMessages_   += nNewMessages;
  stringsSize_ += newStringsSize;

  // Rebuild hashtable.
  nBuckets_ = nMessages_ * 4 / 3;

  delete[] buckets_;
  buckets_ = new Message*[nBuckets_] {};

  for (int i = 0; i < nMessages_; ++i) {
    const char* original = strings_ + messages_[i].original;

    if (String::isEmpty(original)) {
      continue;
    }

    uint index = uint(Hash<const char*>()(original)) % uint(nBuckets_);

    messages_[i].next = buckets_[index];
    buckets_[index] = &messages_[i];
  }

  return true;
}

void Gettext::clear()
{
  delete[] buckets_;
  delete[] messages_;
  delete[] strings_;

  buckets_     = nullptr;
  messages_    = nullptr;
  strings_     = nullptr;
  nBuckets_    = 0;
  nMessages_   = 0;
  stringsSize_ = 0;
}

}
