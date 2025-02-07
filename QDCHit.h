#ifndef QDC_HIT_H
#define QDC_HIT_H

#include <iostream>
#include <BitFunctions.h>

/* QDCHit: data provided by CAEN V792 charge to digital converter (QDC)
 *
 * Fields:
 * bits    width field
 *  0 - 11 12    Value
 * 12 - 12 1     Overflow
 * 13 - 13 1     UnderThreshold
 * 14 - 18 5     GEO
 * 19 - 23 5     Channel
 * 24 - 31 8     Crate
 * 32 - 55 24    Event
 *
 * - Value: measured charge, in 100 fC
 * - UnderThreshold: when set, the datum was below the threshold set by the QDC
 *   configuration.
 * - Overflow: when set, the measured charge was too large to fit into Value.
 * - GEO: QDC number set by configuration (GEO address).
 * - Channel: QDC channel number for this measurement.
 * - Event: trigger number for this measurement.
 *
 * Bits map:
 *   01234567
 * 0 vvvvvvvv         v = Value
 * 1 vvvvougg         o = Overflow
 * 2 gggccccc         u = UnderThreshold
 * 3 CCCCCCCC         g = GEO
 * 4 eeeeeeee         c = Channel
 * 5 eeeeeeee         C = Crate
 * 6 eeeeeeee         e = Event
 */

class QDCHit {
  
public:
  QDCHit() {
    std::fill(std::begin(data), std::end(data), 0);
  };
  
  QDCHit(
	 uint32_t header,
	 uint32_t packet,
	 uint32_t trailer
	 ) {
    SetCrate(bits<16, 23>(header));
    SetValue(bits<0, 11>(packet));
    SetOverflow(bits<13, 13>(packet));
    SetUnderThreshold(bits<12, 12>(packet));
    SetChannel(bits<16, 20>(packet));
    SetGEO(bits<27, 31>(packet));
    SetEvent(bits<0, 23>(trailer));
  };
  
  uint16_t GetValue() const {
    return bits<0, 11>(data);
  };
  
  void SetValue(uint16_t value) {
    set_bits<0, 11>(data, value);
  };
  
  bool GetOverflow() const {
    return bits<12, 12>(data);
  };
  
  void SetOverflow(bool overflow) {
    set_bits<12, 12>(data, overflow);
  };
  
  bool GetUnderThreshold() const {
    return bits<13, 13>(data);
  };
  
  void SetUnderThreshold(bool ut) {
    set_bits<13, 13>(data, ut);
  };
  
  uint8_t GetGEO() const {
    return bits<14, 18>(data);
  };
  
  void SetGEO(uint8_t geo) {
    set_bits<14, 18>(data, geo);
  };
  
  uint8_t GetChannel() const {
    return bits<19, 23>(data);
  };
  
  void SetChannel(uint8_t channel) {
    set_bits<19, 23>(data, channel);
  };
  
  uint8_t GetCrate() const {
    return bits<24, 31>(data);
  };
  
  void SetCrate(uint8_t crate) {
    set_bits<24, 31>(data, crate);
  };
  
  uint32_t GetEvent() const {
    return bits<32, 55>(data);
  };
  
  void SetEvent(uint32_t event) {
    set_bits<32, 55>(data, event);
  };
  
  void Print(std::ostream& output = std::cout) const {
      output << "value = " << GetValue() << std::endl;
      output << "overflow = " << GetOverflow() << std::endl;
      output << "under_threshold = " << GetUnderThreshold() << std::endl;
      output << "geo = " << static_cast<int>(GetGEO()) << std::endl;
      output << "channel = " << static_cast<int>(GetChannel()) << std::endl;
      output << "crate = " << static_cast<int>(GetCrate()) << std::endl;
      output << "event = " << GetEvent() << std::endl;
  };
  
private:
  uint8_t data[7];

};


#endif
