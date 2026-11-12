#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

constexpr std::size_t kFuzzyExtractorLfwKeyBytes = 32;
constexpr std::size_t kFuzzyExtractorLfwSeedBytes = 32;
using FuzzyExtractorLfwKey =
    std::array<std::uint8_t, kFuzzyExtractorLfwKeyBytes>;
using FuzzyExtractorLfwSeed =
    std::array<std::uint8_t, kFuzzyExtractorLfwSeedBytes>;

FuzzyExtractorLfwKey FuzzyExtractorLfwDeriveKey(
    const FuzzyExtractorLfwSeed& seed,
    const std::uint8_t* witness,
    std::size_t witnessSize);

bool FuzzyExtractorLfwKeyEqual(
    const FuzzyExtractorLfwKey& left,
    const FuzzyExtractorLfwKey& right);

