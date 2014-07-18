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

#include <cstring>
#include "com/centreon/profiling/utils.hh"

using namespace com::centreon::profiling;

/**
 *  Get bool string.
 *
 *  @param[in] state  The bool to stringify.
 *
 *  @return Booleen string.
 */
char const* utils::bool_to_str(bool b) throw () {
  return (b ? "true" : "false");
}

/**
 *  Get check type string.
 *
 *  @return The check type string.
 */
char const* utils::check_type_to_str(unsigned int type) throw () {
  return (type ? "PASSIVE" : "ACTIVE");
}

/**
 *  Get the command type string.
 *
 *  @param[in] type The command type.
 *
 *  @return The command type string.
 */
char const* utils::command_type_to_str(
              object_info::command_type type) throw () {
  static char const* tab_str[] = {
    "command",
    "event_handler",
    "global_event_handler",
    "notification"
  };
  return (tab_str[type]);
}

/**
 *  Get the time now.
 *
 *  @return String with time now.
 */
std::string utils::now() {
  time_t now(time(NULL));
  char time_buf[32];
  time_buf[0] = 0;
  if (ctime_r(&now, time_buf))
    time_buf[strlen(time_buf) - 1] = 0;
  return (time_buf);
}

/**
 *  Get the host state string.
 *
 *  @param[in] state  The state to stringify.
 *
 *  @return host state string.
 */
char const* utils::host_state_to_str(unsigned int state) throw () {
  static char const* tab_str[] = {
    "UP",
    "DOWN",
    "UNREACHABLE"
  };
  char const* state_str("UP");
  if (state < sizeof(tab_str) / sizeof(*tab_str))
    state_str = tab_str[state];
  return (state_str);
}

/**
 *  Get the serivce state string.
 *
 *  @param[in] state  The bool to stringify.
 *
 *  @return service state string.
 */
char const* utils::service_state_to_str(unsigned int state) throw () {
  static char const* tab_str[] = {
    "OK",
    "WARNING",
    "CRITICAL"
  };
  char const* state_str("UNKWOWN");
  if (state < sizeof(tab_str) / sizeof(*tab_str))
    state_str = tab_str[state];
  return (state_str);
}

/**
 *  Split external command arguments.
 *
 *  @param[in]  args  String with ';' to split arguments.
 *  @param[out] res   Splited array.
 *
 *  @return The number of argument to split.
 */
unsigned int utils::split(
               std::string const& args,
               std::vector<std::string>& res,
               unsigned int sep) {
  res.clear();
  size_t start(0);
  size_t end(0);
  while ((end = args.find(sep, start)) != std::string::npos) {
    res.push_back(std::string(args, start, end - start));
    start = end + 1;
  }
  res.push_back(std::string(args, start));
  return (res.size());
}

/**
 *  Trim a string.
 *
 *  @param[in] str The string.
 *
 *  @return The trimming stream.
 */
std::string& utils::trim(std::string& data) {
  static char const* whitespaces(" \t\r\n");
  size_t pos(data.find_last_not_of(whitespaces));

  if (pos == std::string::npos)
    data.clear();
  else {
    data.erase(pos + 1);
    pos = data.find_first_not_of(whitespaces);
    if (pos != std::string::npos)
      data.erase(0, pos);
  }
  return (data);
}
