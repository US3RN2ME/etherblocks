
#ifndef ETHERBLOCKS_UT_MAIN_HPP
#define ETHERBLOCKS_UT_MAIN_HPP

#include <boost/ut.hpp>

using namespace boost::ut;

int main() {
   return cfg<override>.run();
}

#endif // ETHERBLOCKS_UT_MAIN_HPP
