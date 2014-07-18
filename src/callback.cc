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

#include <cstddef>
#include <stdexcept>
#include <cstring>
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/logging.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/profiling/callback.hh"
#include "com/centreon/profiling/statistics.hh"

using namespace com::centreon::profiling;

// Singleton.
static callback* _instance = NULL;

// list of external command handler.
static struct {
  char const* command;
  void (statistics::*func)(time_t, std::string const&);
} gl_external_command[] = {
  { "_PROFILING_ENABLE", &statistics::cleanup },
  { "_PROFILING_DISABLE", &statistics::cleanup },
  { "_PROFILING_SAVE_GLOBAL_STATISTICS", &statistics::save_global_statistics },
  { "_PROFILING_SAVE_HOST_STATISTICS", &statistics::save_host_statistics },
  { "_PROFILING_SAVE_SERVICE_STATISTICS", &statistics::save_service_statistics }
};

/**
 *  Load callback manager.
 *
 *  @param[in] handle  The handle to manage callbacks.
 */
void callback::load(void* handle) {
  if (!_instance)
    _instance = new callback(handle);
}

/**
 *  Unload callback manager.
 */
void callback::unload() throw () {
  delete _instance;
  _instance = NULL;
}

/**
 *  constructor.
 */
callback::callback(void* handle)
  : _handle(handle),
    _is_enable(false) {
  _register_callbacks();
}

/**
 *  Destructor.
 */
callback::~callback() throw () {
  _unregister_callbacks();
}

/**
 *  Add callback.
 *
 *  @param[in] id       Callback ID.
 *  @param[in] handle   Module handle.
 *  @param[in] function Callback function.
 */
void callback::_add(
                 int id,
                 int (*func)(int, void*)) {
  if (_callbacks.find(id) == _callbacks.end()) {
    _callbacks[id] = func;
    if (neb_register_callback(id, _handle, 0, func))
      throw (std::runtime_error("profiling: "
                                "callback registration failed"));
  }
}

/**
 *  This function is called by the monitoring engine when some external
 *  command is received.
 *
 *  @param[in] type  Type of the callback
 *                   (NEBCALLBACK_EXTERNALCOMMAND_DATA).
 *  @param[in] d     A pointer to a nebstruct_externalcommand_data
 *                   containing the external command data.
 *
 *  @return 0 on success.
 */
int callback::_callback_external_command(int type, void* d) throw () {
  (void)type;

  try {
    // check if data is a valid pointer.
    if (!d)
      throw (std::invalid_argument("data is null pointer"));

    // get external command data.
    nebstruct_external_command_data&
      data(*static_cast<nebstruct_external_command_data*>(d));

    // check if we need to execute external command.
    if (data.type != NEBTYPE_EXTERNALCOMMAND_START)
      return (0);
    // check if this external command is a custom command.
    if (data.command_type != CMD_CUSTOM_COMMAND)
      return (0);

    if (!_instance->_is_enable
        && !strcmp(data.command_string, "_PROFILING_ENABLE")) {
      _instance->_add(
        NEBCALLBACK_HOST_CHECK_DATA,
        &_callback_host_process);
      _instance->_add(
        NEBCALLBACK_SERVICE_CHECK_DATA,
        &_callback_service_process);
      _instance->_add(
        NEBCALLBACK_EVENT_HANDLER_DATA,
        &_callback_event_handler_process);
      _instance->_is_enable = true;
    }
    else if (_instance->_is_enable
             && !strcmp(data.command_string, "_PROFILING_DISABLE")) {
      _instance->_unregister_callbacks();
      _instance->_register_callbacks();
      _instance->_is_enable = false;
    }

    // execute custom external command.
    for (unsigned int i(0);
         i < sizeof(gl_external_command) / sizeof(gl_external_command[0]);
         ++i)
      if (!strcmp(gl_external_command[i].command, data.command_string)) {
        log_debug_info(
          DEBUGL_FUNCTIONS,
          DEBUGV_MORE,
          "profiling: callback external command: %s\n",
          data.command_string);

        (statistics::instance().*gl_external_command[i].func)(
          data.entry_time,
          data.command_args);
        break;
      }
  }
  catch (std::exception const& e) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from external command callback: %s",
      e.what());
  }
  catch (...) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from external command callback: catch all");
  }
  return (0);
}

/**
 *  This function is called by the monitoring engine when event handler
 *  is processed.
 *
 *  @param[in] type  Type of the callback (NEBCALLBACK_EVENT_HANDLER_DATA).
 *  @param[in] d     A pointer to a nebstruct_host_check_data
 *                   containing the process data.
 *
 *  @return 0 on success.
 */
int callback::_callback_event_handler_process(int type, void* d) throw () {
  (void)type;

  try {
    // check if data is a valid pointer.
    if (!d)
      throw (std::invalid_argument("data is null pointer"));

    // get process data.
    nebstruct_event_handler_data&
      data(*static_cast<nebstruct_event_handler_data*>(d));

    // check if we need to execute callback.
    if (data.type != NEBTYPE_EVENTHANDLER_END)
      return (0);

    log_debug_info(
      DEBUGL_FUNCTIONS,
      DEBUGV_MORE,
      "profiling: callback event handler process");

    statistics::instance().event_handler_processed(data);
  }
  catch (std::exception const& e) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: %s",
      e.what());
  }
  catch (...) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: catch all");
  }
  return (0);
}

/**
 *  This function is called by the monitoring engine when host check
 *  is processed.
 *
 *  @param[in] type  Type of the callback (NEBCALLBACK_HOST_CHECK_DATA).
 *  @param[in] d     A pointer to a nebstruct_host_check_data
 *                   containing the process data.
 *
 *  @return 0 on success.
 */
int callback::_callback_host_process(int type, void* d) throw () {
  (void)type;

  try {
    // check if data is a valid pointer.
    if (!d)
      throw (std::invalid_argument("data is null pointer"));

    // get process data.
    nebstruct_host_check_data&
      data(*static_cast<nebstruct_host_check_data*>(d));

    // check if we need to execute callback.
    if (data.type != NEBTYPE_HOSTCHECK_PROCESSED)
      return (0);

    log_debug_info(
      DEBUGL_FUNCTIONS,
      DEBUGV_MORE,
      "profiling: callback host process");

    statistics::instance().host_processed(data);
  }
  catch (std::exception const& e) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: %s",
      e.what());
  }
  catch (...) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: catch all");
  }
  return (0);
}

/**
 *  This function is called by the monitoring engine when service check
 *  is processed.
 *
 *  @param[in] type  Type of the callback (NEBCALLBACK_SERVICE_CHECK_DATA).
 *  @param[in] d     A pointer to a nebstruct_service_check_data
 *                   containing the process data.
 *
 *  @return 0 on success.
 */
int callback::_callback_service_process(int type, void* d) throw () {
  (void)type;

  try {
    // check if data is a valid pointer.
    if (!d)
      throw (std::invalid_argument("data is null pointer"));

    // get process data.
    nebstruct_service_check_data&
      data(*static_cast<nebstruct_service_check_data*>(d));

    // check if we need to execute callback.
    if (data.type != NEBTYPE_SERVICECHECK_PROCESSED)
      return (0);

    log_debug_info(
      DEBUGL_FUNCTIONS,
      DEBUGV_MORE,
      "profiling: callback service process");

    statistics::instance().service_processed(data);
  }
  catch (std::exception const& e) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: %s",
      e.what());
  }
  catch (...) {
    logit(
      NSLOG_RUNTIME_ERROR,
      false,
      "profiling: error from process callback: catch all");
  }
  return (0);
}

/**
 *  Register callback.
 */
void callback::_register_callbacks() {
  try {
    log_debug_info(
      DEBUGL_FUNCTIONS,
      DEBUGV_MORE,
      "profiling: register callbacks\n");

    _add(NEBCALLBACK_EXTERNAL_COMMAND_DATA, &_callback_external_command);
  }
  catch (std::exception const& e) {
    (void)e;
    _unregister_callbacks();
    throw;
  }
}

/**
 *  Unregister callback.
 */
void callback::_unregister_callbacks() {
  log_debug_info(
    DEBUGL_FUNCTIONS,
    DEBUGV_MORE,
    "profiling: unregister callbacks\n");

  for (std::map<int, int (*)(int, void*)>::const_iterator
         it(_callbacks.begin()), end(_callbacks.end());
       it != end;
       ++it)
    neb_deregister_callback(it->first, it->second);
  _callbacks.clear();
}
