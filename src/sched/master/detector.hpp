#ifndef __CHAMELEON_MASTER_DETECTOR_HPP__
#define __CHAMELEON_MASTER_DETECTOR_HPP__

#include <string>

#include <mesos.pb.h>

#include <process/future.hpp>

#include <stout/duration.hpp>
#include <stout/option.hpp>
#include <stout/try.hpp>

namespace mesos {
namespace master {
namespace detector {

/**
 * An abstraction of a Master detector which can be used to
 * detect the leading master from a group.
 */
class MasterDetector
{
public:
  /**
   * Creates a master detector. If `masterDetectorModule` contains a valid
   * module name (that is, a name that matches the name specified in a JSON
   * file/string passed into the command-line invocation using the `--modules`
   * flag), the result is the `MasterDetector` returned by the module.
   * Additional parameters required to create an object of the detector type are
   * expected to be specified in the JSON's `parameter` object. In command-line
   * invocations, the value of `masterDetectorModule` is expected to come from
   * the `--master_detector` flag.
   *
   * If `masterDetectorModule` is `None`, `zk` is checked and if it contains a
   * valid `zk://` or `file://` path (passed in using the `--master` flag), an
   * instance of ZooKeeperMasterDetector is returned.
   *
   * If both arguments are `None`, `StandaloneMasterDetector` is returned.
   */
  static Try<MasterDetector*> create(
      const Option<std::string>& zk,
      const Option<std::string>& masterDetectorModule = None(),
      const Option<Duration>& zkSessionTimeout = None());

  virtual ~MasterDetector() = 0;

  /**
   * Returns MasterInfo after an election has occurred and the elected
   * master is different than that specified (if any), or NONE if an
   * election occurs and no master is elected (e.g., all masters are
   * lost). A failed future is returned if the detector is unable to
   * detect the leading master due to a non-retryable error.
   * Note that the detector transparently tries to recover from
   * retryable errors.
   * The future is never discarded unless it stays pending when the
   * detector destructs.
   *
   * The 'previous' result (if any) should be passed back if this
   * method is called repeatedly so the detector only returns when it
   * gets a different result.
   */
  virtual process::Future<Option<mesos::MasterInfo>> detect(
      const Option<mesos::MasterInfo>& previous = None()) = 0;
};

} // namespace detector {
} // namespace master {
} // namespace mesos {

#endif // __CHAMELEON_MASTER_DETECTOR_HPP__
