/*
** Copyright 2013 Merethis
**
** This file is part of Centreon Profiling.
**
** Centreon Profiling is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Profiling is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Profiling. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_PROFILING_OBJECT_INFO_HH
#  define CC_PROFILING_OBJECT_INFO_HH

#  include <string>
#  include "com/centreon/profiling/namespace.hh"

CC_PROFILING_BEGIN()

/**
 *  @class object_info object_info.hh "com/centreon/profiling/object_info.hh"
 *  @brief Store object_info.
 *
 *  Store object informations.
 */
class                object_info {
public:
  enum               command_type {
    command = 0,
    event_handler,
    global_event_handler,
    notification
  };

                     object_info(
                       unsigned int state,
                       unsigned int check_type,
                       double execution_time,
                       double latency,
                       command_type type,
                       bool is_timeout);
                     object_info(object_info const& right);
                     ~object_info() throw ();
  object_info&       operator=(object_info const& right);
  unsigned int       check_type() const throw ();
  double             execution_time() const throw ();
  command_type       execution_type() const throw ();
  bool               is_timeout() const throw ();
  double             latency() const throw ();
  unsigned int       state() const throw ();

private:
  unsigned int       _check_type;
  double             _execution_time;
  command_type       _execution_type;
  bool               _is_timeout;
  double             _latency;
  unsigned int       _state;
};

CC_PROFILING_END()

#endif // !CC_PROFILING_OBJECT_INFO_HH
