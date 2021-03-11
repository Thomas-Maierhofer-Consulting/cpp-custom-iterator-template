// Copyright Thomas Maierhofer Consulting, Bad Waldsee, Germany
// Licensed under MIT 

#include <gtest/gtest.h>

auto main(int argc, char **argv) -> int{
    testing::FLAGS_gtest_catch_exceptions = true;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}