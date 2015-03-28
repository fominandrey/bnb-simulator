#include "apply_settings.hpp"

void apply_settings()
{
    std::basic_ifstream<json_char> fin("settings.json");

    json_string content{ std::istreambuf_iterator<json_char>(fin), std::istreambuf_iterator<json_char>() };

    JSONNode node = libjson::parse(content);

    simulator::timer::apply_settings(node["timer"]);
    simulator::serializer::apply_settings(node["serializer"]);
    simulator::communicator::apply_settings(node["communicator"]);
    simulator::resolver::apply_settings(node["resolver"]);
}
