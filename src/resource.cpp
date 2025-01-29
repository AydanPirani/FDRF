#include "resource.h"
#include <format>
#include <string>

Resource::Resource(): _cpu(0), _ram(0) { }

Resource::Resource(RESOURCE_T cpu, RESOURCE_T ram): _cpu(cpu), _ram(ram) { }

bool Resource::operator==(const Resource& other) const {
    return _cpu == other._cpu && _ram == other._ram;
}

bool Resource::operator<(const Resource& other) const {
    return _cpu < other._cpu || _ram < other._ram;
}

Resource& Resource::operator-=(const Resource& other) {
    if (!(other < *this)) {
        throw std::runtime_error("Resource subtraction leads to negative values");
    }

    _cpu -= other._cpu;
    _ram -= other._ram;
    return *this;
}

Resource& Resource::operator+=(const Resource& other) {
    _cpu += other._cpu;
    _ram += other._ram;
    return *this;
}

Resource& Resource::operator=(const Resource& other) {
    if (this != &other) {
        _cpu = other._cpu;
        _ram = other._ram;
    }
    return *this;
}

float Resource::getDominantShare(const Resource& other) {
    if (other._cpu == 0 || other._ram == 0) {
        throw std::invalid_argument("Division by zero in getDominantShare");
    }

    float cpu_share = static_cast<float>(_cpu) / other._cpu;
    float ram_share = static_cast<float>(_ram) / other._ram;
    return std::max(cpu_share, ram_share);
}

float Resource::getDiff(const Resource& other) {
    RESOURCE_T cpu_delta = _cpu - other._cpu;
    RESOURCE_T ram_delta = _cpu - other._cpu;

    return cpu_delta*cpu_delta + ram_delta*ram_delta;
}

std::string Resource::toString() {
    return std::format("({},{})", _cpu, _ram);
}

Resource operator-(Resource lhs, const Resource& rhs) {
    lhs -= rhs;
    return lhs;
}
