/*
Copyright Contributors to the libdnf project.

This file is part of libdnf: https://github.com/rpm-software-management/libdnf/

Libdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Libdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libdnf.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "install.hpp"

#include "microdnf/context.hpp"

#include "libdnf-cli/exception.hpp"
#include "libdnf-cli/output/transaction_table.hpp"

#include <libdnf/base/goal.hpp>
#include <libdnf/conf/option_string.hpp>
#include <libdnf/rpm/package.hpp>
#include <libdnf/rpm/package_query.hpp>
#include <libdnf/rpm/package_set.hpp>
#include <libdnf/rpm/transaction.hpp>

#include <filesystem>
#include <iostream>


namespace fs = std::filesystem;


namespace microdnf {


using namespace libdnf::cli;


InstallCommand::InstallCommand(Command & parent) : Command(parent, "install") {
    auto & ctx = static_cast<Context &>(get_session());
    auto & parser = ctx.get_argument_parser();

    auto & cmd = *get_argument_parser_command();
    cmd.set_short_description("Install software");

    auto keys =
        parser.add_new_positional_arg("keys_to_match", ArgumentParser::PositionalArg::UNLIMITED, nullptr, nullptr);
    keys->set_short_description("List of keys to match");
    keys->set_parse_hook_func(
        [this]([[maybe_unused]] ArgumentParser::PositionalArg * arg, int argc, const char * const argv[]) {
            parse_add_specs(argc, argv, pkg_specs, pkg_file_paths);
            return true;
        });
    keys->set_complete_hook_func([&ctx](const char * arg) { return match_specs(ctx, arg, false, true, true, false); });
    cmd.register_positional_arg(keys);
}


void InstallCommand::run() {
    auto & ctx = static_cast<Context &>(get_session());

    ctx.load_repos(true);

    std::vector<std::string> error_messages;
    const auto cmdline_packages = ctx.add_cmdline_packages(pkg_file_paths, error_messages);
    for (const auto & msg : error_messages) {
        std::cout << msg << std::endl;
    }

    std::cout << std::endl;

    libdnf::Goal goal(ctx.base);
    for (const auto & pkg : cmdline_packages) {
        goal.add_rpm_install(pkg);
    }
    for (const auto & spec : pkg_specs) {
        goal.add_rpm_install(spec);
    }

    auto transaction = goal.resolve(false);
    if (transaction.get_problems() != libdnf::GoalProblem::NO_PROBLEM) {
        throw CommandExecutionError();
    }

    if (!libdnf::cli::output::print_transaction_table(transaction)) {
        return;
    }

    if (!userconfirm(ctx.base.get_config())) {
        throw CommandExecutionError(1, "Operation aborted.");
    }

    ctx.download_and_run(transaction);
}


}  // namespace microdnf
