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

#ifndef LIBDNF_REPO_REPO_GPGME_HPP
#define LIBDNF_REPO_REPO_GPGME_HPP

#include "libdnf/base/base_weak.hpp"
#include "libdnf/common/exception.hpp"
#include "libdnf/repo/repo_callbacks.hpp"
#include "libdnf/repo/config_repo.hpp"

#include <string>
#include <vector>


namespace libdnf::repo {

/// Wraps gpgme in a higher-level interface.
/// @exception RepoGpgError (public) Thrown on any gpgme-related error.
class RepoGpgme {
public:
    RepoGpgme(const BaseWeakPtr & base, const ConfigRepo & config);

    void set_callbacks(RepoCallbacks * callbacks) noexcept { this->callbacks = callbacks; }

    std::string get_keyring_dir() { return config.get_cachedir() + "/pubring"; }

    void import_key(int fd, const std::string & url);

private:
    BaseWeakPtr base;
    const ConfigRepo & config;
    std::vector<std::string> known_keys;
    RepoCallbacks * callbacks = nullptr;
};

}  // namespace libdnf::repo

#endif  // LIBDNF_REPO_REPO_GPGME_HPP
