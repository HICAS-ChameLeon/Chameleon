// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: module.proto

#ifndef PROTOBUF_module_2eproto__INCLUDED
#define PROTOBUF_module_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
#include "mesos.pb.h"
// @@protoc_insertion_point(includes)

namespace mesos {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_module_2eproto();
void protobuf_AssignDesc_module_2eproto();
void protobuf_ShutdownFile_module_2eproto();

class Modules;
class Modules_Library;
class Modules_Library_Module;

// ===================================================================

class Modules_Library_Module : public ::google::protobuf::Message {
 public:
  Modules_Library_Module();
  virtual ~Modules_Library_Module();

  Modules_Library_Module(const Modules_Library_Module& from);

  inline Modules_Library_Module& operator=(const Modules_Library_Module& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Modules_Library_Module& default_instance();

  void Swap(Modules_Library_Module* other);

  // implements Message ----------------------------------------------

  Modules_Library_Module* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Modules_Library_Module& from);
  void MergeFrom(const Modules_Library_Module& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // repeated .mesos.Parameter parameters = 2;
  inline int parameters_size() const;
  inline void clear_parameters();
  static const int kParametersFieldNumber = 2;
  inline const ::mesos::Parameter& parameters(int index) const;
  inline ::mesos::Parameter* mutable_parameters(int index);
  inline ::mesos::Parameter* add_parameters();
  inline const ::google::protobuf::RepeatedPtrField< ::mesos::Parameter >&
      parameters() const;
  inline ::google::protobuf::RepeatedPtrField< ::mesos::Parameter >*
      mutable_parameters();

  // @@protoc_insertion_point(class_scope:mesos.Modules.Library.Module)
 private:
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::google::protobuf::RepeatedPtrField< ::mesos::Parameter > parameters_;
  friend void  protobuf_AddDesc_module_2eproto();
  friend void protobuf_AssignDesc_module_2eproto();
  friend void protobuf_ShutdownFile_module_2eproto();

  void InitAsDefaultInstance();
  static Modules_Library_Module* default_instance_;
};
// -------------------------------------------------------------------

class Modules_Library : public ::google::protobuf::Message {
 public:
  Modules_Library();
  virtual ~Modules_Library();

  Modules_Library(const Modules_Library& from);

  inline Modules_Library& operator=(const Modules_Library& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Modules_Library& default_instance();

  void Swap(Modules_Library* other);

  // implements Message ----------------------------------------------

  Modules_Library* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Modules_Library& from);
  void MergeFrom(const Modules_Library& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef Modules_Library_Module Module;

  // accessors -------------------------------------------------------

  // optional string file = 1;
  inline bool has_file() const;
  inline void clear_file();
  static const int kFileFieldNumber = 1;
  inline const ::std::string& file() const;
  inline void set_file(const ::std::string& value);
  inline void set_file(const char* value);
  inline void set_file(const char* value, size_t size);
  inline ::std::string* mutable_file();
  inline ::std::string* release_file();
  inline void set_allocated_file(::std::string* file);

  // optional string name = 2;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 2;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // repeated .mesos.Modules.Library.Module modules = 3;
  inline int modules_size() const;
  inline void clear_modules();
  static const int kModulesFieldNumber = 3;
  inline const ::mesos::Modules_Library_Module& modules(int index) const;
  inline ::mesos::Modules_Library_Module* mutable_modules(int index);
  inline ::mesos::Modules_Library_Module* add_modules();
  inline const ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library_Module >&
      modules() const;
  inline ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library_Module >*
      mutable_modules();

  // @@protoc_insertion_point(class_scope:mesos.Modules.Library)
 private:
  inline void set_has_file();
  inline void clear_has_file();
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* file_;
  ::std::string* name_;
  ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library_Module > modules_;
  friend void  protobuf_AddDesc_module_2eproto();
  friend void protobuf_AssignDesc_module_2eproto();
  friend void protobuf_ShutdownFile_module_2eproto();

  void InitAsDefaultInstance();
  static Modules_Library* default_instance_;
};
// -------------------------------------------------------------------

class Modules : public ::google::protobuf::Message {
 public:
  Modules();
  virtual ~Modules();

  Modules(const Modules& from);

  inline Modules& operator=(const Modules& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Modules& default_instance();

  void Swap(Modules* other);

  // implements Message ----------------------------------------------

  Modules* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Modules& from);
  void MergeFrom(const Modules& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef Modules_Library Library;

  // accessors -------------------------------------------------------

  // repeated .mesos.Modules.Library libraries = 1;
  inline int libraries_size() const;
  inline void clear_libraries();
  static const int kLibrariesFieldNumber = 1;
  inline const ::mesos::Modules_Library& libraries(int index) const;
  inline ::mesos::Modules_Library* mutable_libraries(int index);
  inline ::mesos::Modules_Library* add_libraries();
  inline const ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library >&
      libraries() const;
  inline ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library >*
      mutable_libraries();

  // @@protoc_insertion_point(class_scope:mesos.Modules)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library > libraries_;
  friend void  protobuf_AddDesc_module_2eproto();
  friend void protobuf_AssignDesc_module_2eproto();
  friend void protobuf_ShutdownFile_module_2eproto();

  void InitAsDefaultInstance();
  static Modules* default_instance_;
};
// ===================================================================


// ===================================================================

// Modules_Library_Module

// optional string name = 1;
inline bool Modules_Library_Module::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Modules_Library_Module::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Modules_Library_Module::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Modules_Library_Module::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& Modules_Library_Module::name() const {
  // @@protoc_insertion_point(field_get:mesos.Modules.Library.Module.name)
  return *name_;
}
inline void Modules_Library_Module::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:mesos.Modules.Library.Module.name)
}
inline void Modules_Library_Module::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:mesos.Modules.Library.Module.name)
}
inline void Modules_Library_Module::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:mesos.Modules.Library.Module.name)
}
inline ::std::string* Modules_Library_Module::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:mesos.Modules.Library.Module.name)
  return name_;
}
inline ::std::string* Modules_Library_Module::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Modules_Library_Module::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:mesos.Modules.Library.Module.name)
}

// repeated .mesos.Parameter parameters = 2;
inline int Modules_Library_Module::parameters_size() const {
  return parameters_.size();
}
inline void Modules_Library_Module::clear_parameters() {
  parameters_.Clear();
}
inline const ::mesos::Parameter& Modules_Library_Module::parameters(int index) const {
  // @@protoc_insertion_point(field_get:mesos.Modules.Library.Module.parameters)
  return parameters_.Get(index);
}
inline ::mesos::Parameter* Modules_Library_Module::mutable_parameters(int index) {
  // @@protoc_insertion_point(field_mutable:mesos.Modules.Library.Module.parameters)
  return parameters_.Mutable(index);
}
inline ::mesos::Parameter* Modules_Library_Module::add_parameters() {
  // @@protoc_insertion_point(field_add:mesos.Modules.Library.Module.parameters)
  return parameters_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::mesos::Parameter >&
Modules_Library_Module::parameters() const {
  // @@protoc_insertion_point(field_list:mesos.Modules.Library.Module.parameters)
  return parameters_;
}
inline ::google::protobuf::RepeatedPtrField< ::mesos::Parameter >*
Modules_Library_Module::mutable_parameters() {
  // @@protoc_insertion_point(field_mutable_list:mesos.Modules.Library.Module.parameters)
  return &parameters_;
}

// -------------------------------------------------------------------

// Modules_Library

// optional string file = 1;
inline bool Modules_Library::has_file() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Modules_Library::set_has_file() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Modules_Library::clear_has_file() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Modules_Library::clear_file() {
  if (file_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_->clear();
  }
  clear_has_file();
}
inline const ::std::string& Modules_Library::file() const {
  // @@protoc_insertion_point(field_get:mesos.Modules.Library.file)
  return *file_;
}
inline void Modules_Library::set_file(const ::std::string& value) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(value);
  // @@protoc_insertion_point(field_set:mesos.Modules.Library.file)
}
inline void Modules_Library::set_file(const char* value) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(value);
  // @@protoc_insertion_point(field_set_char:mesos.Modules.Library.file)
}
inline void Modules_Library::set_file(const char* value, size_t size) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:mesos.Modules.Library.file)
}
inline ::std::string* Modules_Library::mutable_file() {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:mesos.Modules.Library.file)
  return file_;
}
inline ::std::string* Modules_Library::release_file() {
  clear_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = file_;
    file_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Modules_Library::set_allocated_file(::std::string* file) {
  if (file_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete file_;
  }
  if (file) {
    set_has_file();
    file_ = file;
  } else {
    clear_has_file();
    file_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:mesos.Modules.Library.file)
}

// optional string name = 2;
inline bool Modules_Library::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Modules_Library::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Modules_Library::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Modules_Library::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& Modules_Library::name() const {
  // @@protoc_insertion_point(field_get:mesos.Modules.Library.name)
  return *name_;
}
inline void Modules_Library::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:mesos.Modules.Library.name)
}
inline void Modules_Library::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:mesos.Modules.Library.name)
}
inline void Modules_Library::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:mesos.Modules.Library.name)
}
inline ::std::string* Modules_Library::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:mesos.Modules.Library.name)
  return name_;
}
inline ::std::string* Modules_Library::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Modules_Library::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:mesos.Modules.Library.name)
}

// repeated .mesos.Modules.Library.Module modules = 3;
inline int Modules_Library::modules_size() const {
  return modules_.size();
}
inline void Modules_Library::clear_modules() {
  modules_.Clear();
}
inline const ::mesos::Modules_Library_Module& Modules_Library::modules(int index) const {
  // @@protoc_insertion_point(field_get:mesos.Modules.Library.modules)
  return modules_.Get(index);
}
inline ::mesos::Modules_Library_Module* Modules_Library::mutable_modules(int index) {
  // @@protoc_insertion_point(field_mutable:mesos.Modules.Library.modules)
  return modules_.Mutable(index);
}
inline ::mesos::Modules_Library_Module* Modules_Library::add_modules() {
  // @@protoc_insertion_point(field_add:mesos.Modules.Library.modules)
  return modules_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library_Module >&
Modules_Library::modules() const {
  // @@protoc_insertion_point(field_list:mesos.Modules.Library.modules)
  return modules_;
}
inline ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library_Module >*
Modules_Library::mutable_modules() {
  // @@protoc_insertion_point(field_mutable_list:mesos.Modules.Library.modules)
  return &modules_;
}

// -------------------------------------------------------------------

// Modules

// repeated .mesos.Modules.Library libraries = 1;
inline int Modules::libraries_size() const {
  return libraries_.size();
}
inline void Modules::clear_libraries() {
  libraries_.Clear();
}
inline const ::mesos::Modules_Library& Modules::libraries(int index) const {
  // @@protoc_insertion_point(field_get:mesos.Modules.libraries)
  return libraries_.Get(index);
}
inline ::mesos::Modules_Library* Modules::mutable_libraries(int index) {
  // @@protoc_insertion_point(field_mutable:mesos.Modules.libraries)
  return libraries_.Mutable(index);
}
inline ::mesos::Modules_Library* Modules::add_libraries() {
  // @@protoc_insertion_point(field_add:mesos.Modules.libraries)
  return libraries_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library >&
Modules::libraries() const {
  // @@protoc_insertion_point(field_list:mesos.Modules.libraries)
  return libraries_;
}
inline ::google::protobuf::RepeatedPtrField< ::mesos::Modules_Library >*
Modules::mutable_libraries() {
  // @@protoc_insertion_point(field_mutable_list:mesos.Modules.libraries)
  return &libraries_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace mesos

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_module_2eproto__INCLUDED
