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

#ifndef CC_PROFILING_NAMESPACE_HH
#  define CC_PROFILING_NAMESPACE_HH

#  ifndef CC_PROFILING_BEGIN
#    define CC_PROFILING_BEGIN() \
  namespace       com {      \
    namespace     centreon { \
      namespace   profiling {
#  endif // !CC_PROFILING_BEGIN

#  ifndef CC_PROFILING_END
#    define CC_PROFILING_END() } } }
#  endif // !CC_PROFILING_END

#endif // !CC_PROFILING_NAMESPACE_HH
