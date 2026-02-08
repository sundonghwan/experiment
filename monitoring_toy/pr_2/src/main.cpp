#include "memmon/macos_collector.h"
#include <iostream> 
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unistd.h>

static double mib(uint64_t b) { return b / (1024.0 * 1024.0); }

int main(int argc, char** argv) {
    int pid = getpid();
    int interval_ms = 500;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--pid" && i + 1 < argc) pid = std::atoi(argv[++i]);
        else if (a == "--interval" && i + 1 < argc) interval_ms = std::atoi(argv[++i]);
        else if (a == "--no-proc") pid = -1;
        else {
            std::cerr << "usage: memmon [--pid N] [--interval ms] [--no-proc]\n";
            return 2;
        }
    }

    auto c = memmon::make_macos_collector(pid);
    memmon::SysSnapShot sys{};
    memmon::ProcSnapshot proc{};

    uint64_t last_ts = 0, last_in = 0, last_out = 0;
    while (true) {
        bool ok = false;
        if (pid >= 0) ok = c->collect(sys, &proc);
        else ok = c->collect(sys, nullptr);

        if (!ok) {
            std::cerr << "collect failed: " << c->last_error() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            continue;
        }

        double dt = 0.0;
        if (last_ts) dt = (sys.ts_ms - last_ts) / 1000.0;

        double pin_s = 0.0, pout_s = 0.0;
        if (dt > 0.0) {
            pin_s  = (sys.pageins_total  - last_in)  / dt;
            pout_s = (sys.pageouts_total - last_out) / dt;
        }

        std::cout << std::fixed;
        if (pid >= 0) {
            std::cout
                << "pid=" << proc.pid
                << " rss=" << mib(proc.rss_bytes) << "MiB"
                << " vms=" << mib(proc.vms_bytes) << "MiB";
        }

    std::cout
      << " | free=" << mib(sys.free_bytes) << "MiB"
      << " active=" << mib(sys.active_bytes) << "MiB"
      << " inactive=" << mib(sys.inactive_bytes) << "MiB"
      << " wired=" << mib(sys.wired_bytes) << "MiB"
      << " compressed=" << mib(sys.compressed_bytes) << "MiB"
      << " swap_used=" << mib(sys.swap_used_bytes) << "MiB"
      << " | pageins/s=" << pin_s
      << " pageouts/s=" << pout_s
      << "\n";

    last_ts = sys.ts_ms;
    last_in = sys.pageins_total;
    last_out = sys.pageouts_total;

    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
}