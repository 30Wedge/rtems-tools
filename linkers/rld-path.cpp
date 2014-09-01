/*
 * Copyright (c) 2011-2014, Chris Johns <chrisj@rtems.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rld.h>

namespace rld
{
  namespace path
  {
    std::string
    basename (const std::string& name)
    {
      size_t b = name.find_last_of (RLD_PATH_SEPARATOR);
      if (b != std::string::npos)
        return name.substr (b + 1);
      return name;
    }

    std::string
    dirname (const std::string& name)
    {
      size_t b = name.find_last_of (RLD_PATH_SEPARATOR);
      if (b != std::string::npos)
        return name.substr (0, b - 1);
      return name;
    }

    std::string
    extension (const std::string& name)
    {
      size_t b = name.find_last_of ('.');
      if (b != std::string::npos)
        return name.substr (b);
      return name;
    }

    void
    path_split (const std::string& path, paths& paths)
    {
      strings ps;
      rld::split (path, ps, RLD_PATHSTR_SEPARATOR);
      if (ps.size ())
      {
        for (strings::iterator psi = ps.begin ();
             psi != ps.end ();
             ++psi)
        {
          if (check_directory (*psi))
            paths.push_back (*psi);
        }
      }
    }

    void
    path_join (const std::string& path_, const std::string& file_, std::string& joined)
    {
      if ((path_[path_.size () - 1] != RLD_PATH_SEPARATOR) &&
          (file_[0] != RLD_PATH_SEPARATOR))
        joined = path_ + RLD_PATH_SEPARATOR + file_;
      else if ((path_[path_.size () - 1] == RLD_PATH_SEPARATOR) &&
               (file_[0] == RLD_PATH_SEPARATOR))
        joined = path_ + &file_[1];
      else
        joined = path_ + file_;
    }

    bool
    check_file (const std::string& path)
    {
      struct stat sb;
      if (::stat (path.c_str (), &sb) == 0)
        if (S_ISREG (sb.st_mode))
          return true;
      return false;
    }

    bool
    check_directory (const std::string& path)
    {
      struct stat sb;
      if (::stat (path.c_str (), &sb) == 0)
        if (S_ISDIR (sb.st_mode))
          return true;
      return false;
    }

    void
    find_file (std::string& path, const std::string& name, paths& search_paths)
    {
      for (paths::iterator pi = search_paths.begin ();
           pi != search_paths.end ();
           ++pi)
      {
        path_join (*pi, name, path);
        if (check_file (path))
          return;
      }
      path.clear ();
    }

  }
}
