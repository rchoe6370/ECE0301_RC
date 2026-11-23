#include "Resistor.hpp"
#include  <stdexcept>

// Overrides Component to reject negative resistance
// However, uses actual storing to Component class
void Resistor::set_value(double r)
{
  if (r < 0) {
    throw std::invalid_argument("Resistance can not be negative");
  }
  Component::set_value(r);
}

// Ohm's alw
double Resistor::get_current() const
{
  double r = Component::get_value();
  if (r <= 0) {
    throw std::logic_error("Resistance can not be zero");
  }

  double v = get_voltage();
  return v / r;
}