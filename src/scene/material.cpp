#include "material.hpp"

Tint::Material::Material()
{
}

Tint::Material::~Material()
{
}

Tint::Material::Property Tint::Material::operator[](const std::string &s)
{
    return Property{};
}
