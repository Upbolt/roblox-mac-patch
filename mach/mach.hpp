#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mach-o/loader.h>

#include <string>
#include <string_view>
#include <filesystem>

namespace mach {
    class file {
        std::string _data;
    public:
        file(const std::filesystem::path&);
        
        mach_header_64* header() const;
        mach::segments segments() const;
    };
    
    class sections;
    struct section;
    
    struct segment : segment_command_64 {
    public:
        class iterator {
            section* _data;
            std::size_t _index;
        public:
            iterator(section* data, std::size_t = 0);
            mach::sections sections();
        };
    };
    
    struct section : section_64 {
        std::string_view name() const;
    };
    
    class sections {
        mach::segment* _data;
    public:
        class iterator {
            section* _data;
            std::size_t _index;
        public:
            iterator(section*, std::size_t = 0);
            
            section* operator->();
            section& operator*();
            
            iterator& operator++();
            bool operator==(const iterator&);
        }
        
        sections(mach::segment*);
        
        iterator begin();
        iterator end();
    };
    
    class segments {
        mach_header_64* _data;
    public:
        class iterator {
            segment* _data;
            std::size_t _index;
        public:
            iterator(segment*, std::size_t = 0);
            
            segment* operator->();
            segment& operator*();
            
            iterator& operator++();
            bool operator==(const iterator&);
        }
        
        segments(mach_header_64*);
        iterator begin();
        iterator end();
    };
}
