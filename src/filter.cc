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

#include "com/centreon/profiling/filter.hh"

using namespace com::centreon::profiling;

/**
 *  Default constructor.
 */
filter::filter(
  bool enable_command,
  bool enable_event_handler,
  bool enable_notification,
  bool enable_active,
  bool enable_host,
  bool enable_passive,
  bool enable_service)
  : _command(enable_command),
    _event_handler(enable_event_handler),
    _notification(enable_notification),
    _active(enable_active),
    _host(enable_host),
    _passive(enable_passive),
    _service(enable_service) {

}

/**
 *  Destructor.
 */
filter::~filter() throw () {

}

/**
 *  Build filter.
 *  Try to create a reasonably clever filter. If nothing is set
 *  in a given category (creating a filter that would let nothing pass),
 *  assume that all the category is wanted.
 *
 *  @return filter.
 */
filter filter::build(
         bool enable_command,
         bool enable_event_handler,
         bool enable_notification,
         bool enable_active,
         bool enable_host,
         bool enable_passive,
         bool enable_service) {
  if (!enable_service && !enable_host) {
    enable_service = true;
    enable_host = true;
  }
  if (!enable_active && !enable_passive) {
    enable_active = true;
    enable_passive = true;
  }
  if (!enable_command && !enable_event_handler && !enable_notification) {
    enable_command = true;
    enable_event_handler = true;
    enable_notification = true;
  }
  return (filter(
            enable_command,
            enable_event_handler,
            enable_notification,
            enable_active,
            enable_host,
            enable_passive,
            enable_service));
}

/**
 *  Get is hosts is filtered.
 *
 *  @return True if hosts is filtered.
 */
bool filter::filter_hosts() const throw () {
  return (!_host);
}


/**
 *  Get is services is filtered.
 *
 *  @return True if services is filtered.
 */
bool filter::filter_services() const throw () {
  return (!_service);
}

/**
 *  Get if object info pass the filter.
 *
 *  @param[in] hst  The object to check.
 *
 *  @return True if the object is filter.
 */
bool filter::is_filter(host const& hst) const throw () {
  if (_active && hst.check_type == 0)
    return (true);
  if (_passive && hst.check_type == 1)
    return (true);
  return (false);
}

/**
 *  Get if object info pass the filter.
 *
 *  @param[in] info  The object to check.
 *
 *  @return True if the object is filter.
 */
bool filter::is_filter(object_info const& info) const throw () {
  object_info::command_type type(info.execution_type());
  if (_command && type == object_info::command)
    return (true);
  if (_notification && type == object_info::notification)
    return (true);
  if (_event_handler
      && (type == object_info::event_handler
          || type == object_info::global_event_handler))
    return (true);
  return (false);
}

/**
 *  Get if object info pass the filter.
 *
 *  @param[in] svc  The object to check.
 *
 *  @return True if the object is filter.
 */
bool filter::is_filter(service const& svc) const throw () {
  if (_active && svc.check_type == 0)
    return (true);
  if (_passive && svc.check_type == 1)
    return (true);
  return (false);
}

