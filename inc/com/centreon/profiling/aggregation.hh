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

#ifndef CC_PROFILING_AGGREGATION_HH
#  define CC_PROFILING_AGGREGATION_HH

#  include "com/centreon/engine/objects.hh"
#  include "com/centreon/profiling/namespace.hh"
#  include "com/centreon/profiling/object_info.hh"

CC_PROFILING_BEGIN()

class                aggregation {
public:
  enum               grouping {
    group_min = 0,
    group_avg,
    group_max,
    group_total
   };

                     aggregation();
                     aggregation(host& hst);
                     aggregation(service& svc);
                     ~aggregation() throw ();
  void               add_info(object_info const& info) throw ();
  double             execution_time(grouping type) const throw ();
  double             execution_time(
                       unsigned int pos,
                       grouping type) const throw ();
  std::string const& id() const throw ();
  bool               is_host() const throw ();
  double             latency(grouping type) const throw ();
  void*              object() const throw ();

private:
  unsigned int       _total_aggregation() const throw ();

  double             _execution_time[4][3];
  std::string        _id;
  bool               _is_host;
  double             _latency[3];
  void*              _obj;
  unsigned int       _total[4];
};

CC_PROFILING_END()

#endif // !CC_PROFILING_AGGREGATION_HH
