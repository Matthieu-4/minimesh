#pragma once

#include <memory>

#include "cpptoml.h"
#include "transform.hpp"

class OptionsParser
{
public:
  OptionsParser(const std::string&);
  ~OptionsParser(void) {}

  // TODO: modifier le type de retour en cohérence
  // avec la structure de données renvoyée.
  Transformation* parse(void);

private:
  std::string m_file_name;
  std::shared_ptr<cpptoml::table> m_data;
};
