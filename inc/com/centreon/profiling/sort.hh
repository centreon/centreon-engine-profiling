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

#ifndef CC_PROFILING_SORT_HH
#  define CC_PROFILING_SORT_HH

#  include "com/centreon/profiling/aggregation.hh"
#  include "com/centreon/profiling/namespace.hh"

CC_PROFILING_BEGIN()

class     sort {
public:
  enum    sortby {
    id = 0,
    execution_time,
    latency
  };

          sort(sortby s);
          ~sort() throw ();
  bool    operator()(
            aggregation const& a,
            aggregation const& b) const;

private:
  typedef bool (sort::*functor)(
                  aggregation const& a,
                  aggregation const& b) const;

  bool    _sort_id(
            aggregation const& a,
            aggregation const& b) const;
  bool    _sort_execution_time(
            aggregation const& a,
            aggregation const& b) const;
  bool    _sort_latency(
            aggregation const& a,
            aggregation const& b) const;

  functor _pfunc;
};

CC_PROFILING_END()

#endif // !CC_PROFILING_SORT_HH
