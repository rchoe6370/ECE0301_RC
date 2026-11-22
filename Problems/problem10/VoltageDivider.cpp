#include "VoltageDivider.hpp"
#include <stdexcept>

VoltageDivider::VoltageDivider(double v, double res[], int num_res) : Network(num_res + 1, num_res + 1)
{
    if (num_res < 1) {
        throw std::invalid_argument("Number of resistors must be > 1");
    }

    VoltageSource *vs = new VoltageSource();
    set_component(0, vs);
    connect_component_terminal_a(0, 0);
    connect_component_terminal_b(0, 1);
    set_component_value(0, v);

    for (int i = 0; i < num_res; i++) {
        Resistor* r = new Resistor();
        int index = i + 1;
        set_component(index, r);

        int node_b = i + 1;
        int node_a = (i == num_res - 1) ? 0 : (i + 2);

        connect_component_terminal_b(index, node_b);
        connect_component_terminal_a(index, node_a);

        set_component_value(index, res[i]);
    }

    update_node_voltages();
}

void VoltageDivider::update_node_voltages() {
    int num_res = get_num_resistors();
    double vs = get_source_voltage();

    set_node_voltage(0, 0.0);
    set_node_voltage(1, vs);

    double r_eq = 0.0;
    for (int i = 0; i < num_res; i++) {
        r_eq += get_resistance(i);
    }

    double current = vs / r_eq;

    double v_curr = vs;
    for (int i = 0; i < num_res - 1; i++) {
        v_curr -= current * get_resistance(i);
        set_node_voltage(i + 2, v_curr);
    }
}

int VoltageDivider::get_num_resistors() const {
    return get_num_components() - 1;
}

double VoltageDivider::get_source_voltage() const {
    return get_component_value(0);
}

double VoltageDivider::get_resistance(int idx) const {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }
    return get_component_value(idx + 1);
}

double VoltageDivider::get_resistor_voltage(int idx) const {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }
    return get_component_voltage(idx + 1);
}

double VoltageDivider::get_loop_current() const {
    return get_component_current(1);
}

void VoltageDivider::set_source_voltage(double v) {
    set_component_value(0, v);
    update_node_voltages();
}

void VoltageDivider::set_resistance(int idx, double r) {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }

    set_component_value(idx + 1, r);
    update_node_voltages();
}