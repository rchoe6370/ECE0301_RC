#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <stdexcept>
#include <cmath>

#include "Node.hpp"
#include "Component.hpp"
#include "VoltageSource.hpp"
#include "CurrentSource.hpp"
#include "Resistor.hpp"

#include "Network.hpp"
#include "VoltageDivider.hpp"
#include "CurrentDivider.hpp"
#include "PracticalVsCircuit.hpp"

const double THRESHOLD = 1e-9;

bool compare_double(double val1, double val2)
{
  double diff = std::abs(val1 - val2);
  return (diff <= THRESHOLD);
}

// VoltageSource class unit tests
TEST_CASE("Network: Test VoltageSource", "[network]")
{
  VoltageSource vs;

  // test inherited behavior
  REQUIRE(vs.get_node_a() == nullptr);
  REQUIRE(vs.get_node_b() == nullptr);

  Node *node1ptr = new Node();
  Node *node2ptr = new Node();
  REQUIRE_NOTHROW(vs.set_node_a(node1ptr));
  REQUIRE_NOTHROW(vs.set_node_b(node2ptr));
  REQUIRE(vs.get_node_a() == node1ptr);
  REQUIRE(vs.get_node_b() == node2ptr);
  REQUIRE(compare_double(vs.get_voltage(), 0));

  // test source voltage, should start as 0 from Component default
  double voltage = 3.3;
  REQUIRE(compare_double(vs.get_voltage(), 0));
  vs.set_value(voltage);
  // source voltage should be independent of node voltages
  REQUIRE(compare_double(vs.get_voltage(), voltage));

  // construct with pointers, test that source voltage is separate from terminal voltages
  double v1 = 5.2, v2 = 1.3;
  node1ptr->set_voltage(v1);
  node2ptr->set_voltage(v2);
  VoltageSource vs2(node1ptr, node2ptr);
  REQUIRE(compare_double(vs2.get_voltage(), 0));
  vs2.set_value(-voltage);
  REQUIRE(compare_double(vs2.get_voltage(), -voltage));

  delete node1ptr;
  delete node2ptr;
}

// CurrentSource class unit tests
TEST_CASE("Network: Test CurrentSource", "[network]")
{
  // test inherited behavior
  CurrentSource cs;

  REQUIRE(cs.get_node_a() == nullptr);
  REQUIRE(cs.get_node_b() == nullptr);
  REQUIRE_THROWS_AS(cs.get_voltage(), std::logic_error);

  Node *node1ptr = new Node();
  Node *node2ptr = new Node();
  REQUIRE_NOTHROW(cs.set_node_a(node1ptr));
  REQUIRE_NOTHROW(cs.set_node_b(node2ptr));
  REQUIRE(cs.get_node_a() == node1ptr);
  REQUIRE(cs.get_node_b() == node2ptr);
  REQUIRE(compare_double(cs.get_voltage(), 0));

  // test source current, should start as 0 from Component default
  double current = 3.3;
  REQUIRE(compare_double(cs.get_current(), 0));
  cs.set_value(current);
  REQUIRE(compare_double(cs.get_current(), current));

  // construct with pointers, test current and voltage
  double v1 = 5.2, v2 = 1.3;
  node1ptr->set_voltage(v1);
  node2ptr->set_voltage(v2);
  CurrentSource cs2(node1ptr, node2ptr);
  REQUIRE(compare_double(cs2.get_current(), 0));
  cs2.set_value(-current);
  REQUIRE(compare_double(cs2.get_voltage(), v2 - v1));
  REQUIRE(compare_double(cs2.get_current(), -current));

  delete node1ptr;
  delete node2ptr;
}

// Resistor class unit tests
TEST_CASE("Network: Test Resistor", "[network]")
{
  // test inheritance
  Resistor r;

  REQUIRE(r.get_node_a() == nullptr);
  REQUIRE(r.get_node_b() == nullptr);
  REQUIRE_THROWS_AS(r.get_voltage(), std::logic_error);

  Node *node1ptr = new Node();
  Node *node2ptr = new Node();
  REQUIRE_NOTHROW(r.set_node_a(node1ptr));
  REQUIRE_NOTHROW(r.set_node_b(node2ptr));
  REQUIRE(r.get_node_a() == node1ptr);
  REQUIRE(r.get_node_b() == node2ptr);
  REQUIRE(compare_double(r.get_voltage(), 0));

  // test resistance, should start as 0 from Component default
  double resistance = 590;
  REQUIRE(compare_double(r.get_value(), 0));
  r.set_value(0); // 0 resistance is allowed
  r.set_value(resistance);
  REQUIRE(compare_double(r.get_value(), resistance));

  // test negative resistance
  REQUIRE_THROWS_AS(r.set_value(-resistance), std::invalid_argument);

  // test current
  double v1 = 5.2, v2 = 1.3;
  node1ptr->set_voltage(v1);
  node2ptr->set_voltage(v2);
  REQUIRE(compare_double(r.get_current(), (v2 - v1) / resistance));

  // test get_current throws
  r.set_node_a(nullptr);
  // node a nullptr
  REQUIRE_THROWS_AS(r.get_current(), std::logic_error);
  r.set_node_b(nullptr);
  // both nodes nullptr
  REQUIRE_THROWS_AS(r.get_current(), std::logic_error);
  r.set_node_a(node1ptr);
  // node b still nullptr
  REQUIRE_THROWS_AS(r.get_current(), std::logic_error);
  r.set_node_b(node2ptr);
  // valid
  REQUIRE(compare_double(r.get_current(), (v2 - v1) / resistance));
  // div by 0
  r.set_value(0);
  REQUIRE_THROWS_AS(r.get_current(), std::logic_error);

  delete node1ptr;
  delete node2ptr;
}

// General unit tests of overriding and dynamic binding
TEST_CASE("Network: Test overriding and dynamic binding", "[network]")
{
  Component c;
  Resistor r;
  Component *c_ptr = new Resistor();

  REQUIRE_NOTHROW(c.set_value(-1));
  REQUIRE_THROWS_AS(r.set_value(-1), std::invalid_argument);
  REQUIRE_THROWS_AS(c_ptr->set_value(-1), std::invalid_argument);

  c.set_value(0);
  r.set_value(0);
  c_ptr->set_value(0);
  REQUIRE_NOTHROW(c.get_current());
  REQUIRE_THROWS_AS(r.get_current(), std::logic_error);
  REQUIRE_THROWS_AS(c_ptr->get_current(), std::logic_error);

  double val = 0.1;
  Component c_base(val);
  VoltageSource vs(val);
  CurrentSource cs(val);
  Component *c_ptr1 = new VoltageSource(val);
  Component *c_ptr2 = new CurrentSource(val);

  REQUIRE(compare_double(c_base.get_current(), 0.0));
  REQUIRE(compare_double(vs.get_current(), 0.0));
  REQUIRE(compare_double(cs.get_current(), val));
  REQUIRE(compare_double(c_ptr1->get_current(), 0.0));
  REQUIRE(compare_double(c_ptr2->get_current(), val));

  REQUIRE_THROWS_AS(c_base.get_voltage(), std::logic_error);
  REQUIRE(compare_double(vs.get_voltage(), val));
  REQUIRE_THROWS_AS(cs.get_voltage(), std::logic_error);
  REQUIRE(compare_double(c_ptr1->get_voltage(), val));
  REQUIRE_THROWS_AS(c_ptr2->get_voltage(), std::logic_error);

  delete c_ptr;
  delete c_ptr1;
  delete c_ptr2;
}

// Write your own test cases here.

//Uncomment these partial test cases when you are ready to test the dividers.

TEST_CASE("Network: Test VoltageDivider", "[network]")
{
  double voltage = 5.0;
  const int num = 2;
  double res[num] = {200, 400};
  VoltageDivider div(voltage, res, num);

  // Check initial values
  REQUIRE(div.get_num_nodes() == 3);
  REQUIRE(div.get_num_components() == 3);
  REQUIRE(div.get_num_resistors() == 2);
  REQUIRE(compare_double(div.get_source_voltage(), 5.0));
  REQUIRE(compare_double(div.get_resistance(0), 200));
  REQUIRE(compare_double(div.get_resistance(1), 400));

  double curr = voltage / 600;
  REQUIRE(compare_double(div.get_loop_current(), curr));
  REQUIRE(compare_double(div.get_node_voltage(0), 0.0));
  REQUIRE(compare_double(div.get_node_voltage(1), 5.0));
  REQUIRE(compare_double(div.get_node_voltage(2), 5.0 - 200 * curr));

  // Testing setting source voltage
  div.set_source_voltage(6.0);
  curr = 6.0 / 600.0;
  REQUIRE(compare_double(div.get_source_voltage(), 6.0));
  REQUIRE(compare_double(div.get_loop_current(), curr));
  REQUIRE(compare_double(div.get_node_voltage(1), 6.0));
  REQUIRE(compare_double(div.get_node_voltage(2), 4.0));
  REQUIRE(compare_double(div.get_resistor_voltage(0), 2.0));
  REQUIRE(compare_double(div.get_resistor_voltage(1), 4.0));

  // Testing setting new resistance
  div.set_resistance(1, 800.0);
  curr = 6.0 / 10.0;
  REQUIRE(compare_double(div.get_resistance(1), 800.0));
  REQUIRE(compare_double(div.get_loop_current(), 0.006));
  REQUIRE(compare_double(div.get_node_voltage(1), 6.0));
  REQUIRE(compare_double(div.get_node_voltage(2), 4.8));
  REQUIRE(compare_double(div.get_resistor_voltage(0), 1.2));
  REQUIRE(compare_double(div.get_resistor_voltage(1), 4.8));

  // Test exceptions
  double bad_network[2] = {-400.0, 100.0};
  REQUIRE_THROWS_AS(VoltageDivider(voltage, res, 0), std::invalid_argument);
  REQUIRE_THROWS_AS(VoltageDivider(voltage, bad_network, 2), std::invalid_argument);

  REQUIRE_THROWS_AS(div.get_resistance(-1), std::out_of_range);
  REQUIRE_THROWS_AS(div.get_resistance(2), std::out_of_range);
  REQUIRE_THROWS_AS(div.set_resistance(-1, 100.0), std::out_of_range);
  REQUIRE_THROWS_AS(div.set_resistance(2, 1000.0), std::out_of_range);
}

// TEST_CASE("Network: Test CurrentDivider", "[network]")
// {
//   double current = 0.006;
//   const int num = 2;
//   double res[num] = {15000, 75000};
//   CurrentDivider div(current, res, num);

//   // TODO
// }