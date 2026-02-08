#include <chrono>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <thread>

#include "mem.h"

static volatile std::sig_atomic_t g_stop = 0;
static void on_sigint(int) { g_stop = 1;}

static double mib(std::uint64_t bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0);
}

int main(int argc, char** argv) {
    int interval_ms = 1000;
    if (argc >= 2) interval_ms = std::max(100, std::atoi(argv[1]));
    std::signal(SIGINT, on_sigint);
    std::cout << "memmon (Ctrl+C to stop), interval=" << interval_ms << "ms\n";

    while (!g_stop) {
        const auto p = get_self_mem();
        const auto s = get_system_mem();
        const auto vm = get_vm_page_info();

        const auto mb = [&](std::uint64_t pages) {
            return pages * vm.page_size / (1024.0 * 1024.0);
        };

        // std::cout << std::fixed << std::setprecision(1)
        //           << "self_rss=" << mib(p.rss_bytes) << " MiB";   
        // if (s.total_bytes) {
        //     std::cout << " | sys_used=" << mib(s.used_bytes) << " MiB"
        //               << " sys_free=" << mib(s.free_bytes) << " MiB"
        //               << " total=" << mib(s.total_bytes) << " MiB";
        // }
        // std::cout << "\n";
        std::cout << std::fixed << std::setprecision(1)
          << "free="        << mb(vm.free)        << "MB "
          << "active="      << mb(vm.active)      << "MB "
          << "inactive="    << mb(vm.inactive)    << "MB "
          << "wired="       << mb(vm.wired)       << "MB "
          << "compressed="  << mb(vm.compressed)  << "MB\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
     };

}
