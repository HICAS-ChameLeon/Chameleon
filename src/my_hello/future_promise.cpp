
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>

#include "Offer.pb.h"

using namespace process;

using namespace process::http;

using std::cerr;
using std::cout;
using std::endl;

using std::chrono::seconds;

using process::Future;
using process::Promise;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;

using std::string;
using namespace std;
TEST(FutureTest, Future)
{
Promise<bool> promise;
promise.set(true);
ASSERT_TRUE(promise.future().isReady());
EXPECT_TRUE(promise.future().get());
}

TEST(FutureTest, Associate){
    Promise<bool> promise3;
    Promise<bool> promise4;
    promise3.associate(promise4.future());
   promise4.fail("associate");
    ASSERT_TRUE(promise3.future().isFailed());
    EXPECT_EQ("associate",promise3.future().failure());
}

static Future<string> itoa1(int* const& i){
    std::ostringstream out;
    out<<*i;
    return out.str();
}

TEST(FutureTest, Then){
    Promise<int*> promise;
    int i=42;
    promise.set(&i);
    Future<string> future = promise.future().then(lambda::bind(&itoa1,lambda::_1));
    ASSERT_TRUE(future.isReady());
    EXPECT_EQ("42",future.get());
}

Future<int> repair(const Future<int>& future){
    EXPECT_TRUE(future.isFailed());
    EXPECT_EQ("Failure",future.failure());
    return 43;
}

TEST(FutureTest, Repair){
    Promise<int> promise1;
    Future<int> future1 = promise1.future();
    future1.repair(lambda::bind(&repair,lambda::_1));

    EXPECT_TRUE(future1.isPending());
    promise1.set(42);
    EXPECT_EQ(42,future1.get());

//
//    Promise<int> promise2;
//    Future<int> future2 = promise2.future();
//    future2.repair(lambda::bind(&repair,lambda::_1));
//
//    EXPECT_TRUE(future2.isPending());
//    promise2.set(42);
//
//    promise2.fail("Failure");
//
//    EXPECT_EQ(43,future2.get());
}

//TEST(FuturebasedTest, future_based){
//    Future<int> f = process::collect(asn)
//}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
//    Promise<int> promise;
//
//    // Set up a callback to discard the future if
//    // requested (this is not always possible!).
//    promise.future().onDiscard([&]() {
//        promise.discard();
//    });
//
//    Future<int> future = promise.future();
//
////    CHECK_PENDING(future);
//
//    future.discard();
//
////    CHECK_DISCARDED(future); // NO LONGER PENDING!

    return RUN_ALL_TESTS();
}