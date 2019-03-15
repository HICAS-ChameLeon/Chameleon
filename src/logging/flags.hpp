
#ifndef __LOGGING_FLAGS_HPP__
#define __LOGGING_FLAGS_HPP__

#include <string>

#include <stout/flags.hpp>
#include <stout/option.hpp>

namespace chameleon {
namespace logging {

class Flags : public virtual flags::FlagsBase
{
public:
  Flags();

  bool quiet;
  std::string logging_level;
  Option<std::string> log_dir;
  int logbufsecs;
  bool initialize_driver_logging;
  Option<std::string> external_log_file;
};

} // namespace logging {
} // namespace chameleon {

#endif // __LOGGING_FLAGS_HPP__
