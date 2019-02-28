/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：18-11-26
 * Description：scheduler
 */

#ifndef __SCHEDULER_PROTO_HPP__
#define __SCHEDULER_PROTO_HPP__

// ONLY USEFUL AFTER RUNNING PROTOC.
#include <scheduler.pb.h>

namespace mesos {
namespace scheduler {

inline std::ostream& operator<<(std::ostream& stream, const Call::Type& type)
{
  return stream << Call::Type_Name(type);
}


inline std::ostream& operator<<(std::ostream& stream, const Event::Type& type)
{
  return stream << Event::Type_Name(type);
}

} // namespace scheduler {
} // namespace mesos {

#endif // __SCHEDULER_PROTO_HPP__
