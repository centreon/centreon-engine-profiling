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

#ifndef CC_PROFILING_FILTER_HH
#  define CC_PROFILING_FILTER_HH

#  include "com/centreon/engine/objects.hh"
#  include "com/centreon/profiling/object_info.hh"

CC_PROFILING_BEGIN()

/**
 *  @class filter filter.hh "com/centreon/profiling/filter.hh"
 *
 *  Store filter rules.
 */
class           filter {
public:
                filter(
                  bool enable_command = false,
                  bool enable_event_handler = false,
                  bool enable_notification = false,
                  bool enable_active = false,
                  bool enable_host = false,
                  bool enable_passive = false,
                  bool enable_service = false);
                ~filter() throw ();
  static filter build(
                  bool enable_command = false,
                  bool enable_event_handler = false,
                  bool enable_notification = false,
                  bool enable_active = false,
                  bool enable_host = false,
                  bool enable_passive = false,
                  bool enable_service = false);
  bool          filter_hosts() const throw ();
  bool          filter_services() const throw ();
  bool          is_filter(service const& svc) const throw ();
  bool          is_filter(object_info const& info) const throw ();
  bool          is_filter(host const& hst) const throw ();

private:
  bool          _command;
  bool          _event_handler;
  bool          _notification;
  bool          _active;
  bool          _host;
  bool          _passive;
  bool          _service;
};

CC_PROFILING_END()

#endif // !CC_PROFILING_FILTER_HH
