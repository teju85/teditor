/*
 * Copyright (c) 2020, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <catch.hpp>
#include "core/timer.h"
#include <algorithm>
#include <iomanip>

namespace teditor {
namespace catch2 {

static char const* dashed_line =
  "--------------------------------------------------------------------------";
static int const MAX_TEST_NAME_LEN = 64;

struct Listener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;  // inherit ctor

  void testRunStarting(Catch::TestRunInfo const& info) override {
    tic(info.name);
    std::cout << dashed_line << std::endl;
    std::cout << "Test started... [" << info.name << "] (all time in seconds)"
              << std::endl << dashed_line << std::endl << std::flush;
  }

  void testRunEnded(Catch::TestRunStats const& stats) override {
    toc(stats.runInfo.name);
    std::cout << dashed_line << std::endl;
    std::cout << "Time taken: "
              << getTimer(stats.runInfo.name).elapsed() << std::endl
              << std::flush;
  }

  void testCaseStarting(Catch::TestCaseInfo const& info) override {
    tic(info.name);
    std::string name = info.name + " ... ";
    auto n_spaces =
      MAX_TEST_NAME_LEN - std::min(MAX_TEST_NAME_LEN, int(name.size()));
    std::string spaces(n_spaces, ' ');
    std::cout << name << spaces << std::flush;
  }

  void testCaseEnded(Catch::TestCaseStats const& stats) override {
    const auto& name = stats.testInfo.name;
    const char* status = stats.totals.assertions.allPassed() ? "OK" : "FAIL";
    toc(name);
    std::cout << "[" << status << "] [" << std::fixed << std::setw(2)
              << std::setprecision(6) << std::setfill('0')
              << getTimer(name).elapsed() << "]" << std::endl << std::flush;
  }
};  // struct Listener

CATCH_REGISTER_LISTENER(Listener);

}  // namespace catch2
}  // namespace teditor
