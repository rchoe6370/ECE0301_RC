#include "PracticalVsCircuit.hpp"
#include <vector>
#include <stdexcept>

// Real source = ideal source with "internal" resistance in series
// Basically just two resistor voltage divider
PracticalVsCircuit::PracticalVsCircuit(double vs, double rs, double rl) : VoltageDivider(vs, std::vector<double>{rs, rl}.data(), 2) {

}

// Voltage across real voltage source (ideal + resistor)
double PracticalVsCircuit::get_source_voltage() const {
    return VoltageDivider::get_source_voltage() - get_resistor_voltage(0);
}

void PracticalVsCircuit::set_load(double rl) {
    set_resistance(1, rl);
}

// Using P = v^2/R to get power of load resistor
double PracticalVsCircuit::get_load_power() const {
    double vl = get_resistor_voltage(1);
    double rl = get_resistance(1);
    return (vl * vl) / rl;
}

// Sets load resistor to rl
// If load power equals theoretical max, return true
// Rl = Rs
bool PracticalVsCircuit::maximum_power_transfer(double rl) {
    set_load(rl);

    double vs = VoltageDivider::get_source_voltage();
    double rs = get_resistance(0);

    // max power in thevenin circuit
    double p_max = (vs * vs) / (4.0 * rs);

    return std::abs(p_max - get_load_power()) < 1e-9;
}