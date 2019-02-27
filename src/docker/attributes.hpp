
#ifndef __ATTRIBUTES_HPP__
#define __ATTRIBUTES_HPP__

// C++ 11 dependencies
#include <iterator>
#include <string>

// protobuf dependencies
#include <mesos/mesos.hpp>

// stout dependencies
#include <stout/option.hpp>

namespace mesos {

std::ostream& operator<<(std::ostream& stream, const Attribute& attribute);


class Attributes
{
public:
  Attributes() {}

  /*implicit*/
  Attributes(const google::protobuf::RepeatedPtrField<Attribute>& _attributes)
  {
    attributes.MergeFrom(_attributes);
  }

  /*implicit*/
  Attributes(const Attributes& that)
  {
    attributes.MergeFrom(that.attributes);
  }

  Attributes& operator=(const Attributes& that)
  {
    if (this != &that) {
      attributes.Clear();
      attributes.MergeFrom(that.attributes);
    }

    return *this;
  }

  bool operator==(const Attributes& that) const;


  bool operator!=(const Attributes& that) const
  {
    return !(*this == that);
  }

  size_t size() const
  {
    return attributes.size();
  }

  // Using this operator makes it easy to copy an attributes object into
  // a protocol buffer field.
  operator const google::protobuf::RepeatedPtrField<Attribute>&() const
  {
    return attributes;
  }

  void add(const Attribute& attribute)
  {
    attributes.Add()->MergeFrom(attribute);
  }

  const Attribute get(int index) const
  {
    return attributes.Get(index);
  }

  const Option<Attribute> get(const Attribute& thatAttribute) const;

  template <typename T>
  T get(const std::string& name, const T& t) const;

  typedef google::protobuf::RepeatedPtrField<Attribute>::iterator
  iterator;

  typedef google::protobuf::RepeatedPtrField<Attribute>::const_iterator
  const_iterator;

  iterator begin() { return attributes.begin(); }
  iterator end() { return attributes.end(); }

  const_iterator begin() const { return attributes.begin(); }
  const_iterator end() const { return attributes.end(); }

  static Attribute parse(const std::string& name, const std::string& value);
  static Attributes parse(const std::string& s);

  static bool isValid(const Attribute& attribute);

private:
  google::protobuf::RepeatedPtrField<Attribute> attributes;
};

} // namespace mesos {

#endif // __ATTRIBUTES_HPP__
