#ifndef BIT_FUNCTIONS_H
#define BIT_FUNCTIONS_H

#include <cstdint>

template <unsigned nbits>
using UInt = typename std::conditional<
  nbits <= 8,
  uint8_t,
  typename std::conditional<
    nbits <= 16,
    uint16_t,
    typename std::conditional<
      nbits <= 32,
      uint32_t,
      typename std::enable_if<
        nbits <= 64,
        uint64_t
      >::type
    >::type
  >::type
>::type;

// Treats data as a little-endian number and extracts bits [start:end]
// (inclusive) from it
template <unsigned start, unsigned end>
inline
UInt<end + 1 - start>
bits(const uint8_t* data) {
  const int is = start / 8; // index of the start byte
  const int bs = start % 8; // first bit in the start byte
  const int ie = end   / 8; // index of the end byte
  const int be = end   % 8; // last bit in the ending byte
  UInt<end + 1 - start> x = data[ie] & 0xFF >> 7 - be;
  if (ie == is) return x >> bs;
  for (int i = ie; --i > is;) x = x << 8 | data[i];
  return x << 8 - bs | data[is] >> bs;
};

// Treats data as a little-endian number and sets bits [start:end] (inclusive)
// to value
template <unsigned start, unsigned end>
inline
void set_bits(uint8_t* data, UInt<end + 1 - start> value) {
  const int is = start / 8;
  const int bs = start % 8;
  const int ie = end   / 8;
  const int be = end   % 8;

  if (ie == is) {
    uint8_t mask = (~(0xFF << (be + 1 - bs)) << bs) & 0xFF;
    data[ie] = data[ie] & ~mask | value << bs & mask;
  } else {
    data[is] = data[is] & 0xFF >> 8 - bs | value << bs;
    value >>= 8 - bs;
    for (int i = is + 1; i < ie; ++i) {
      data[i] = value;
      value >>= 8;
    };
    uint8_t mask = 0xFF >> 7 - be;
    data[ie] = value & mask | data[ie] & ~mask;
  };
};

// Extracts bits [start:end] (inclusive) from value
template <unsigned start, unsigned end, typename UInt_>
inline
typename std::enable_if<
  std::is_integral<UInt_>::value, UInt<end + 1 - start>
>::type
bits(UInt_ value) {
  return (value & (1ULL << end + 1) - 1) >> start;
};

// Sets bits [start:end] (inclusive) in value to bits
template <unsigned start, unsigned end, typename UInt_>
inline
typename std::enable_if<
  std::is_integral<UInt_>::value, UInt<end + 1 - start>
>::type
set_bits(UInt_ value, UInt<end + 1 - start> bits) {
  uint64_t mask = ~(~1ULL << end - start) << start;
  return value & ~mask | bits << start & mask;
};

#endif
