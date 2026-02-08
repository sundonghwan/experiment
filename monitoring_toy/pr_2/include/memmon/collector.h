#pragma once
#include <cstdint>
#include <string>

namespace memmon {
    struct SysSnapShot{
        uint64_t ts_ms = 0;
        uint64_t page_size = 0;
        uint64_t mem_total_bytes = 0;

        //bytes
        uint64_t free_bytes = 0;
        uint64_t active_bytes = 0;
        uint64_t inactive_bytes = 0;
        uint64_t wired_bytes = 0;
        uint64_t speculative_bytes = 0;
        uint64_t compressed_bytes = 0;

        // cumulative counters
        uint64_t pageins_total = 0;
        uint64_t pageouts_total = 0;

        uint64_t swap_total_bytes = 0 ;
        uint64_t swap_used_bytes = 0 ;
    };

    struct ProcSnapshot {
        uint64_t ts_ms = 0;
        int pid = -1;

        uint64_t vms_bytes = 0;
        uint64_t rss_bytes = 0;
    };
    class ICollector {
        public:
            virtual ~ICollector() = default;
            virtual bool collect(SysSnapShot& sys, ProcSnapshot* proc) = 0;
            virtual std::string last_error() const = 0 ; 
    };
    
}