#ifndef TDC_HIT_H
#define TDC_HIT_H

#include <iostream>
#include <BitFunctions.h>

/* TDCHit: data provided by CAEN V1290 time to digital converter (TDC)
 *
 * Fields:
 * bits     width  field
 *  0 - 20  21     Value
 * 21 - 25   5     GEO
 * 26 - 26   1     Trailing
 * 27 - 31   5     Channel
 * 32 - 53  22     Event
 * 56 - 87  32     ExtendedTime
 *
 * - Value: measured hit time, in 25 pcs. This is the value of the internal
 *   clock of the TDC that happened to be when the hit occured. It loops every
 *   52.4 us.
 * - ExtendedTime: extended trigger time tag, in 25 ns. This is the value of
 *   the internal clock sampled at lower frequency that happened to be at the
 *   start of the trigger window. It loops every 107 s.
 * - Event: trigger number for this measurement.
 * - Channel: TDC channel number for this measurement.
 * - Trailing: whether this is the measurement of the trailing (1) or the
 *   leading (0) edge of the pulse.
 * - GEO: TDC number set by configuration (GEO address).
 *
 * Bits map:
 *    01234567
 *  0 vvvvvvvv          0 = reserved
 *  1 vvvvvvvv          v = Value
 *  2 vvvvvggg          g = GEO
 *  3 ggtccccc          t = Trailing
 *  4 EEEEEEEE          c = Channel
 *  5 EEEEEEEE          E = Event
 *  6 EEEEEE00          T = ExtendedTime
 *  7 TTTTTTTT
 *  8 TTTTTTTT
 *  9 TTTTTTTT
 * 10 TTTTTTTT
 */

class TDCHit{
  
public:
  TDCHit() {
    std::fill(std::begin(data), std::end(data), 0);
  };
  
  TDCHit(
      uint32_t header,
      uint32_t measurement,
      uint32_t extended_time,
      uint32_t trailer
  ) {
    data[6] = 0;
    init(header, measurement);
    SetExtendedTime(bits<0, 26>(extended_time) << 5 | bits<0, 4>(trailer));
  };

  // When extended trigger time tag is disabled, trailer is not needed
  TDCHit(uint32_t header, uint32_t measurement) {
    std::fill(data + 6, std::end(data), 0);
    init(header, measurement);
  };

  uint32_t GetValue() const {
    return bits<0, 20>(data);
  };

  void SetValue(uint32_t value) {
    set_bits<0, 20>(data, value);
  };

  uint8_t GetGEO() const {
    return bits<21, 25>(data);
  };

  void SetGEO(uint8_t geo) {
    set_bits<21, 25>(data, geo);
  };

  bool GetTrailing() const {
    return bits<26, 26>(data);
  };

  void SetTrailing(bool trailing) {
    set_bits<26, 26>(data, trailing);
  };

  uint8_t GetChannel() const {
    return bits<27, 31>(data);
  };

  void SetChannel(uint8_t channel) {
    set_bits<27, 31>(data, channel);
  };

  uint32_t GetEvent() const {
    return bits<32, 53>(data);
  };

  void SetEvent(uint32_t event) {
    set_bits<32, 53>(data, event);
  };

  uint32_t GetExtendedTime() const {
    return bits<56, 87>(data);
  };

  void SetExtendedTime(uint32_t time) {
    set_bits<56, 87>(data, time);
  };

  void Print(std::ostream& output = std::cout) const {
    output << "value = " << GetValue() << std::endl;
    output << "geo = " << static_cast<int>(GetGEO()) << std::endl;
    output << "trailing = " << GetTrailing() << std::endl;
    output << "channel = " << GetChannel() << std::endl;
    output << "event = " << GetEvent() << std::endl;
    output << "extended_time = " << GetExtendedTime() << std::endl;
  };

private:
  uint8_t data[11];

  void init(uint32_t header, uint32_t measurement) {
    SetGEO(bits<0, 4>(header));
    SetEvent(bits<5, 26>(header));
    SetValue(bits<0, 20>(measurement));
    SetChannel(bits<21, 25>(measurement));
    SetTrailing(bits<26, 26>(measurement));
  };
};


#endif
