/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

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