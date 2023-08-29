
#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

#define DIFFERENT_VEC_SIZE "It is not possible to initialize an object with \
two vectors of different sizes"
#define NO_KEY_FOUND "The key does not exist in the hashmap"
#define MIN_SIZE 16
#define UPPER_LOAD_FACTOR 3/4
#define LOWER_LOAD_FACTOR 1/4


template <typename KeyT, typename ValueT>
class HashMap
{
  typedef std::vector<std::pair<KeyT, ValueT>> bucket;

 protected:
  bucket* _hashmap;
  int _capacity;
  int _num_of_elem;

  /**
   * This function maps key and value to a hash table.
   * @tparam KeyT
   * @tparam ValueT
   * @param key int value of the key.
   * @return the index in the hash table that the pair will be store.
   */
  int hash_func (KeyT key) const
  {
    int int_key = std::hash<KeyT>{} (key);
    return int_key&(_capacity - 1);
  }

  /**
   * This function increase the hashmap if the load factor is above 3/4.
   */
  void increase_hashmap ()
  {
    int old_capacity = _capacity;
    auto old_hashmap = new bucket[old_capacity];
    for (int i = 0; i < old_capacity; i++)
    {
      old_hashmap[i] = _hashmap[i];
    }
    delete[] _hashmap;
    int new_capacity = old_capacity * 2;
    _hashmap = new bucket[new_capacity];
    _capacity = new_capacity;
    _num_of_elem = 0;
    for (int i = 0; i < old_capacity; i++)
    {
      for(auto it = old_hashmap[i].begin(); it != old_hashmap[i].end(); it++)
      {
        insert (it->first, it->second);
      }
    }
    delete[] old_hashmap;
  }

  /**
   * This function recuse the hashmap if the load factor is less then 1/4.
   */
  void reduce_hashmap ()
  {
    int old_capacity = _capacity;
    auto old_hashmap = new bucket[old_capacity];
    for (int i = 0; i < old_capacity; i++)
    {
      old_hashmap[i] = _hashmap[i];
    }
    delete[] _hashmap;
    while ((get_load_factor() < (double) LOWER_LOAD_FACTOR) && (_capacity > 1))
    {
      _capacity /= 2;
    }
    _hashmap = new bucket[_capacity];
    _num_of_elem = 0;
    for (int i = 0; i < old_capacity; i++)
    {
      for (const auto &pair : old_hashmap[i])
      {
        insert (pair.first, pair.second);
      }
    }
    delete[] old_hashmap;
  }

 public:

  /*****************************/
  /*** class's c'tor & d'tor ***/
  /*****************************/

  /**
   * Default constructor.
   * @tparam KeyT
   * @tparam ValueT
   */
  HashMap()
  {
    _hashmap = new bucket[MIN_SIZE];
    _capacity = MIN_SIZE;
    _num_of_elem = 0;
  }

  /**
   * This function initialize a hashmap and inserts given pairs of keys and
   * values.
   * @tparam KeyT
   * @tparam ValueT
   * @param keys vector of keys.
   * @param values vector of values.
   */
  HashMap (const std::vector<KeyT>& keys, const std::vector<ValueT>& values)
  {
    size_t vec_size = keys.size();
    if (vec_size != values.size())
    {
      throw std::length_error (DIFFERENT_VEC_SIZE);
    }
    _hashmap = new bucket[MIN_SIZE];
    _capacity = MIN_SIZE;
    _num_of_elem = 0;
    for (size_t i = 0; i < vec_size; i++)
    {
      if (!insert (keys[i], values[i]))
      {
        KeyT k = keys[i];
        ValueT v = values[i];
        (*this)[k] = v;
      }
    }
  }

  /**
   * Copy constructor.
   * @tparam KeyT
   * @tparam ValueT
   * @param copied_hm
   */
  HashMap (const HashMap& copied_hm)
  {
    _capacity = copied_hm.capacity();
    _hashmap = new bucket[_capacity];
    _num_of_elem = 0;
    for (int i = 0; i < _capacity; i++)
    {
      for (const auto& pair : copied_hm._hashmap[i])
      {
        insert (pair.first, pair.second);
      }
    }
  }

  /**
   * Class's destructor.
   */
  virtual ~HashMap()
  {
    delete[] _hashmap;
  }

  /*************************/
  /*** class's functions ***/
  /*************************/

  /**
   * This function insert new pair of key and value to the hashmap.
   * @tparam KeyT
   * @tparam ValueT
   * @param new_elem pair of key and value.
   * @return true if the new pair added successfully and false otherwise.
   */
  bool insert (const KeyT& key, const ValueT& value)
  {
    if (contains_key (key))
    {
      return false;
    }
    int cur_bucket_i = hash_func (key);
    _hashmap[cur_bucket_i].push_back(std::pair<KeyT, ValueT> (key, value));
    _num_of_elem++;
    if ((get_load_factor()) > (double) UPPER_LOAD_FACTOR)
    {
      increase_hashmap();
    }
    return true;
  }

  /**
   * This function erase pair of key and value from the hashmap.
   * @tparam KeyT
   * @tparam ValueT
   * @param key the key of the pair that will be erased.
   * @return true if the pair successfully erased, false otherwise.
   */
  virtual bool erase (const KeyT& key)
  {
    if (contains_key (key))
    {
      int cur_bucket = bucket_index (key);
      for (auto it = _hashmap[cur_bucket].begin(); it !=
      _hashmap[cur_bucket].end(); it++)
      {
        if (it->first == key)
        {
          _hashmap[cur_bucket].erase(it);
          _num_of_elem--;
          if ((get_load_factor()) < (double) LOWER_LOAD_FACTOR)
          {
            reduce_hashmap();
          }
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @return number of elements in the hashmap.
   */
  int size() const {return _num_of_elem;}

  /**
   * @return the capacity of the hashmap.
   */
  int capacity() const {return _capacity;}

  /**
   * @return true if the hashmap is empty, otherwise false.
   */
  bool empty() const {return (_num_of_elem == 0);}

  /**
   * This function checks whether a given key is in the hashmap.
   * @tparam KeyT
   * @tparam ValueT
   * @param key
   * @return true if the key is in the hashmap, false otherwise.
   */
  bool contains_key (const KeyT& key) const
  {
    int cur_bucket_i = hash_func (key);
    for (const std::pair<KeyT, ValueT>& pair : _hashmap[cur_bucket_i])
    {
      if (pair.first == key)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * This function return the value of a given key.
   * @tparam KeyT
   * @tparam ValueT
   * @param key
   * @return if the key is in the hashmap the function returns its value,
   * otherwise an exception is thrown.
   */
  const ValueT& at (const KeyT& key) const
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument (NO_KEY_FOUND);
    }
    int cur_bucket_i = bucket_index (key);
    for (auto& pair : _hashmap[cur_bucket_i])
    {
      if (pair.first == key)
      {
        return pair.second;
      }
    }
    return _hashmap[0][0].second;
  }

  /**
   * This function return the value of a given key.
   * @tparam KeyT
   * @tparam ValueT
   * @param key
   * @return if the key is in the hashmap the function returns its value,
   * otherwise an exception is thrown.
   */
  ValueT& at (const KeyT& key)
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument (NO_KEY_FOUND);
    }
    int cur_bucket_i = bucket_index (key);
    for (auto& pair : _hashmap[cur_bucket_i])
    {
      if (pair.first == key)
      {
        return pair.second;
      }
    }
    return _hashmap[0][0].second;
  }

  /**
   * @return the load factor of the hashmap.
   */
  double get_load_factor () const {return ((double)_num_of_elem) / ((double)
  _capacity);}

  /**
   * This function return the size of a bucket that contains a given key.
   * @param key
   * @return the size of the bucket if the key is in the hashmap, otherwise an
   * exception is thrown.
   */
  int bucket_size (const KeyT& key) const
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument (NO_KEY_FOUND);
    }
    int cur_bucket = bucket_index (key);
    return _hashmap[cur_bucket].size();
  }

  /**
   * This function return the index of the bucket that contains a given key.
   * @tparam KeyT
   * @tparam ValueT
   * @param key
   * @return the index of the bucket if the key is in the hashmap, otherwise an
   * exception is thrown.
   */
  int bucket_index (const KeyT& key) const
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument (NO_KEY_FOUND);
    }
    return hash_func (key);
  }

  /**
   * This function clears the hashmap.
   * @tparam KeyT
   * @tparam ValueT
   */
  void clear()
  {
    for (int i = 0; i < _capacity; i++)
    {
      _hashmap[i].clear();
      _num_of_elem = 0;
    }
  }

  /*************************/
  /*** class's operators ***/
  /*************************/

  /**
   * Assignment Operator.
   * @tparam KeyT
   * @tparam ValueT
   * @param rhs
   * @return Hashmap that will be assigned in another Hashmap.
   */
  HashMap& operator= (const HashMap& rhs)
  {
    if (this != &rhs)
    {
      _capacity = rhs.capacity();
      _num_of_elem = rhs.size();
      delete[] _hashmap;
      _hashmap = new bucket[_capacity];
      for (int i = 0; i < _capacity; i++)
      {
        _hashmap[i] = rhs._hashmap[i];
      }
    }
    return *this;
  }

  /**
   * overlaod const operator.
   * @param key
   * @return the value of the given key if found, otherwise its an unexpected
   * behavior.
   */
  const ValueT& operator[] (const KeyT& key) const
  {
    if (contains_key (key))
    {
      int cur_bucket_i = bucket_index (key);
      for (const auto& pair : _hashmap[cur_bucket_i])
      {
        if (pair.first == key)
        {
          return pair.second;
        }
      }
    }
    return _hashmap[0][0].second;
  }

  /**
   * overlaod operator.
   * @param key
   * @return the value of the given key if found, otherwise the given key is
   * added to the hashmap.
   */
  ValueT& operator[] (const KeyT& key)
  {
    if (!contains_key (key))
    {
      insert (key, ValueT());
    }
    int cur_bucket_i = bucket_index (key);
    for (auto& pair : _hashmap[cur_bucket_i])
    {
      if (pair.first == key)
      {
        return pair.second;
      }
    }
    return _hashmap[0][0].second;
  }

  /**
   * Comparison operator
   * @param rhs
   * @return true if both hashmaps contains the same elements,
   * false otherwise.
   */
  bool operator == (const HashMap& rhs) const
  {
    if (_num_of_elem != rhs.size())
    {
      return false;
    }
    for (int i = 0; i < _capacity; i++)
    {
      for (const auto &pair: _hashmap[i])
      {
        if (!rhs.contains_key (pair.first))
        {
          return false;
        }
        if (pair.second != rhs.at (pair.first))
        {
          return false;
        }
      }
    }
    return true;
  }

  /**
   * Different operator.
   * @param rhs
   * @return false if both hashmaps contains the same elements,
   * true otherwise
   */
  bool operator != (const HashMap& rhs) const
  {
    return !operator== (rhs);
  }

  /****************/
  /*** Iterator ***/
  /****************/

  class ConstIterator
  {
    friend class HashMap;

   private:
    const HashMap<KeyT, ValueT> *_map;
    size_t _bucket_index;
    size_t _pair_index;

   public:

    /***********************/
    /*** Iterator traits ***/
    /***********************/

    typedef std::pair<KeyT, ValueT> value_type;
    typedef const value_type &reference;
    typedef const value_type *pointer;
    typedef ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    /*********************/
    /*** class's c'tor ***/
    /*********************/

    /**
     * Copy constructor
     * @param copy_it
     */
    ConstIterator (const ConstIterator& copy_it) : _map(copy_it
    ._map), _bucket_index(copy_it._bucket_index), _pair_index(copy_it
    ._pair_index) {}

    /**
     * Constructor
     * @param hash_map
     * @param bucket_index
     * @param pair_index
     */
    ConstIterator (const HashMap<KeyT, ValueT> *hash_map, size_t bucket_index,
                   size_t
    pair_index) : _map(hash_map), _bucket_index(bucket_index),
                  _pair_index(pair_index) {};

    /*************************/
    /*** class's operators ***/
    /*************************/

    /**
     * pre-post increment operator.
     * @return the increased const iterator.
     */
    ConstIterator& operator++ ()
    {
      const bucket& cur_bucket = _map->_hashmap[_bucket_index];
      if (++_pair_index >= cur_bucket.size())
      {
        _pair_index = 0;
        _bucket_index++;
      }
      while ((_bucket_index <
              _map->_capacity) && (_map->_hashmap[_bucket_index].empty
              ()))
      {
        _bucket_index++;
      }
      return *this;
    }

    /**
     * post-increment operator.
     * @return the original const iterator.
     */
    ConstIterator operator++ (int)
    {
      ConstIterator it (*this);
      this->operator++();
      return it;
    }

    /**
     * Assignment operator.
     * @param rhs
     * @return The iterator with the values of the given const iterator.
     */
    ConstIterator& operator= (const ConstIterator& rhs)
    {
      if (this != &rhs)
      {
        _map = rhs._map;
        _bucket_index = rhs._bucket_index;
        _pair_index = rhs._pair_index;
      }
      return (*this);
    }

    /**
     * Comparison operator
     * @param rhs
     * @return true if both hashmaps contains the same elements, false
     * otherwise.
     */
    bool operator== (const ConstIterator& rhs) const
    {
      return ((_map == rhs._map) && (_bucket_index == rhs
      ._bucket_index) && (_pair_index == rhs._pair_index));
    }

    /**
     * Different operator.
     * @param rhs
     * @return false if both hashmaps contains the same elements,
     * true otherwise
     */
    bool operator!= (const ConstIterator& rhs) const
    {
      return !operator== (rhs);
    }

    /**
     * Dereference as an lvalue operator.
     */
    reference operator* () const
    {
      return _map->_hashmap[_bucket_index][_pair_index];
    }

    /**
     * Dereference as a rvalue operator.
     */
    pointer operator-> () const
    {
      return &(operator* ());
    }
  };

  using const_iterator = ConstIterator;

  /**
   * @return a const iterator pointing to the first element of the vector
   * container.
   */
  const_iterator cbegin() const
  {
    int i = 0;
    while ((i < _capacity) && (_hashmap[i].empty()))
    {
      i++;
    }
    return ConstIterator(this, i, 0);
  }

  /**
   * @return a const iterator which points to past-the-end element in the
   * vector container
   */
  const_iterator cend() const
  {
    return ConstIterator (this, _capacity, 0);
  }

  /**
   * @return a const iterator pointing to the first element of the vector
   * container.
   */
  const_iterator begin() const
  {
    return cbegin();
  }

  /**
   * @return a const iterator which points to past-the-end element in the
   * vector container
   */
  const_iterator end() const
  {
    return cend();
  }
};

#endif //_HASHMAP_HPP_

