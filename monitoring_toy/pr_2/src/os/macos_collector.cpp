#include "memmon/macos_collector.h"
#include "memmon/time.h"

#include <cstring>
#include <string>

#include <sys/sysctl.h>
#include <mach/mach.h>
#include <libproc.h>


namespace memmon {
    namespace {
        static bool sysctl_u64(const char* name, uint64_t& out){
            size_t size = sizeof(out);
            if (sysctlbyname(name, &out, &size, nullptr, 0) != 0 || size != sizeof(out)) {
                return false;
            }
            return true;
        }
        static bool sysctl_swapusage(uint64_t& total, uint64_t& used) {
            xsw_usage xsw{};
            size_t len = sizeof(xsw);

            if (sysctlbyname("vm.swapusage", &xsw, &len, nullptr, 0) != 0) return false;
            if (len < sizeof(xsw)) return false;
            total = static_cast<uint64_t>(xsw.xsu_total);
            used  = static_cast<uint64_t>(xsw.xsu_used);
            return true;
        };

        class MacCollector final : public ICollector {
            public:
                explicit MacCollector(int pid) : pid_(pid) {} 
                bool collect(SysSnapShot& sys, ProcSnapshot* proc) override {
                    err_.clear();
                    sys.ts_ms = now_ms_monotonic();

                    if (!sysctl_u64("hw.memsize", sys.mem_total_bytes)){
                        err_ = "sysctl hw.memsize failed";
                        return false;
                    }

                    vm_size_t page_size = 0 ;
                    if (host_page_size(mach_host_self(), &page_size) !=KERN_SUCCESS){
                        err_ = "host_page_size failed";
                        return false;
                    }
                    sys.page_size = static_cast<uint64_t>(page_size);
                    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
                    vm_statistics64_data_t vmstat{};
                    kern_return_t kr = host_statistics64(
                        mach_host_self(),
                        HOST_VM_INFO64,
                        reinterpret_cast<host_info64_t>(&vmstat),
                        &count
                    );
                    if (kr != KERN_SUCCESS){
                        err_ = "host_statistics64(HOST_VM_INFO64) failed ";
                        return false;
                    }
                    auto pg2b = [&](uint64_t pages) {
                        return pages * sys.page_size;
                    };

                    sys.free_bytes        = pg2b(vmstat.free_count);
                    sys.active_bytes      = pg2b(vmstat.active_count);
                    sys.inactive_bytes    = pg2b(vmstat.inactive_count);
                    sys.wired_bytes       = pg2b(vmstat.wire_count);
                    sys.speculative_bytes = pg2b(vmstat.speculative_count);

                    // “압축된 페이지 수” 기반 근사. vm_stat의 compressor와 맞춰 보는 용도.
                    sys.compressed_bytes  = pg2b(vmstat.compressor_page_count);

                    // 누적 카운터
                    sys.pageins_total  = static_cast<uint64_t>(vmstat.pageins);
                    sys.pageouts_total = static_cast<uint64_t>(vmstat.pageouts);

                    // swap
                    (void)sysctl_swapusage(sys.swap_total_bytes, sys.swap_used_bytes);

                    if (proc) {
                        proc -> ts_ms = sys.ts_ms;
                        proc -> pid = pid_;
                        proc_taskinfo pti{};

                        int ret = proc_pidinfo(
                            pid_,
                            PROC_PIDTASKINFO,
                            0,
                            &pti,
                            sizeof(pti)
                        );
                        if (ret <=0) {
                            err_ = "proc_pidinfo failed for pid " + std::to_string(pid_);
                            return false;
                        }

                        proc->rss_bytes = static_cast<uint64_t>(pti.pti_resident_size);
                        proc->vms_bytes = static_cast<uint64_t>(pti.pti_virtual_size);
                    }
                    return true;
                }
                std::string last_error() const override {
                    return err_;
                }
            private:
                int pid_;
                std::string err_;
        };

    }

    std::unique_ptr<ICollector> make_macos_collector(int pid) {
        return std::make_unique<MacCollector>(pid);
    }
}

