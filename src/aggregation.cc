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

#include <cfloat>
#include <cstddef>
#include <cstring>
#include "com/centreon/profiling/aggregation.hh"

using namespace com::centreon::profiling;

/**
 *  Default constructor.
 */
aggregation::aggregation()
  : _is_host(false),
    _obj(NULL) {
  memset(_execution_time, 0, sizeof(_execution_time));
  memset(_latency, 0, sizeof(_latency));
  memset(_total, 0, sizeof(_total));
  _latency[group_min] = DBL_MAX;
  for (unsigned int i(0); i < 4; ++i)
    _execution_time[i][group_min] = DBL_MAX;
}

/**
 *  Constructor.
 *
 *  @param[in] svc  The service to store.
 */
aggregation::aggregation(host& hst)
  : _id(hst.name),
    _is_host(true),
    _obj(&hst) {
  memset(_execution_time, 0, sizeof(_execution_time));
  memset(_latency, 0, sizeof(_latency));
  memset(_total, 0, sizeof(_total));
  _latency[group_min] = DBL_MAX;
  for (unsigned int i(0); i < 4; ++i)
    _execution_time[i][group_min] = DBL_MAX;
}

/**
 *  Constructor.
 *
 *  @param[in] svc  The service to store.
 */
aggregation::aggregation(service& svc)
  : _id(svc.host_name + std::string("::") + svc.description),
    _is_host(false),
    _obj(&svc) {
  memset(_execution_time, 0, sizeof(_execution_time));
  memset(_latency, 0, sizeof(_latency));
  memset(_total, 0, sizeof(_total));
  _latency[group_min] = DBL_MAX;
  for (unsigned int i(0); i < 4; ++i)
    _execution_time[i][group_min] = DBL_MAX;
}

/**
 *  Destructor.
 */
aggregation::~aggregation() throw () {

}

/**
 *  Add object info to object.
 *
 *  @param[in] info  The object info.
 */
void aggregation::add_info(object_info const& info) throw () {
  unsigned int pos(info.state());
  if (pos > 3)
    return;

  _execution_time[pos][group_avg] += info.execution_time();
  if (_execution_time[pos][group_min] > info.execution_time())
    _execution_time[pos][group_min] = info.execution_time();
  if (_execution_time[pos][group_max] < info.execution_time())
    _execution_time[pos][group_max] = info.execution_time();

  _latency[group_avg] += info.latency();
  if (_latency[group_min] > info.latency())
    _latency[group_min] = info.latency();
  if (_latency[group_max] < info.latency())
    _latency[group_max] = info.latency();

  _total[pos] += 1;
}

/**
 *  Get the total execution time.
 *
 *  @param[in] type  The grouping type.
 *
 *  @return The execution time value.
 */
double aggregation::execution_time(grouping type) const throw () {
  double val(0.0);
  grouping type_index = (type == group_total) ? group_avg : type;
  for (unsigned int i(0); i < 4; ++i)
    if (_total[i])
      val += _execution_time[i][type_index];

  if (type == group_avg) {
    unsigned int total(_total_aggregation());
    val = (total ? val / total : 0.0);
  }

  return (val);
}

/**
 *  Get the execution time.
 *
 *  @param[in] state  Get the execution time information
 *                    link to the state.
 *  @param[in] type   The grouping type.
 *
 *  @return The execution time value.
 */
double aggregation::execution_time(
         unsigned int state,
         grouping type) const throw () {
  double val(0.0);
  grouping type_index = (type == group_total) ? group_avg : type;
  if (state < 4) {
    unsigned int total(_total[state]);
    if (total) {
      val = _execution_time[state][type_index];
      if (type == group_avg)
        val = (total ? val / total : 0.0);
    }
  }
  return (val);
}

/**
 *  Get the object id.
 *
 *  @return The id.
 */
std::string const& aggregation::id() const throw () {
  return (_id);
}

/**
 *  Get if object is an host.
 *
 *  @return True if is an host.
 */
bool aggregation::is_host() const throw () {
  return (_is_host);
}

/**
 *  Get the latency.
 *
 *  @param[in] type  The grouping type.
 *
 *  @return The latency value.
 */
double aggregation::latency(grouping type) const throw () {
  double val(0.0);
  grouping type_index = (type == group_total) ? group_avg : type;
  unsigned int total(_total_aggregation());
  if (total) {
    val = _latency[type_index];
    if (type == group_avg)
      val = (total ? val / total : 0.0);
  }
  return (val);
}

/**
 *  Get the object to aggregate informations.
 *
 *  @return Object pointer.
 */
void* aggregation::object() const throw () {
  return (_obj);
}

/**
 *  Get the total number of object aggregation.
 *
 *  @return Total
 */
unsigned int aggregation::_total_aggregation() const throw () {
  unsigned int res(0);
  for (unsigned int i(0); i < 4; ++i)
    res += _total[i];
  return (res);
}
