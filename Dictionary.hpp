
#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_

#include "HashMap.hpp"

class InvalidKey : public std::invalid_argument
{
 public:
  /*****************************/
  /*** class's c'tor & d'tor ***/
  /*****************************/

  /**
   * Default constructor.
   */
  InvalidKey() : std::invalid_argument ("") {}

  /**
   * Constructor
   * @param message the message that will be thrown.
   */
  explicit InvalidKey(const std::string& message) : std::invalid_argument
  (message) {}

  /**
   * Copy Constructor.
   * @param rhs
   */
  InvalidKey(const InvalidKey& rhs) = default;
};

class Dictionary : public HashMap<std::string, std::string>
{
 public:

  /*****************************/
  /*** class's c'tor & d'tor ***/
  /*****************************/

  /**
   * Default constructor.
   */
    Dictionary() = default;

  /**
   * Constructor.
   * @param keys
   * @param values
   */
  Dictionary(const std::vector<std::string>& keys, const
  std::vector<std::string>& values) : HashMap<std::string, std::string>
      (keys, values) {};

  /**
   * Copy constructor.
   * @param copied_d
   */
  Dictionary(const Dictionary& copied_d) = default;

  /**
   * Destructor.
   */
  ~Dictionary() override = default;

  /*************************/
  /*** class's functions ***/
  /*************************/

  /**
   * This function erase a pair from the dictionary.
   * @param key the key of the pair.
   * @return if the key is not in the dictionary an exception will be thrown,
   * else, if the key was found then it will be erased and the function will
   * return true.
   */
  bool erase (const std::string& key) override
  {
    if (!HashMap<std::string, std::string>::erase (key))
    {
      throw InvalidKey (NO_KEY_FOUND);
    }
    return true;
  }

  /**
   * This function adds new element from given iterators.
   * @tparam Iterator
   * @param it_start begin iterator.
   * @param it_end end iterator.
   */
  template<typename Iterator>
  void update (const Iterator& it_start, const Iterator& it_end)
  {
    if (it_start != it_end)
    {
      for (auto it = it_start; it != it_end; it++)
      {
        (*this)[it->first] = it->second;
      }
    }
  }
};


#endif //_DICTIONARY_HPP_
