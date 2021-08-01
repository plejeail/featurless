#include "featurless/test.h"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

enum class FilterType { none = 0, enabled = 1, disabled = 2 };

enum class StatusCode : int8_t { ok = 0, help = -1, argerror = 1, requirefail = 2 };

struct Stats {
    StatusCode status{ StatusCode::ok };
    uint8_t count_checks{ 0 };
    uint8_t count_success{ 0 };
    uint8_t count_total{ 0 };
};

struct featurless::test::Data {
    FilterType filter_type{ FilterType::none };
    Stats global_stats;

    std::unordered_map<std::string, Stats> groups_stats;
    std::vector<std::string> filter_values;
};

featurless::test::test() {
    _data = new Data;
}

featurless::test::~test() {
    delete _data;
}

void featurless::test::parse_args(int argc, const char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string_view current_arg{ argv[i] };
        if (current_arg == "-h" || current_arg == "--help") {
            std::cout
              << "Description: \n"
                 "Execute the featurless tests you just compiled.\n"
                 "Arguments:\n"
                 "\t-h, --help    \tdisplay this help and exit\n"
                 "\t-e, --enable  \tenable only the groups of tests whose name is provided\n"
                 "\t              \tafter this argument.\n"
                 "\t-d, --disable \tdisable only the groups of tests whose name is provided\n"
                 "\t              \tafter this argument.\n";
            _data->global_stats.status = StatusCode::help;
            break;
        } else if (current_arg == "-e" || current_arg == "--enabled") {
            if (_data->filter_type == FilterType::none)
                _data->filter_type = FilterType::enabled;
            else {
                _data->global_stats.status = StatusCode::argerror;
                break;
            }
        } else if (current_arg == "-d" || current_arg == "--disabled") {
            if (_data->filter_type == FilterType::none)
                _data->filter_type = FilterType::disabled;
            else {
                _data->global_stats.status = StatusCode::argerror;
                break;
            }
        } else
            _data->filter_values.emplace_back(current_arg);
    }
}

void featurless::test::add_group(const char* const group_name) {
    if (!_data->groups_stats.contains(group_name)) {
        _data->groups_stats.insert({ group_name, Stats() });
    }
}

inline bool featurless::test::__check(const char* const description, bool condition) {
    if (_data->global_stats.status != StatusCode::ok)
        return false;

    ++_data->global_stats.count_checks;
    if (condition) {
        ++_data->global_stats.count_success;
    } else
        std::cout << std::left << std::setfill('.') << std::setw(line_width) << description
                  << "Failure\n";

    return condition;
}

inline bool featurless::test::__check(const char* const description,
                                      const char* const group_name,
                                      bool condition) {
    Stats& stats = _data->groups_stats[group_name];
    if (_data->global_stats.status != StatusCode::ok || stats.status != StatusCode::ok)
        return false;

    ++stats.count_checks;
    ++_data->global_stats.count_checks;
    if (condition) {
        ++stats.count_success;
        ++_data->global_stats.count_success;
    } else
        std::cout << std::left << std::setfill('.') << std::setw(line_width) << description
                  << "Failure\n";

    return condition;
}

void featurless::test::check(const char* const description, bool condition) {
    ++_data->global_stats.count_total;
    __check(description, condition);
}

void featurless::test::require(const char* const description, bool condition) {
    ++_data->global_stats.count_total;
    bool success = __check(description, condition);
    if (!success && _data->global_stats.status != StatusCode::help)
        _data->global_stats.status = StatusCode::requirefail;
}

void featurless::test::check(const char* const description, featurless::test::testfun_t condition) {
    ++_data->global_stats.count_total;
    __check(description, condition());
}

void featurless::test::require(const char* const description,
                               featurless::test::testfun_t condition) {
    ++_data->global_stats.count_total;
    bool success = __check(description, condition());
    if (!success && _data->global_stats.status != StatusCode::help)
        _data->global_stats.status = StatusCode::requirefail;
}

bool featurless::test::is_group_enabled(const char* const group_name) {
    if (!_data->groups_stats.contains(group_name)) {
        std::cout << "Warning: Unknown group '" << group_name << "', skipped check.\n";
        return false;
    }

    if (_data->filter_type == FilterType::none)
        return true;
    if (_data->groups_stats[group_name].status != StatusCode::ok)
        return false;

    const auto value =
      std::find(_data->filter_values.cbegin(), _data->filter_values.cend(), group_name);

    return (_data->filter_type == FilterType::disabled && value == _data->filter_values.cend())
           || (_data->filter_type == FilterType::enabled && value != _data->filter_values.cend());
}

void featurless::test::check(const char* const group_name,
                             const char* const description,
                             bool condition) {
    ++_data->global_stats.count_total;
    if (is_group_enabled(group_name))
        __check(description, group_name, condition);
}

void featurless::test::require(const char* const group_name,
                               const char* const description,
                               bool condition) {
    ++_data->global_stats.count_total;
    if (is_group_enabled(group_name)) {
        bool success = __check(description, group_name, condition);
        if (!success)
            _data->groups_stats[group_name].status = StatusCode::requirefail;
    }
}

void featurless::test::check(const char* const group_name,
                             const char* const description,
                             featurless::test::testfun_t condition) {
    ++_data->global_stats.count_total;
    if (is_group_enabled(group_name))
        __check(description, group_name, condition());
}

void featurless::test::require(const char* const group_name,
                               const char* const description,
                               featurless::test::testfun_t condition) {
    ++_data->global_stats.count_total;
    if (is_group_enabled(group_name)) {
        bool success = __check(description, group_name, condition());
        if (!success)
            _data->groups_stats[group_name].status = StatusCode::requirefail;
    }
}

static void print_group_summary(const std::string_view group,
                                bool success,
                                uint8_t nb_success,
                                uint8_t nb_checks) {
    std::cout << " - (" << group << ") status: " << (success ? "OK" : "KO") << ", "
              << "successes: " << std::to_string(nb_success) << ", "
              << "checked: " << std::to_string(nb_checks) << '\n';
}

void featurless::test::print_summary() const {
    if (_data->global_stats.status == StatusCode::help
        || _data->global_stats.status == StatusCode::argerror)
        return;

    std::cout << "### TEST GLOBAL SUMMARY:\n"
              << " - successes: " << std::to_string(_data->global_stats.count_success) << '\n'
              << " - checked: " << std::to_string(_data->global_stats.count_checks) << '\n'
              << " - total checks: " << std::to_string(_data->global_stats.count_total)
              << " (counting unperformed ones)\n"
              << "### GROUPS SUMMARY:\n";

    for (const auto& group : _data->groups_stats) {
        print_group_summary(group.first, group.second.status == StatusCode::ok,
                            group.second.count_success, group.second.count_checks);
    }
}

int featurless::test::status() const noexcept {
    return _data->global_stats.status <= StatusCode::ok
             ? 0
             : static_cast<int>(_data->global_stats.status);
}
