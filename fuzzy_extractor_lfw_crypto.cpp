#include "fuzzy_extractor_lfw_crypto.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <stdexcept>

FuzzyExtractorLfwKey FuzzyExtractorLfwDeriveKey(
    const FuzzyExtractorLfwSeed& seed,
    const std::uint8_t* witness,
    std::size_t witnessSize)
{
    FuzzyExtractorLfwKey key{};
    unsigned int outputSize = 0;
    if (HMAC(
            EVP_sha256(),
            seed.data(),
            static_cast<int>(seed.size()),
            witness,
            witnessSize,
            key.data(),
            &outputSize) == nullptr ||
        outputSize != key.size()) {
        throw std::runtime_error("HMAC-SHA-256 key derivation failed");
    }
    return key;
}

bool FuzzyExtractorLfwKeyEqual(
    const FuzzyExtractorLfwKey& left,
    const FuzzyExtractorLfwKey& right)
{
    return CRYPTO_memcmp(left.data(), right.data(), left.size()) == 0;
}

