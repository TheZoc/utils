#pragma once

//
// Cast Keeping Bits
//
// Felipe Guedes da Silveira
// March 2022
//
// This header-only utility is used to change the type of a variable as a
// static cast, while keeping the internal bit structure intact.
//
// As an example, this can be used with with frameworks that can only generate
// random numbers as signed integers (e.g. Unreal Engine) and use their output
// to seed custom noise or RNG implementations (e.g. SquirrelNoise5).
// This way, it's possible to use the full range of bits instead of a truncated
// value.
//

template <typename ToType, typename FromType>
constexpr ToType CastKeepingBits(const FromType& FromValue)
{
	static_assert(sizeof(FromType) == sizeof(ToType), "Sizes of the template parameters do not match.");
	constexpr union SignedToUnsigned { FromType from; ToType to; };
	return SignedToUnsigned({ FromValue }).to;
}
