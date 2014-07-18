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

#include "com/centreon/profiling/sort.hh"

using namespace com::centreon::profiling;

/**
 *  Constructor.
 *
 *  @param[in] s  The sort algorithm.
 */
sort::sort(sortby s) {
  static functor tab_func[] = {
    &sort::_sort_id,
    &sort::_sort_execution_time,
    &sort::_sort_latency,
  };
  _pfunc = tab_func[s];
}

sort::~sort() throw () {

}

/**
 *  Generic sort.
 */
bool sort::operator()(
       aggregation const& a,
       aggregation const& b) const {
  return ((this->*_pfunc)(a, b));
}

/**
 *  Sort id functor.
 */
bool sort::_sort_id(
       aggregation const& a,
       aggregation const& b) const {
  return (a.id() < b.id());
}

/**
 *  Sort execution time functor.
 */
bool sort::_sort_execution_time(
       aggregation const& a,
       aggregation const& b) const {
  double e1(a.execution_time(4, aggregation::group_avg));
  double e2(b.execution_time(4, aggregation::group_avg));
  return (e1 > e2);
}

/**
 *  Sort latency functor.
 */
bool sort::_sort_latency(
       aggregation const& a,
       aggregation const& b) const {
  double l1(a.latency(aggregation::group_avg));
  double l2(b.latency(aggregation::group_avg));
  return (l1 > l2);
}
