#pragma once
#include <cstdint>

struct ProcMemInfo {
    std:: uint64_t rss_bytes;    // Resident Set Size
};

struct SysMemInfo {
    std::uint64_t total_bytes = 0;
    std::uint64_t free_bytes  = 0; // best-effort
    std::uint64_t used_bytes  = 0; // total - free (rough)
};

struct VmPageInfo {
    std::uint64_t page_size; 
    
    std::uint64_t free;
    std::uint64_t active;
    std::uint64_t inactive;
    std::uint64_t wired;
    std::uint64_t compressed;
    std::uint64_t speculative;
    
};


ProcMemInfo get_self_mem();
SysMemInfo  get_system_mem();
VmPageInfo get_vm_page_info();