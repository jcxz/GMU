/** Utility functions concerning statistics */

#ifndef UTILS_STATS_H
#define UTILS_STATS_H

#include <limits>


namespace utils {

namespace stats {

template <class T>
class Aggregator
{
  public:
    Aggregator(void)
      : m_min(std::numeric_limits<T>::max())
      , m_max(std::numeric_limits<T>::min())
      , m_avg(0.0f)
    {
    }

    T min(void) const { return m_min; }
    T max(void) const { return m_max; }
    float avg(void) const { return m_avg; }

    void add(T val, unsigned int count)
    {
      if (val > m_max) m_max = val;
      if (val < m_min) m_min = val;
      m_avg = (m_avg * count + val) / (count + 1);
    }

  private:
    T m_min;
    T m_max;
    float m_avg;
};

} // End of stats namespace

} // End of utils namespace

#endif