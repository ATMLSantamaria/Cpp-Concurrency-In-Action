/*
  Analysis of how to protect a rarely updated data structure. In this case a cache of DNS entries for 
  resolving domain names to their corresponding IP addresses.alignas

  A DNS entry will remain unchanged for a very long period of time (like years). 

  New entries may be added from time to time, this data will remain unchanged throughout its life.alignas

  So, updates are rare, but still happen and if the cache is to be concurrently accesed it needs to be protected.

  There is some special purpose data structures, that we will check later. But so far the analysis is, a thread
  needs exlusive access to update the structure, but once the update is done, the data will only be read and 
  multiply threads can safely access it concurrently.alignas
  
  std::mutex is to restrictive in this scenario, since it eliminates possible concurrency in reads.alignas

  We need a kind of reader-writer mutex, that allows 2 kinds of usage.
      1 - Exclusive access by a single "writer" thread
      2 - Shared access by multiple "reader" threads

  C++17 provides: 
      1 - std::shared_mutex 
      2 - std::shared_timed_mutex (same than shared mutex but some additional operations)

  This mutex come with some hidden costs, so it is very important to profile the performance of the target system
  and assure there is benefit.alignas
  
  Basically, we use std::shared_mutex instead of std::mutex, and for the update, the lock_guard<shared_mutex> or
  unique_lock<shared_mutex> will be used.

  When a update is not necessary (so you just want to read) you may use std::shared_lock<std::shared_mutex>

  IF any thread has a shared lock, the thread that tries to acquired the exclusive lock (for writing) will block
  until all other threads have relinquished their locks.
  And if one thread has acquired the exclsive lock, all the rest of the threads will not acquire a shared or exclusive
  lock until the first has relinquished its lock.
*/


// Lets see a simple example involving a DNS  cahe, using std::map to hold the cache data, and protecting with
// std::shared_mutex

#include <map>
#include <string>
#include <mutex>
#include <shared_mutex>

class dns_entry{};
class dns_cache {

  std::map<std::string,dns_entry> entries;
  mutable std::shared_mutex entry_mutex;

  dns_entry find_entry(std::string const & domain) const {
    std::shared_lock<std::shared_mutex> lk(entry_mutex); // shared_lock since we are just reading
    std::map<std::string,dns_entry>::const_iterator it = entries.find(domain);
    return (it == entries.end()) ? dns_entry() : it->second; // return the stored dns_domain or create one
  }

  void update_or_add_entry(std::string const & domain,dns_entry const & dns_details) {
    std::lock_guard<std::shared_mutex> lk(entry_mutex); // exclusive lock to write
    entries[domain] = dns_details;
  }
};