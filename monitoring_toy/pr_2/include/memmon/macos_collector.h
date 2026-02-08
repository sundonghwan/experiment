#pragma once
#include "memmon/collector.h"
#include <memory>


namespace memmon {
    std::unique_ptr<ICollector> make_macos_collector(int pid);
}
