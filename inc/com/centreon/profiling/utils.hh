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

#ifndef CC_PROFILING_UTILS_HH
#  define CC_PROFILING_UTILS_HH

#  include <string>
#  include <vector>
#  include "com/centreon/profiling/namespace.hh"
#  include "com/centreon/profiling/object_info.hh"

CC_PROFILING_BEGIN()

namespace      utils {
  char const*  bool_to_str(bool b) throw ();
  char const*  check_type_to_str(unsigned int type) throw ();
  char const*  command_type_to_str(
                 object_info::command_type type) throw ();
  std::string  now();
  char const*  host_state_to_str(unsigned int state) throw ();
  char const*  service_state_to_str(unsigned int state) throw ();
  unsigned int split(
                 std::string const& args,
                 std::vector<std::string>& res,
                 unsigned int sep = ';');
  std::string& trim(std::string& data);
}

CC_PROFILING_END()

#endif // !CC_PROFILING_UTILS_HH
