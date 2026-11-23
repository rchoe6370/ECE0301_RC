#include "CurrentDivider.hpp"
#include <stdexcept>


CurrentDivider::CurrentDivider(double c, double res[], int num_res) : Network(2, num_res + 1) {
    if (num_res < 1) {
        throw std::invalid_argument("Number of resistors must be >= 1");
    }

    // Only two nodes, bottom (ground) and top
    CurrentSource* cs = new CurrentSource();
    set_component(0, cs);
    connect_component_terminal_a(0, 0);
    connect_component_terminal_b(0, 1);
    set_component_value(0, c);

    // All resistors connect to ground or top node
    for (int i = 0; i < num_res; i++) {
        Resistor* r = new Resistor();
        int index = i + 1;
        set_component(index, r);

        connect_component_terminal_a(index, 0);
        connect_component_terminal_b(index, 1);

        set_component_value(index, res[i]);
    }

    update_node_voltages();
}

void CurrentDivider::update_node_voltages() {
    int num_res = get_num_resistors();
    double cs = get_source_current();

    set_node_voltage(0, 0.0);

    double r_eq = get_resistance(0);

    // Adding resistors in parallel
    for (int i = 1; i < num_res; i++) {
        double r_i = get_resistance(i);

        r_eq = (r_eq * r_i) / (r_eq + r_i);

    }

    // Voltage = req * current source
    double nv = cs * r_eq;
    set_node_voltage(1, nv);
}

int CurrentDivider::get_num_resistors() const {
    return get_num_components() - 1;
}

double CurrentDivider::get_source_current() const {
    return get_component_value(0);
} 

double CurrentDivider::get_resistance(int idx) const {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }
    return get_component_value(idx + 1);
} 

double CurrentDivider::get_resistor_voltage() const { 
    return get_component_voltage(1);
}

double CurrentDivider::get_resistor_current(int idx) const {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }

    return get_component_current(idx + 1);
}

void CurrentDivider::set_source_current(double c) {
    set_component_value(0, c);
    update_node_voltages();
}

void CurrentDivider::set_resistance(int idx, double r) {
    if (idx < 0 || idx >= get_num_resistors()) {
        throw std::out_of_range("Resistor index out of range");
    }
    set_component_value(idx + 1, r);
    update_node_voltages();
}
