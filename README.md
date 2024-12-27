Custom containers (ccnt) is a c++ library designed to offer a set of data structures, utilities and algorithms to aid development of large codebases.

Loosely based on the std, ccnt has been built from the ground up with specific use cases in mind and is still in active development.

One of such cases is ccnt::Bitset, a structure that offers a similar feature set to std::bitset with the added benefit of occupying the minimum amount of space required to just hold the number of bits requested.

And built on top of that, ccnt::DynamicBitset, a resizable bitset with dynamic allocation capabilities, aiming to achieve what std::vector<bool> has not.

## INSTALLATION
Requires a c++ 20 compliant compiler.

Build using CMake.

## LICENCE
Distributed under the Apache License 2.0. See LICENSE for more information.
