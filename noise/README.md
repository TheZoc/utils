Noise Utilities
===============

Utilities to generate Noise and possibly be used as a Random Number Generator.

SquirrelNoise5 [^1]
-------------------

This header-only utility includes a set of `constexpr` functions to create
noise in 1-4 dimensions, by using a set of primes to do it.
Depending on the application, it might be worth tweaking the primes used.

This implementation supersedes SquirrelNoise3, originally presented in the GDC
Talk "Noise-Based RNG"[^2].


[^1]: Originally retrieved from: http://eiserloh.net/noise/SquirrelNoise5.hpp

[^2]: GDC Talk for SquirrelNoise3: https://www.youtube.com/watch?v=LWFzPP8ZbdU
