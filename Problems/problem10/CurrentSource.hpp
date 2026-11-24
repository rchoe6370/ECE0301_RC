#ifndef CURRENT_SOURCE_HPP
#define CURRENT_SOURCE_HPP

#include "Component.hpp"

class CurrentSource : public Component
{
public:
  using Component::Component;
  double get_current() const override;
};

#endif