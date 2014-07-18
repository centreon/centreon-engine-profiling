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

#include "com/centreon/profiling/object_info.hh"

using namespace com::centreon::profiling;

/**
 *  Constructor.
 *
 *  @param[in] state           The object state.
 *  @param[in] check_type      If the check is active or passive.
 *  @param[in] execution_time  The total time to execute command.
 *  @param[in] latency         The latency before execute command.
 *  @param[in] type            Command type (comman, event_handler).
 *  @param[in] is_timeout      True if the command was stop by timeout.
 */
object_info::object_info(
  unsigned int state,
  unsigned int check_type,
  double execution_time,
  double latency,
  command_type type,
  bool is_timeout)
  : _check_type(check_type),
    _execution_time(execution_time),
    _execution_type(type),
    _is_timeout(is_timeout),
    _latency(latency),
    _state(state) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] right  The object to copy.
 */
object_info::object_info(object_info const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
object_info::~object_info() throw () {

}

/**
 *  Copy operator.
 *
 *  @param[in] right  The object to copy.
 *
 *  @return This object.
 */
object_info& object_info::operator=(object_info const& right) {
  if (this != &right) {
    _check_type = right._check_type;
    _execution_time = right._execution_time;
    _execution_type = right._execution_type;
    _is_timeout = right._is_timeout;
    _latency = right._latency;
    _state = right._state;
  }
  return (*this);
}

/**
 *  Get the check type.
 *
 *  @return The check type.
 */
unsigned int object_info::check_type() const throw () {
  return (_check_type);
}

/**
 *  Get the execution time.
 *
 *  @return The execution time.
 */
double object_info::execution_time() const throw () {
  return (_execution_time);
}

/**
 *  Get the execution type.
 *
 *  @return The execution type.
 */
object_info::command_type object_info::execution_type() const throw () {
  return (_execution_type);
}

/**
 *  Get is timeout.
 *
 *  @return True if timeout.
 */
bool object_info::is_timeout() const throw () {
  return (_is_timeout);
}

/**
 *  Get the latency.
 *
 *  @return The latency.
 */
double object_info::latency() const throw () {
  return (_latency);
}

/**
 *  Get the state.
 *
 *  @return The state.
 */
unsigned int object_info::state() const throw () {
  return (_state);
}
