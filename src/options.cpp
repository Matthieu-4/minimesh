#include <memory>

#include "options.hpp"

OptionsParser::OptionsParser(const std::string& file_name)
  : m_file_name(file_name)
{
  m_data = cpptoml::parse_file(m_file_name);
}

Transformation* OptionsParser::parse(void)
{
  auto transform = m_data->get_table("transform");
  auto name = *transform->get_as<std::string>("name");

  if (name.compare("merge") == 0) {
    bool merge_nodes = *transform->get_as<bool>("merge_nodes");

    auto io = m_data->get_table("io");
    std::vector<std::string> meshes = *io->get_array_of<std::string>("inputs");
    std::string result_file_name = *io->get_as<std::string>("output");

    auto quality = m_data->get_table("quality");
    bool compute_quality = *quality->get_as<bool>("compute_quality");

    return new Merge(merge_nodes, meshes, result_file_name, compute_quality);
  } else if (name.compare("translate") == 0) {
    std::vector<double> coords = *transform->get_array_of<double>("translation");

    auto io = m_data->get_table("io");
    std::string mesh = *io->get_as<std::string>("input");
    std::string result_file_name = *io->get_as<std::string>("output");

    auto quality = m_data->get_table("quality");
    bool compute_quality = *quality->get_as<bool>("compute_quality");

    return new Translation(coords, mesh, result_file_name, compute_quality);
  } else {
  }
}
