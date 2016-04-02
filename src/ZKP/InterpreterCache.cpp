#include "InterpreterCache.h"
#include <boost/thread/tss.hpp>

// thead-local storage for the interpreter cache
boost::thread_specific_ptr<InterpreterCache> localInterpreterCache;

InterpreterCache& InterpreterCache::instance() {
    if (localInterpreterCache.get() == 0) // no ICache for this thread
        localInterpreterCache.reset(new InterpreterCache()); // create one
    return *localInterpreterCache;
}
