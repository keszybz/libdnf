/*
Copyright Contributors to the libdnf project.

This file is part of libdnf: https://github.com/rpm-software-management/libdnf/

Libdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

Libdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libdnf.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBDNF_CLI_EXCEPTION_HPP
#define LIBDNF_CLI_EXCEPTION_HPP

#include "exit-codes.hpp"

#include "libdnf/common/exception.hpp"

namespace libdnf::cli {

class CommandExecutionError : public Error {
public:
    /// Constructs the error with given error code and without any specific message.
    ///
    /// @param error_code The error code, defaults to libdnf::cli::ExitCode::ERROR
    explicit CommandExecutionError(int error_code = static_cast<int>(ExitCode::ERROR))
        : Error("Command failed"),
          error_code(error_code),
          message_set(false) {}

    /// Constructs the error with given error code and with custom formated error
    /// message.
    ///
    /// @param error_code The error code
    /// @format error_code The format string for the message
    /// @args The format arguments
    template <typename... Ss>
    CommandExecutionError(int error_code, const std::string & format, Ss &&... args)
        : Error(format, std::forward<Ss>(args)...),
          error_code(error_code),
          message_set(true) {}

    const char * get_domain_name() const noexcept override { return "libdnf::cli"; }
    const char * get_name() const noexcept override { return "CommandExecutionError"; }

    /// @return The error code
    int get_error_code() const noexcept { return error_code; }

    /// @return Returns `true` if custom error message is set
    bool get_message_set() const noexcept { return message_set; }

private:
    int error_code;
    bool message_set;
};

}  // namespace libdnf::cli

#endif
