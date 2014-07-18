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

#ifndef CC_PROFILING_STATISTICS_HH
#  define CC_PROFILING_STATISTICS_HH

#  include <deque>
#  include <map>
#  include <string>
#  include <vector>
#  include "com/centreon/engine/nebstructs.hh"
#  include "com/centreon/engine/objects.hh"
#  include "com/centreon/profiling/aggregation.hh"
#  include "com/centreon/profiling/filter.hh"
#  include "com/centreon/profiling/namespace.hh"
#  include "com/centreon/profiling/object_info.hh"
#  include "com/centreon/profiling/sort.hh"

CC_PROFILING_BEGIN()

/**
 *  @class statistics statistics.hh "com/centreon/profiling/statistics.hh"
 *  @brief Manager NEB statisticss.
 *
 *  Handle statistics to store and dump statistics.
 */
class                 statistics {
public:
  void                cleanup(
                        time_t entry_time,
                        std::string const& args);
  unsigned int        get_limit() const throw ();
  void                event_handler_processed(
                        nebstruct_event_handler_data const& ds);
  void                host_processed(
                        nebstruct_host_check_data const& ds);
  static statistics&  instance() throw ();
  static void         load();
  void                save_global_statistics(
                        time_t entry_time,
                        std::string const& args);
  void                save_host_statistics(
                        time_t entry_time,
                        std::string const& args);
  void                save_service_statistics(
                        time_t entry_time,
                        std::string const& args);
  void                service_processed(
                        nebstruct_service_check_data const& ds);
  void                set_limit(unsigned int value) throw ();
  static void         unload();

private:
                      statistics();
                      statistics(statistics const& right);
                      ~statistics() throw ();
  statistics&         operator=(statistics const& right);
  static void         _add_host_to_list(
                        std::map<host*, std::deque<object_info> > const& ref,
                        filter const& f,
                        std::deque<aggregation>& res);
  static void         _add_service_to_list(
                        std::map<service*, std::deque<object_info> > const& ref,
                        filter const& f,
                        std::deque<aggregation>& res);

  static std::string  _build_command_line(
                        host const& hst,
                        command const* cmd);
  static std::string  _build_command_line(
                        service const& svc,
                        command const* cmd);
  static filter       _build_filter(std::string data);
  static sort::sortby _build_sortby(std::string data);
  static void         _dump_global_statistics(
                        std::string const& path,
                        std::deque<aggregation> const& lst,
                        unsigned int limit);
  static void         _dump_object_statistics(
                        std::string const& path,
                        host const& hst,
                        std::deque<object_info> const& lst);
  static void         _dump_object_statistics(
                        std::string const& path,
                        service const& svc,
                        std::deque<object_info> const& lst);

  std::map<host*, std::deque<object_info> >
                      _hosts;
  unsigned int        _limit;
  std::map<service*, std::deque<object_info> >
                      _services;
};

CC_PROFILING_END()

#endif // !CC_PROFILING_STATISTICS_HH
