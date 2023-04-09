# roblox-mac-patch
some wrapper classes around mac c file apis

## original
```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mach-o/loader.h>

int main() {
  int fd = open("/Applications/Roblox.app/Contents/MacOS/Roblox", O_RDWR);
  struct stat st;
  fstat(fd, &st);
  char *ptr = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  struct mach_header_64 *header = (struct mach_header_64 *)ptr;
  struct segment_command_64 *cmd = (struct segment_command_64 *)(header + 1);
  for (int i = 0; i < header->ncmds; i++) {
    if (cmd->cmd == LC_SEGMENT_64) {
      struct section_64 *sect = (struct section_64 *)(cmd + 1);
      for (int j = 0; j < cmd->nsects; j++) {
        if (!strcmp(sect->sectname, "__text")) {
          char *text = ptr + sect->offset;
          memset(text, 0x90, sect->size);
          memcpy(text, "\xeb\x1e\x5f\x48\x31\xc0\x88\x47\x07\x48\x89\x7f\x08\x48\x89\x47\x10\xb0\x3b\x48\x31\xd2\x0f\x05\x48\x31\xc0\xb0\x01\x48\x31\xff\x0f\x05\xe8\xdd\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68", 49);
          munmap(ptr, st.st_size);
          return 0;
        }
        sect++;
      }
    }
    cmd = (struct segment_command_64 *)((char *)cmd + cmd->cmdsize);
  }
  munmap(ptr, st.st_size);
  return -1;
}
```

## using wrapper classes
```cpp
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
                std::memcpy(data, shell_code.data());
            }
        }
    }
}
```
