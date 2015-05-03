/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

/**
 * @file ozCore/HashSet.hh
 *
 * `HashSet` class template.
 */

#pragma once

#include "System.hh"
#include "Pool.hh"

namespace oz
{

/**
 * Chaining hashtable implementation, containing only keys without values.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::HashMap`, `oz::Set`
 */
template <typename Elem, class HashFunc = Hash<Elem>>
class HashSet
{
protected:

  /// Granularity for bucket array and pool block sizes.
  static const int GRANULARITY = 256;

protected:

  /**
   * Bucket.
   */
  struct Entry
  {
    Entry* next; ///< Next entry in a bucket.
    int    hash; ///< Cached hash.
    Elem   elem; ///< Element (or key-value pair).

    OZ_PLACEMENT_POOL_ALLOC(Entry)
  };

  /**
   * Hashtable iterator.
   */
  template <class EntryType, class ElemType>
  class HashIterator : public detail::IteratorBase<EntryType>
  {
  private:

    using detail::IteratorBase<EntryType>::elem;

    const HashSet* table = nullptr; ///< Hashtable that is being iterated.
    int            index = 0;       ///< Index of the current bucket.

  public:

    /**
     * Create an invalid iterator.
     */
    HashIterator() = default;

    /**
     * Create hashtable iterator, initially pointing to the first hashtable element.
     */
    explicit HashIterator(const HashSet& ht) :
      detail::IteratorBase<EntryType>(ht.size == 0 ? nullptr : ht.data[0]), table(&ht)
    {
      while (elem == nullptr && index < table->size - 1) {
        ++index;
        elem = table->data[index];
      }
    }

    /**
     * Pointer to the current element.
     */
    OZ_ALWAYS_INLINE
    operator ElemType* () const
    {
      return &elem->elem;
    }

    /**
     * Reference to the current element.
     */
    OZ_ALWAYS_INLINE
    ElemType& operator * () const
    {
      return elem->elem;
    }

    /**
     * Access to the current element's member.
     */
    OZ_ALWAYS_INLINE
    ElemType* operator -> () const
    {
      return &elem->elem;
    }

    /**
     * Advance to the next element.
     */
    HashIterator& operator ++ ()
    {
      hard_assert(elem != nullptr);

      if (elem->next != nullptr) {
        elem = elem->next;
      }
      else if (index == table->size - 1) {
        elem = nullptr;
      }
      else {
        do {
          ++index;
          elem = table->data[index];
        }
        while (elem == nullptr && index < table->size - 1);
      }
      return *this;
    }

    /**
     * STL-style begin iterator.
     */
    OZ_ALWAYS_INLINE
    HashIterator begin() const
    {
      return *this;
    }

    /**
     * STL-style end iterator.
     */
    OZ_ALWAYS_INLINE
    HashIterator end() const
    {
      return HashIterator();
    }

  };

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef HashIterator<const Entry, const Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef HashIterator<Entry, Elem> Iterator;

protected:

  Pool<Entry, GRANULARITY> pool; ///< Memory pool for entries.
  Entry**                  data; ///< Array of buckets, each containing a linked list of entries.
  int                      size; ///< Number of buckets.

protected:

  /**
   * True iff a bucket's chains have the same length and contain equal elements.
   */
  static bool areChainsEqual(const Entry* entryA, const Entry* entryB)
  {
    const Entry* firstB = entryB;

    while (entryA != nullptr && entryB != nullptr) {
      const Entry* i = firstB;
      do {
        if (i->elem == entryA->elem) {
          goto nextElem;
        }
        i = i->next;
      }
      while (i != nullptr);

      return false;

nextElem:
      entryA = entryA->next;
      entryB = entryB->next;
    }
    // At least one is nullptr, so this is true iff both chains have the same length.
    return entryA == entryB;
  }

  /**
   * Allocate and make a copy of a given bucket chain.
   */
  Entry* cloneChain(const Entry* entry)
  {
    Entry* clone = nullptr;

    if (entry != nullptr) {
      clone = new(pool) Entry{ clone, entry->hash, entry->elem };

      const Entry* original = entry->next;
      Entry*       prevCopy = clone;

      while (original != nullptr) {
        prevCopy->next = new(pool) Entry{ clone, original->hash, original->elem };

        prevCopy = prevCopy->next ;
        original = original->next;
      }
    }
    return clone;
  }

  /**
   * Delete all elements in a given bucket chain.
   */
  void clearChain(Entry* entry)
  {
    while (entry != nullptr) {
      Entry* next = entry->next;

      entry->~Entry();
      pool.deallocate(entry);

      entry = next;
    }
  }

  /**
   * Resize bucket array and rebuild hashtable.
   */
  void resize(int newSize)
  {
    Entry** newData = nullptr;

    if (newSize != 0) {
      newData = new Entry*[newSize] {};

      // Rebuild hashtable.
      for (int i = 0; i < size; ++i) {
        Entry* chain = data[i];
        Entry* next  = nullptr;

        while (chain != nullptr) {
          uint index = uint(chain->hash) % uint(newSize);

          next = chain->next;
          chain->next = newData[index];
          newData[index] = chain;

          chain = next;
        }
      }
    }
    delete[] data;

    data = newData;
    size = newSize;
  }

  /**
   * Ensure a given size for the array of buckets.
   *
   * Size is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` greater or equal to the requested size.
   */
  void ensureCapacity(int capacity)
  {
    if (capacity < 0) {
      OZ_ERROR("oz::HashMap: Capacity overflow");
    }
    else if (size < capacity) {
      int newSize = size * 2;
      if (newSize < capacity) {
        newSize = (capacity + GRANULARITY - 1) & ~(GRANULARITY - 1);
      }
      if (newSize <= 0) {
        OZ_ERROR("oz::HashMap: Capacity overflow");
      }

      resize(newSize);
    }
  }

  /**
   * Insert an element, optionally overwriting an existing one.
   *
   * This is a helper function to reduce code duplication between `add()` and `include()`.
   */
  template <typename Elem_>
  void insert(Elem_&& elem, bool overwrite)
  {
    ensureCapacity(pool.length() + 1);

    int    h     = HashFunc()(elem);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (elem == entry->elem) {
        if (overwrite) {
          entry->elem = static_cast<Elem_&&>(elem);
        }
        return;
      }
      entry = entry->next;
    }

    data[index] = new(pool) Entry{ data[index], h, static_cast<Elem_&&>(elem) };
  }

public:

  /**
   * Create an empty hashtable.
   */
  HashSet() :
    data(nullptr), size(0)
  {}

  /**
   * Initialise from an initialiser list.
   */
  HashSet(InitialiserList<Elem> l) :
    data(nullptr), size(0)
  {
    for (const Elem& e : l) {
      add(e);
    }
  }

  /**
   * Destructor.
   */
  ~HashSet()
  {
    clear();
    trim();
  }

  /**
   * Copy constructor, copies elements and storage.
   */
  HashSet(const HashSet& ht) :
    data(new Entry*[ht.size]), size(ht.size)
  {
    for (int i = 0; i < ht.size; ++i) {
      data[i] = cloneChain(ht.data[i]);
    }
  }

  /**
   * Move constructor, moves storage.
   */
  HashSet(HashSet&& ht) :
    pool(static_cast<Pool<Entry, GRANULARITY>&&>(ht.pool)), data(ht.data), size(ht.size)
  {
    ht.data = nullptr;
    ht.size = 0;
  }

  /**
   * Copy operator, copies elements and storage.
   */
  HashSet& operator = (const HashSet& ht)
  {
    if (&ht != this) {
      clear();

      if (size < ht.pool.length()) {
        delete[] data;

        data = new Entry[ht.size];
        size = ht.size;
      }

      for (int i = 0; i < size; ++i) {
        data[i] = i < ht.size ? cloneChain(ht.data[i]) : nullptr;
      }
    }
    return *this;
  }

  /**
   * Move operator, moves storage.
   */
  HashSet& operator = (HashSet&& ht)
  {
    if (&ht != this) {
      clear();
      delete[] data;

      pool = static_cast<Pool<Entry, GRANULARITY>&&>(ht.pool);
      data = ht.data;
      size = ht.size;

      ht.data = nullptr;
      ht.size = 0;
    }
    return *this;
  }

  /**
   * Assign from an initialiser list.
   */
  HashSet& operator = (InitialiserList<Elem> l)
  {
    clear();

    for (const Elem& e : l) {
      add(e);
    }
    return *this;
  }

  /**
   * True iff contained elements are equal.
   */
  bool operator == (const HashSet& ht) const
  {
    if (pool.length() != ht.pool.length()) {
      return false;
    }

    for (int i = 0; i < size; ++i) {
      if (!areChainsEqual(data[i], ht.data[i])) {
        return false;
      }
    }
    return true;
  }

  /**
   * False iff contained elements are equal.
   */
  bool operator != (const HashSet& ht) const
  {
    return !operator == (ht);
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citerator() const
  {
    return CIterator(*this);
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iterator()
  {
    return Iterator(*this);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator begin() const
  {
    return CIterator(*this);
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator begin()
  {
    return Iterator(*this);
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator end() const
  {
    return CIterator();
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator end()
  {
    return Iterator();
  }

  /**
   * Number of elements.
   */
  OZ_ALWAYS_INLINE
  int length() const
  {
    return pool.length();
  }

  /**
   * True iff empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return pool.isEmpty();
  }

  /**
   * Current size of memory pool for elements.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return pool.capacity();
  }

  /**
   * True iff an element matching a given key is found in the hashtable.
   */
  template <typename Key = Elem>
  bool contains(const Key& key) const
  {
    if (size == 0) {
      return false;
    }

    int    h     = HashFunc()(key);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (key == entry->elem) {
        return true;
      }

      entry = entry->next;
    }
    return false;
  }

  /**
   * Add a new element, if the element already exists in the hashtable overwrite the existing one.
   */
  template <typename Elem_ = Elem>
  void add(Elem_&& elem)
  {
    insert(static_cast<Elem_&&>(elem), true);
  }

  /**
   * Add a new element if it does not exist in the hashtable.
   */
  template <typename Elem_ = Elem>
  void include(Elem_&& elem)
  {
    insert(static_cast<Elem_&&>(elem), false);
  }

  /**
   * Remove the element that matches a given key.
   *
   * @return True iff the element was found (and removed).
   */
  template <typename Key = Elem>
  bool exclude(const Key& key)
  {
    if (size == 0) {
      return false;
    }

    int     h     = HashFunc()(key);
    uint    index = uint(h) % uint(size);
    Entry*  entry = data[index];
    Entry** prev  = &data[index];

    while (entry != nullptr) {
      if (key == entry->elem) {
        *prev = entry->next;

        entry->~Entry();
        pool.deallocate(entry);

        return true;
      }

      prev = &entry->next;
      entry = entry->next;
    }
    return false;
  }

  /**
   * Trim the bucket array size to 4/3 of the current number of elements.
   *
   * In case the hastable contains no entries all its storage gets deallocated.
   */
  void trim()
  {
    int newSize = pool.length() * 4 / 3;

    if (newSize < size) {
      resize(newSize);

      if (newSize == 0) {
        pool.free();
      }
    }
  }

  /**
   * Clear the hashtable.
   */
  void clear()
  {
    for (int i = 0; i < size; ++i) {
      clearChain(data[i]);
      data[i] = nullptr;
    }
  }

};

}
