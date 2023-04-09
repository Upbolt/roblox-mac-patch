#include "mach.hpp"

using section_iterator = mach::sections::iterator;
using segment_iterator = mach::segments::iterator;

mach::file::file(const std::filesystem::path& path) {
    using iterator = std::istreambuf_iterator<char>;
    
    std::ifstream file{ path };
    
    if (!file.is_open()) {
        throw std::exception("invalid file permissions");
    }
    
    this->_data = std::string{ iterator{ file }, iterator{} };
}

mach_header_64* mach::file::header() const {
    return this->_data;
}

mach::sections::sections(mach::segment* data)
    : _data(data)
{
}

mach::segments::segments(mach_header_64* data)
    : _data(data)
{
}

std::string_view mach::section::name() const {
    return this->sectname;
}

section_iterator mach::sections::begin() {
    return { reinterpret_cast<mach::section*>(this->_data + 1) };
}

section_iterator mach::sections::end() {
    return { nullptr, this->_data->nsects };
}

segment_iterator mach::segments::begin() {
    return { reinterpret_cast<mach::segment*>(this->_data + 1) };
}

segment_iterator mach::segments::end() {
    return { nullptr, this->_data->ncmds };
}

segment_iterator::iterator(mach::segment* data, std::size_t index) 
    : _data(data),
    _index(index)
{
}

section_iterator::iterator(mach::section* data, std::size_t index) 
    : _data(data),
    _index(index)
{
}

mach::section* section_iterator::operator->() {
    return this->_data;
}

mach::section& section_iterator::operator*() {
    return *this->_data;
}

section_iterator& section_iterator::operator++() {
    this->_data++;
    this->_index++;
    
    return *this;
}

bool section_iterator::operator==(const section_iterator& other) {
    return this->_index == other->_index;
}

mach::segment* segment_iterator::operator->() {
    return this->_data;
}

mach::segment& segment_iterator::operator*() {
    return *this->_data;
}

segment_iterator& segment_iterator::operator++() {
    this->_data = reinterpret_cast<mach::segment*>(reinterpret_cast<std::uint8_t*>(this->_data), this->_data->cmdsize);
    this->_index++;
    
    return *this;
}

bool segment_iterator::operator==(const segment_iterator& other) {
    return this->_index == other->_index;
}
