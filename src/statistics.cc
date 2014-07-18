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

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <sys/time.h>
#include "com/centreon/engine/macros.hh"
#include "com/centreon/engine/macros/grab.hh"
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/logging.hh"
#include "com/centreon/engine/macros/grab_host.hh"
#include "com/centreon/engine/macros/grab_service.hh"
#include "com/centreon/engine/sehandlers.hh"
#include "com/centreon/profiling/statistics.hh"
#include "com/centreon/profiling/utils.hh"

using namespace com::centreon::profiling;

// Singleton.
static statistics* _instance = NULL;

/**
 *  Clear all internal informations.
 *
 *  @param[in] entry_time  Unused.
 *  @param[in] args        Unused.
 */
void statistics::cleanup(
       time_t entry_time,
       std::string const& args) {
  (void)entry_time;
  (void)args;

  _hosts.clear();
  _services.clear();
}

/**
 *  Get the queue limit statistics.
 *
 *  @return The limit, 0 is unlimited.
 */
unsigned int statistics::get_limit() const throw () {
  return (_limit);
}

/**
 *  The callback use when event handler execution has finish.
 *
 *  @param[in] ds  The data struct this event handler execution
 *                 informations.
 */
void statistics::event_handler_processed(
       nebstruct_event_handler_data const& ds) {
  if (!ds.object_ptr)
    return;

  object_info::command_type cmdtype;
  std::deque<object_info>* q(NULL);

  if (ds.eventhandler_type == SERVICE_EVENTHANDLER) {
    q = &_services[static_cast<service*>(ds.object_ptr)];
    cmdtype = object_info::event_handler;
  }
  else if (ds.eventhandler_type == HOST_EVENTHANDLER) {
    q = &_hosts[static_cast<host*>(ds.object_ptr)];
    cmdtype = object_info::event_handler;
  }
  else if (ds.eventhandler_type == GLOBAL_SERVICE_EVENTHANDLER) {
    q = &_services[static_cast<service*>(ds.object_ptr)];
    cmdtype = object_info::global_event_handler;
  }
  else if (ds.eventhandler_type == GLOBAL_HOST_EVENTHANDLER) {
    q = &_hosts[static_cast<host*>(ds.object_ptr)];
    cmdtype = object_info::global_event_handler;
  }

  if (q) {
    object_info info(
                  ds.state,
                  0, // ACTIVE
                  ds.execution_time,
                  0.0,
                  cmdtype,
                  ds.early_timeout);
    q->push_back(info);
    if (_limit && q->size() > _limit)
      q->pop_front();
  }
}


/**
 *  The callback use when host execution has finish.
 *
 *  @param[in] ds  The data struct this host execution informations.
 */
void statistics::host_processed(
       nebstruct_host_check_data const& ds) {
  if (ds.object_ptr) {
    host* hst(static_cast<host*>(ds.object_ptr));
    object_info info(
                  ds.state,
                  ds.check_type,
                  ds.execution_time,
                  ds.latency,
                  object_info::command,
                  ds.early_timeout);
    std::deque<object_info>& q(_hosts[hst]);
    q.push_back(info);
    if (_limit && q.size() > _limit)
      q.pop_front();
  }
}

/**
 *  Get singleton instance.
 *
 *  @return Statistics manager instance.
 */
statistics& statistics::instance() throw () {
  return (*_instance);
}

/**
 *  Load statistics manager.
 */
void statistics::load() {
  if (!_instance)
    _instance = new statistics;
}

/**
 *  Save global statistics.
 *
 *  @param[in] entry_time  Time of the command was send.
 *  @param[in] args        The external command arguments.
 *                         (path;sortby;filter;limit).
 */
void statistics::save_global_statistics(
       time_t entry_time,
       std::string const& args) {
  (void)entry_time;

  std::vector<std::string> tab;
  if (utils::split(args, tab) != 4)
    throw (std::invalid_argument("save failed: "
             "invalid number of arguments"));

  filter f(_build_filter(tab[2]));
  sort::sortby algo(_build_sortby(tab[1]));

  std::deque<aggregation> lst;
  _add_host_to_list(_hosts, f, lst);
  _add_service_to_list(_services, f, lst);

  sort sf(algo);
  std::sort(lst.begin(), lst.end(), sf);

  unsigned int limit(strtol(tab[3].c_str(), NULL, 10));
  _dump_global_statistics(tab[0], lst, limit);
}

/**
 *  Save host statistics.
 *
 *  @param[in] entry_time  Time of the command was send.
 *  @param[in] args        The external command arguments.
 *                         (path;host_name).
 */
void statistics::save_host_statistics(
       time_t entry_time,
       std::string const& args) {
  (void)entry_time;

  std::vector<std::string> tab;
  unsigned int size(utils::split(args, tab));
  if (size != 2)
    throw (std::invalid_argument("save host failed: "
             "invalid number of arguments"));

  std::string const& path(tab[0]);
  std::string const& host_name(tab[1]);

  std::map<host*, std::deque<object_info> >::const_iterator
    it(_hosts.begin());
  for (std::map<host*, std::deque<object_info> >::const_iterator
         end(_hosts.end());
       it != end;
       ++it)
    if (it->first->name == host_name)
      break;
  if (it == _hosts.end())
    throw (std::invalid_argument("save host failed: "
             "invalid arguments: host name not found"));
  _dump_object_statistics(path, *it->first, it->second);
}

/**
 *  Save global statistics.
 *
 *  @param[in] entry_time  Time of the command was send.
 *  @param[in] args        The external command arguments.
 *                         (path;host_name;service_description).
 */
void statistics::save_service_statistics(
       time_t entry_time,
       std::string const& args) {
  (void)entry_time;

  std::vector<std::string> tab;
  unsigned int size(utils::split(args, tab));
  if (size != 3)
    throw (std::invalid_argument("save service failed: "
             "invalid number of arguments"));

  std::string const& path(tab[0]);
  std::string const& host_name(tab[1]);
  std::string const& service_description(tab[2]);

  std::map<service*, std::deque<object_info> >::const_iterator
    it(_services.begin());
  for (std::map<service*, std::deque<object_info> >::const_iterator
         end(_services.end());
       it != end;
       ++it)
    if (it->first->host_name == host_name
        && it->first->description == service_description)
      break;
  if (it == _services.end())
    throw (std::invalid_argument("save service failed: "
             "invalid arguments: service name not found"));
  _dump_object_statistics(path, *it->first, it->second);
}

/**
 *  The callback use when service execution has finish.
 *
 *  @param[in] ds  The data struct this service execution informations.
 */
void statistics::service_processed(
       nebstruct_service_check_data const& ds) {
  if (ds.object_ptr) {
    service* svc(static_cast<service*>(ds.object_ptr));
    object_info info(
                  ds.state,
                  ds.check_type,
                  ds.execution_time,
                  ds.latency,
                  object_info::command,
                  ds.early_timeout);
    std::deque<object_info>& q(_services[svc]);
    q.push_back(info);
    if (_limit && q.size() > _limit)
      q.pop_front();
  }
}

/**
 *  Set the queue limit statistics.
 *
 *  @param[in] value  The limit, 0 is unlimited.
 */
void statistics::set_limit(unsigned int value) throw () {
  _limit = value;
}

/**
 *  Unload statistics manager.
 */
void statistics::unload() {
  delete _instance;
  _instance = NULL;
}

/**
 *  Constructor.
 */
statistics::statistics()
  : _limit(0) {

}

/**
 *  Destructor.
 */
statistics::~statistics() throw () {

}

/**
 *  Add host object informations into object list.
 *
 *  @param[in]  ref  The host list with all informations.
 *  @param[in]  f    The filter rules.
 *  @param[out] res  The output result.
 */
void statistics::_add_host_to_list(
       std::map<host*, std::deque<object_info> > const& ref,
       filter const& f,
       std::deque<aggregation>& res) {
  if (f.filter_hosts())
    return;
  for (std::map<host*, std::deque<object_info> >::const_iterator
         it_hst(ref.begin()), end(ref.end());
       it_hst != end;
       ++it_hst) {
    host& hst(*static_cast<host*>(it_hst->first));
    if (!f.is_filter(hst))
      continue;

    aggregation obj(hst);
    bool is_add(false);
    for (std::deque<object_info>::const_iterator
           it(it_hst->second.begin()), end(it_hst->second.end());
         it != end;
         ++it)
      if (f.is_filter(*it)) {
        obj.add_info(*it);
        is_add = true;
      }

    if (is_add)
      res.push_back(obj);
  }
}

/**
 *  Add service object informations into object list.
 *
 *  @param[in]  ref  The service list with all informations.
 *  @param[in]  f    The filter rules.
 *  @param[out] res  The output result.
 */
void statistics::_add_service_to_list(
       std::map<service*, std::deque<object_info> > const& ref,
       filter const& f,
       std::deque<aggregation>& res) {
  if (f.filter_services())
    return;
  for (std::map<service*, std::deque<object_info> >::const_iterator
         it_svc(ref.begin()), end(ref.end());
       it_svc != end;
       ++it_svc) {
    service& svc(*static_cast<service*>(it_svc->first));
    if (!f.is_filter(svc))
      continue;

    aggregation obj(svc);
    bool is_add(false);
    for (std::deque<object_info>::const_iterator
           it(it_svc->second.begin()), end(it_svc->second.end());
         it != end;
         ++it)
      if (f.is_filter(*it)) {
        obj.add_info(*it);
        is_add = true;
      }

    if (is_add)
      res.push_back(obj);
  }
}

/**
 *  Build command line.
 *
 *  @param[in] hst  The host to build command.
 *  @param[in] cmd  The executed command.
 *
 *  @return The command line.
 */
std::string statistics::_build_command_line(
              host const& hst,
              command const* cmd) {
  if (!cmd)
    return ("empty");

  nagios_macros macros;
  memset(&macros, 0, sizeof(macros));
  grab_host_macros_r(&macros, const_cast<host*>(&hst));
  get_raw_command_line_r(
    &macros,
    const_cast<command*>(cmd),
    cmd->name,
    NULL,
    0);

  char* command_line(NULL);
  process_macros_r(
    &macros,
    cmd->command_line,
    &command_line,
    0);
  std::string processed_cmd(command_line);
  delete[] command_line;
  clear_volatile_macros_r(&macros);
  return (processed_cmd);
}

/**
 *  Build command line.
 *
 *  @param[in] svc  The service to build command.
 *  @param[in] cmd  The executed command.
 *
 *  @return The command line.
 */
std::string statistics::_build_command_line(
              service const& svc,
              command const* cmd) {
  if (!cmd)
    return ("empty");

  nagios_macros macros;
  memset(&macros, 0, sizeof(macros));
  grab_service_macros_r(&macros, const_cast<service*>(&svc));
  get_raw_command_line_r(
    &macros,
    const_cast<command*>(cmd),
    cmd->name,
    NULL,
    0);

  char* command_line(NULL);
  process_macros_r(
    &macros,
    cmd->command_line,
    &command_line,
    0);
  std::string processed_cmd(command_line);
  delete[] command_line;
  clear_volatile_macros_r(&macros);
  return (processed_cmd);
}

/**
 *  Create filter struct with string.
 *
 *  @param[in] data  The data to parse.
 *
 *  @return The filter.
 */
filter statistics::_build_filter(std::string data) {
  utils::trim(data);
  std::transform(
    data.begin(),
    data.end(),
    data.begin(),
    (int (*)(int))std::tolower);

  if (data.empty() || data == "all")
    return (filter::build(true, true, true, true, true, true, true));

  bool enable_active(false);
  bool enable_command(false);
  bool enable_event_handler(false);
  bool enable_host(false);
  bool enable_notification(false);
  bool enable_passive(false);
  bool enable_service(false);

  std::vector<std::string> tab;
  utils::split(data, tab, ',');
  for (std::vector<std::string>::const_iterator
         it(tab.begin()), end(tab.end());
       it != end;
       ++it) {
    if (*it == "active")
      enable_active = true;
    else if (*it == "command")
      enable_command = true;
    else if (*it == "event_handler")
      enable_event_handler = true;
    else if (*it == "host")
      enable_host = true;
    else if (*it == "notification")
      enable_notification = true;
    else if (*it == "passive")
      enable_passive = true;
    else if (*it == "service")
      enable_service = true;
    else
      throw (std::runtime_error("parse filter failed: "
               "bad arguments"));
  }
  return (filter::build(
            enable_command,
            enable_event_handler,
            enable_notification,
            enable_active,
            enable_host,
            enable_passive,
            enable_service));
}

/**
 *  Return the sort algorithm to use.
 *
 *  @param[in] data  The to parse.
 */
sort::sortby statistics::_build_sortby(std::string data) {
  utils::trim(data);
  std::transform(
    data.begin(),
    data.end(),
    data.begin(),
    (int (*)(int))std::tolower);

  if (data == "execution_time")
    return (sort::execution_time);
  if (data == "latency")
    return (sort::latency);
  return (sort::id);
}

/**
 *  Write information into file.
 *
 *  @param[in] path   The path to save file statistics.
 *  @param[in] lst    The object list to save.
 *  @param[in] limit  The maximum number object to save.
 */
void statistics::_dump_global_statistics(
       std::string const& path,
       std::deque<aggregation> const& lst,
       unsigned int limit) {
  std::ofstream stream(path.c_str());
  if (!stream.is_open())
    throw (std::runtime_error("dump informations failed: "
             "bad file path"));

  stream << "date" << std::endl;
  stream << utils::now() << std::endl << std::endl;

  stream << std::fixed << std::setprecision(2);
  stream
    << "name\t"
    "check_interval\t"
    "latency\t"
    "total\t"
    "ok/up\t"
    "warning/down\t"
    "critical/unrachable\t"
    "unknown\t"
    << std::endl;
  unsigned int i(0);
  for (std::deque<aggregation>::const_iterator
         it(lst.begin()), end(lst.end());
       it != end && (!limit || i < limit);
       ++it, ++i) {
    unsigned int check_interval(0);
    if (it->is_host()) {
      host const& hst(*static_cast<host const*>(it->object()));
      check_interval = (unsigned int)hst.check_interval;
    }
    else {
      service const& svc(*static_cast<service const*>(it->object()));
      check_interval = (unsigned int)svc.check_interval;
    }

    // write total latency.
    stream
      << it->id() << "\t"
      << check_interval << "\t"
      << it->latency(aggregation::group_min) << "/"
      << it->latency(aggregation::group_avg) << "/"
      << it->latency(aggregation::group_max) << "/"
      << it->latency(aggregation::group_total) << "\t";

    // write total execution time.
    stream
      << it->execution_time(aggregation::group_min) << "/"
      << it->execution_time(aggregation::group_avg) << "/"
      << it->execution_time(aggregation::group_max) << "/"
      << it->execution_time(aggregation::group_total) << "\t";

    // write execution time by state
    for (unsigned int i(0); i < 4; ++i)
      stream
        << it->execution_time(i, aggregation::group_min) << "/"
        << it->execution_time(i, aggregation::group_avg) << "/"
        << it->execution_time(i, aggregation::group_max) << "/"
        << it->execution_time(i, aggregation::group_total) << "\t";
    stream << std::endl;
  }
}

/**
 *  Write information into file.
 *
 *  @param[in] path  The path to save file statistics.
 *  @param[in] hst   The object to save.
 *  @param[in] lst   The data to save.
 */
void statistics::_dump_object_statistics(
       std::string const& path,
       host const& hst,
       std::deque<object_info> const& lst) {
  double total_execution_time(0.0);
  double total_latency(0.0);
  unsigned int total_timeout(0);
  unsigned int total_state_up(0);
  unsigned int total_state_down(0);
  unsigned int total_state_unreachable(0);

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2);
  for (std::deque<object_info>::const_iterator
         it(lst.begin()), end(lst.end());
       it != end;
       ++it) {
    total_execution_time += it->execution_time();
    total_latency += it->latency();
    if (it->is_timeout())
      ++total_timeout;
    if (it->state() == HOST_DOWN)
      ++total_state_down;
    else if (it->state() == HOST_UNREACHABLE)
      ++total_state_unreachable;
    else
      ++total_state_up;

    command const* cmd(NULL);
    if (it->execution_type() == object_info::command)
      cmd = hst.check_command_ptr;
    else if (it->execution_type() == object_info::event_handler)
      cmd = hst.event_handler_ptr;
    else if (it->execution_type() == object_info::global_event_handler)
      cmd = global_host_event_handler_ptr;

    oss << it->execution_time() << "\t"
        << it->latency() << "\t"
        << utils::bool_to_str(it->is_timeout()) << "\t"
        << utils::command_type_to_str(it->execution_type()) << "\t"
        << utils::host_state_to_str(it->state()) << "\t"
        << utils::check_type_to_str(it->check_type()) << "\t"
        << _build_command_line(hst, cmd)
        << std::endl;
  }

  std::ofstream stream(path.c_str());
  if (!stream.is_open())
    throw (std::runtime_error("dump informations failed: "
             "bad file path"));

  stream << std::fixed << std::setprecision(2);
  stream
    << "date\t"
    "name\t"
    "check_interval\t"
    << std::endl;
  stream
    << utils::now() << "\t"
    << hst.name << "\t"
    << (unsigned int)hst.check_interval << "\t"
    << std::endl << std::endl;

  stream
    << "total_execution_time\t"
    "total_latency\t"
    "total_timeout\t"
    "total_up\t"
    "total_down\t"
    "total_unreachable"
    << std::endl;
  stream
    << total_execution_time << "\t"
    << total_latency << "\t"
    << total_timeout << "\t"
    << total_state_up << "\t"
    << total_state_down << "\t"
    << total_state_unreachable
    << std::endl << std::endl;

  stream
    << "execution_time\t"
    "latency\t"
    "is_timeout\t"
    "command_type\t"
    "state\t"
    "check_type\t"
    "command_line"
    << std::endl;
  stream << oss.str();
}

/**
 *  Write information into file.
 *
 *  @param[in] path  The path to save file statistics.
 *  @param[in] hst   The object to save.
 *  @param[in] lst   The data to save.
 */
void statistics::_dump_object_statistics(
       std::string const& path,
       service const& svc,
       std::deque<object_info> const& lst) {
  double total_execution_time(0.0);
  double total_latency(0.0);
  unsigned int total_timeout(0);
  unsigned int total_state_ok(0);
  unsigned int total_state_warning(0);
  unsigned int total_state_critical(0);
  unsigned int total_state_unknown(0);

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2);
  for (std::deque<object_info>::const_iterator
         it(lst.begin()), end(lst.end());
       it != end;
       ++it) {
    total_execution_time += it->execution_time();
    total_latency += it->latency();
    if (it->is_timeout())
      ++total_timeout;
    if (it->state() == STATE_OK)
      ++total_state_ok;
    else if (it->state() == STATE_WARNING)
      ++total_state_warning;
    else if (it->state() == STATE_CRITICAL)
      ++total_state_critical;
    else
      ++total_state_unknown;

    command const* cmd(NULL);
    if (it->execution_type() == object_info::command)
      cmd = svc.check_command_ptr;
    else if (it->execution_type() == object_info::event_handler)
      cmd = svc.event_handler_ptr;
    else if (it->execution_type() == object_info::global_event_handler)
      cmd = global_service_event_handler_ptr;

    oss << it->execution_time() << "\t"
        << it->latency() << "\t"
        << utils::bool_to_str(it->is_timeout()) << "\t"
        << utils::command_type_to_str(it->execution_type()) << "\t"
        << utils::service_state_to_str(it->state()) << "\t"
        << utils::check_type_to_str(it->check_type()) << "\t"
        << _build_command_line(svc, cmd)
        << std::endl;
  }

  std::ofstream stream(path.c_str());
  if (!stream.is_open())
    throw (std::runtime_error("dump informations failed: "
             "bad file path"));

  stream << std::fixed << std::setprecision(2);
  stream
    << "date\t"
    "name\t"
    "check_interval\t"
    << std::endl;
  stream
    << utils::now() << "\t"
    << svc.host_name << "::" << svc.description << "\t"
    << (unsigned int)svc.check_interval << "\t"
    << std::endl << std::endl;

  stream
    << "total_execution_time\t"
    "total_latency\t"
    "total_timeout\t"
    "total_ok\t"
    "total_warning\t"
    "total_critical\t"
    "total_unknown"
    << std::endl;
  stream
    << total_execution_time << "\t"
    << total_latency << "\t"
    << total_timeout << "\t"
    << total_state_ok << "\t"
    << total_state_warning << "\t"
    << total_state_critical << "\t"
    << total_state_unknown
    << std::endl << std::endl;

  stream
    << "execution_time\t"
    "latency\t"
    "is_timeout\t"
    "command_type\t"
    "state\t"
    "check_type\t"
    "command_line"
    << std::endl;
  stream << oss.str();
}
