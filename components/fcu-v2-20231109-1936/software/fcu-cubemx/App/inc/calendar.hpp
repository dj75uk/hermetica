/*
 * calendar.hpp
 *
 *  Created on: Oct 15, 2023
 *      Author: Daniel Johnson
 */
#ifndef APP_INC_CALENDAR_HPP_
#define APP_INC_CALENDAR_HPP_

#include "accuracy.hpp"

class ProvidesDate : public Accuracy
{
  private:
    uint32_t _date = 0;
  protected:
    void date(const uint32_t value) { _date = value; }
  public:
    uint32_t date() const { return _date; }
};

class ProvidesTime : public Accuracy
{
  private:
    uint32_t _time = 0;
  protected:
    void time(const uint32_t value) { _time = value; }
  public:
    uint32_t time() const { return _time; }
};

#endif /* APP_INC_CALENDAR_HPP_ */
