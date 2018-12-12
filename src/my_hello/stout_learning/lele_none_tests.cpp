// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <string>

#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include <stout/none.hpp>
#include <stout/option.hpp>
#include <stout/result.hpp>

using std::string;


None none1()
{
  return None();
}


Option<string> none2()
{
  return None();
}


Option<string> none3(const Option<string>& o)
{
  return o;
}


Result<string> none4()
{
  return None();
}


Result<string> none5(const Result<string>& r)
{
  return r;
}


TEST(NoneTest, Test)
{
  Option<string> o = none1();
  EXPECT_TRUE(o.isNone());
  o = none2();
  EXPECT_TRUE(o.isNone());
  o = none3(none1());
  EXPECT_TRUE(o.isNone());

  Result<string> r = none1();
  EXPECT_TRUE(r.isNone());
  r = none4();
  EXPECT_TRUE(r.isNone());
  r = none5(none1());
  EXPECT_TRUE(r.isNone());
}


int main(int argc, char** argv) {
  // Initialize Google Mock/Test.
  testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}