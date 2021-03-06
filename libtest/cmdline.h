/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <spawn.h>

// http://www.gnu.org/software/automake/manual/automake.html#Using-the-TAP-test-protocol
#define EXIT_SKIP 77
#define EXIT_FATAL 77

namespace libtest {

class Application {
private:
  typedef std::vector< std::pair<std::string, std::string> > Options;

public:

  enum error_t {
    SUCCESS= EXIT_SUCCESS,
    FAILURE= EXIT_FAILURE,
    INVALID= 127
  };

  class Pipe {
  public:
    Pipe(int);
    ~Pipe();

    int fd();

    enum close_t {
      READ= 0,
      WRITE= 1
    };

    void reset();
    void close(const close_t& arg);
    void dup_for_spawn(posix_spawn_file_actions_t& file_actions);

    void nonblock();
    void cloexec();
    bool read(libtest::vchar_t&);

  private:
    const int _std_fd;
    int _pipe_fd[2];
    bool _open[2];
  };

public:
  Application(const std::string& arg, const bool _use_libtool_arg= false);

  virtual ~Application();

  void add_option(const std::string&);
  void add_option(const std::string&, const std::string&);
  void add_long_option(const std::string& option_name, const std::string& option_value);
  error_t run(const char *args[]= NULL);
  error_t wait(bool nohang= true);

  libtest::vchar_t stdout_result() const
  {
    return _stdout_buffer;
  }

  size_t stdout_result_length() const
  {
    return _stdout_buffer.size();
  }

  libtest::vchar_t stderr_result() const
  {
    return _stderr_buffer;
  }

  const char* stderr_c_str() const
  {
    return &_stderr_buffer[0];
  }

  size_t stderr_result_length() const
  {
    return _stderr_buffer.size();
  }

  std::string print();

  void use_valgrind(bool arg= true)
  {
    _use_valgrind= arg;
  }

  bool check() const;

  bool slurp();
  void murder();

  void use_gdb(bool arg= true)
  {
    _use_gdb= arg;
  }

  void use_ptrcheck(bool arg= true)
  {
    _use_ptrcheck= arg;
  }

  std::string arguments();

  std::string gdb_filename()
  {
    return  _gdb_filename;
  }

  pid_t pid() const
  {
    return _pid;
  }

  void will_fail()
  {
    _will_fail= true;
  }

private:
  void create_argv(const char *args[]);
  void delete_argv();

private:
  const bool _use_libtool;
  bool _use_valgrind;
  bool _use_gdb;
  bool _use_ptrcheck;
  bool _will_fail;
  size_t _argc;
  std::string _exectuble_name;
  std::string _exectuble;
  std::string _exectuble_with_path;
  std::string _gdb_filename;
  Options _options;
  Pipe stdin_fd;
  Pipe stdout_fd;
  Pipe stderr_fd;
  char * * built_argv;
  pid_t _pid;
  libtest::vchar_t _stdout_buffer;
  libtest::vchar_t _stderr_buffer;
};

static inline std::ostream& operator<<(std::ostream& output, const enum Application::error_t &arg)
{
  switch (arg)
  {
    case Application::SUCCESS:
      output << "EXIT_SUCCESS";
      break;

    case Application::FAILURE:
      output << "EXIT_FAILURE";
      break;

    case Application::INVALID:
      output << "127";
      break;

    default:
      output << "EXIT_UNKNOWN";
  }

  return output;
}

int exec_cmdline(const std::string& executable, const char *args[], bool use_libtool= false);

const char *gearmand_binary(); 
const char *drizzled_binary();

}
