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

#ifndef CC_PROFILING_CALLBACK_HH
#  define CC_PROFILING_CALLBACK_HH

#  include <map>
#  include "com/centreon/profiling/namespace.hh"

CC_PROFILING_BEGIN()

/**
 *  @class callback callback.hh "com/centreon/profiling/callback.hh"
 *  @brief Manager NEB callbacks.
 *
 *  Handle callback registration/deregistration.
 */
class              callback {
public:
  static void      load(void* handle);
  static void      unload() throw ();

private:
                   callback(void* handle);
                   callback(callback const& right);
                   ~callback() throw ();
  callback&        operator=(callback const& right);
  void             _add(
                     int id,
                     int (*func)(int, void*));
  static int       _callback_event_handler_process(
                     int type,
                     void* data) throw ();
  static int       _callback_external_command(
                     int type,
                     void* data) throw ();
  static int       _callback_host_process(
                     int type,
                     void* data) throw ();
  static int       _callback_service_process(
                     int type,
                     void* data) throw ();
  void             _register_callbacks();
  void             _unregister_callbacks();

  std::map<int, int (*)(int, void*)>
                   _callbacks;
  void*            _handle;
  bool             _is_enable;
};

CC_PROFILING_END()

#endif // !CC_PROFILING_CALLBACK_HH
