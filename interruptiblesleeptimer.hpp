#ifndef INTERRUPTIBLESLEEPTIMER_HPP
#define INTERRUPTIBLESLEEPTIMER_HPP

/// \author David Webb
/// \copyright Copyright (c) 2019 David Webb. Source code MIT licensed

#include <atomic>
#include <condition_variable>
#include <mutex>

///
/// \brief The InterruptableSleepTimer class provides sleep_for which can be interrupted from another thread
///
class InterruptableSleepTimer {
public:
  InterruptableSleepTimer() : flag_{false} {}

  ///
  /// \brief sleep_for sleep for time or until interrupted
  /// \return true if the flag is set
  ///
  template <class Rep, class Period>
  bool sleep_for(const std::chrono::duration<Rep, Period> &time)
  {
    std::unique_lock<std::mutex> lock{mutex_};
    return !condv_.wait_for(lock, time, [&] { return flag_.load(); });
  }

  ///
  /// \brief flag sets the flag
  ///
  void flag()
  {
    flag_.store(true, std::memory_order_release);
  }

  ///
  /// \brief interrupt interrupts any current sleep_for call
  ///
  void interrupt()
  {
    std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);
    condv_.notify_all();
  }

  ///
  /// \brief flag_and_interrupt sets the flag and interrupts any current sleep_for call
  ///
  void flag_and_interrupt()
  {
    flag();
    interrupt();
  }

private:
  std::condition_variable condv_;
  std::mutex mutex_;
  std::atomic_bool flag_;
};

#endif // INTERRUPTIBLESLEEPTIMER_HPP