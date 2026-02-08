#include "mem.h"

#ifdef MEMMON_MACOS

#include <mach/mach.h>
#include <sys/sysctl.h>

ProcMemInfo get_self_mem() {
    ProcMemInfo out{};

    task_basic_info_data_t info{};
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;

    kern_return_t kr = task_info(
        mach_task_self(),
        TASK_BASIC_INFO,
        reinterpret_cast<task_info_t>(&info),
        &count
    );

    if (kr == KERN_SUCCESS) {
        out.rss_bytes = static_cast<std::uint64_t>(info.resident_size);
    }

    return out;
}

static std::uint64_t sysctl_u64(const char* name) {
    std::uint64_t v = 0;
    size_t len = sizeof(v);
    if (sysctlbyname(name, &v, &len, nullptr, 0) != 0) return 0;
    return v;
}

SysMemInfo get_system_mem() {
    SysMemInfo out{};
    out.total_bytes = sysctl_u64("hw.memsize");

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat{};
    mach_port_t host = mach_host_self();

    if (host_statistics64(host, HOST_VM_INFO64,
                          reinterpret_cast<host_info64_t>(&vmstat),
                          &count) == KERN_SUCCESS) {
        const std::uint64_t page_size = sysctl_u64("hw.pagesize");
        const std::uint64_t free_pages = static_cast<std::uint64_t>(vmstat.free_count);
        out.free_bytes = free_pages * page_size;
    }

    if (out.total_bytes && out.total_bytes >= out.free_bytes) {
        out.used_bytes = out.total_bytes - out.free_bytes;
    }

    return out;
}

VmPageInfo get_vm_page_info() {
    VmPageInfo out{};

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vm{};
    mach_port_t host = mach_host_self();

    if (host_statistics64(host, HOST_VM_INFO64,
                          reinterpret_cast<host_info64_t>(&vm),
                          &count) != KERN_SUCCESS) {
                            return out;
                          }
    std::uint64_t page_size = 0;
    size_t len = sizeof(page_size);
    sysctlbyname("hw.pagesize", &page_size, &len, nullptr, 0);

    out.page_size  = page_size;
    out.free       = vm.free_count;
    out.active     = vm.active_count;
    out.inactive   = vm.inactive_count;
    out.wired      = vm.wire_count;
    out.compressed = vm.compressor_page_count;
    out.speculative= vm.speculative_count;
    
    return out;
}

#else
#error "This file is macOS-only right now."
#endif
