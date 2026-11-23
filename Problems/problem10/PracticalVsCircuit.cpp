#include "PracticalVsCircuit.hpp"
#include <vector>
#include <stdexcept>

PracticalVsCircuit::PracticalVsCircuit(double vs, double rs, double rl) : VoltageDivider(vs, std::vector<double>{rs, rl}.data(), 2) {

}

double PracticalVsCircuit::get_source_voltage() const {
    return VoltageDivider::get_source_voltage() - get_resistor_voltage(0);
}

void PracticalVsCircuit::set_load(double rl) {
    set_resistance(1, rl);
}

double PracticalVsCircuit::get_load_power() const {
    double vl = get_resistor_voltage(1);
    double rl = get_resistance(1);
    return (vl * vl) / rl;
}

bool PracticalVsCircuit::maximum_power_transfer(double rl) {
    set_load(rl);

    double vs = VoltageDivider::get_source_voltage();
    double rs = get_resistance(0);
    double p_max = (vs * vs) / (4.0 * rs);

    return std::abs(p_max - get_load_power()) < 1e-9;
}