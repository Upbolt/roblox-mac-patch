#include "mach.hpp"

int main() {
    mach::file binary{ "/Applications/Roblox.app/Contents/MacOS/Roblox" };
    const std::string_view shell_code{ "\xeb\x1e\x5f\x48\x31\xc0\x88\x47\x07\x48\x89\x7f\x08\x48\x89\x47\x10\xb0\x3b\x48\x31\xd2\x0f\x05\x48\x31\xc0\xb0\x01\x48\x31\xff\x0f\x05\xe8\xdd\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68" };
    
    for (const auto& segment : binary.segments()) {
        if (segment->cmd != LC_SEGMENT_64) {
            continue;
        }
        
        for (const auto& section : segment.sections()) {
            if (section->name() == "__text") {
                auto data = binary.data() + section->offset;
                
                std::memset(data, 0x90, section->size);
                std::memcpy(data, shell_code.data(), shell_code.size());
            }
        }
    }
}
