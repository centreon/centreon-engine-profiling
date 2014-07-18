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

#include <clocale>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include "com/centreon/engine/objects/host.hh"
#include "com/centreon/engine/objects/service.hh"
#include "com/centreon/engine/logging.hh"
#include "com/centreon/engine/nebmodules.hh"
#include "com/centreon/profiling/callback.hh"
#include "com/centreon/profiling/statistics.hh"
#include "com/centreon/profiling/version.hh"

using namespace com::centreon;

// specify the event broker API version.
NEB_API_VERSION(CURRENT_NEB_API_VERSION)

extern "C" {
  /**
   *  This function is called when the module gets unloaded. It will
   *  deregister all previously registered callbacks.
   *
   *  @param[in] flags   Unused.
   *  @param[in] reason  Unused.
   *
   *  @return 0 on success, any other value on failure.
   */
  int nebmodule_deinit(int flags, int reason) {
    int ret(-1);

    (void)flags;
    (void)reason;

    try {
      // unload managers.
      profiling::callback::unload();
      profiling::statistics::unload();

      // deinit success.
      ret = 0;
    }
    catch (std::exception const& e) {
      logit(
        NSLOG_RUNTIME_ERROR,
        false,
        "profiling: error from nebmodule_deinit: %s",
        e.what());
    }
    catch (...) {
      logit(
        NSLOG_RUNTIME_ERROR,
        false,
        "profiling: error from nebmodule_deinit: catch all");
    }
    return (ret);
  }

  /**
   *  This function is called when the module gets loaded. It will
   *  register callbacks.
   *
   *  @param[in] flags   Unused.
   *  @param[in] args    Path to save retention information.
   *  @param[in] handle  The module handle.
   *
   *  @return 0 on success, any other value on failure.
   */
  int nebmodule_init(int flags, char const* args, void* handle) {
    int ret(-1);

    (void)flags;
    (void)args;

    try {
      // check module argument.
      long int max_data_retention(args && args[0] != '\0' ? strtol(args, NULL, 10) : 0);
      if (max_data_retention < 0)
        throw (std::invalid_argument("profiling: "
                 "invalid module argument: "
                 "max data retention"));
      // set module informations.
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_TITLE,
        "Centreon Profiling module");
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_AUTHOR,
        "Merethis");
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_COPYRIGHT,
        "Copyright 2013 Merethis");
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_VERSION,
        CENTREON_PROFILING_VERSION_STRING);
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_LICENSE,
        "GPL version 2");
      neb_set_module_info(
        handle,
        NEBMODULE_MODINFO_DESC,
        "Centreon Profiling.");

      // reset locale.
      setlocale(LC_NUMERIC, "C");

      // load managers.
      profiling::statistics::load();
      profiling::callback::load(handle);
      profiling::statistics::instance()
        .set_limit(max_data_retention);

      // init success.
      ret = 0;
    }
    catch (std::exception const& e) {
      logit(
        NSLOG_RUNTIME_ERROR,
        false,
        "profiling: error from nebmodule_init: %s",
        e.what());
    }
    catch (...) {
      logit(
        NSLOG_RUNTIME_ERROR,
        false,
        "profiling: error from nebmodule_init: catch all");
    }
    return (ret);
  }
}
