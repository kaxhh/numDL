/**
 *******************************************************************************
 * Copyright (c) 2022 China Mobile Communications Group Co.,Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file        constants.h
 *
 * @brief       Holds usefull constants
 *
 * @revision
 * Date         Author          Notes
 * 2022-01-12   OneOS AI Team   First Version
 *******************************************************************************
 */
#ifndef __NUMDL_CONSTANTS_H__
#define __NUMDL_CONSTANTS_H__

namespace nd
{
    //================================Constants====================================
    /// Holds usefull constants
    namespace constants
    {
        constexpr double    c = 3.0e8; ///< speed of light
        constexpr double    e = 2.718281828459045;  ///< eulers number
        constexpr double    pi = 3.14159265358979323846; ///< Pi

        constexpr int    DAYS_PER_WEEK = 7; ///< Number of days in a week
        constexpr int    MINUTES_PER_HOUR = 60; ///< Number of minutes in an hour
        constexpr int    SECONDS_PER_MINUTE = 60; ///< Number of seconds in a minute
        constexpr int    MILLISECONDS_PER_SECOND = 1000; ///< Number of milliseconds in a second
        constexpr int    SECONDS_PER_HOUR = MINUTES_PER_HOUR * SECONDS_PER_MINUTE; ///< Number of seconds in an hour
        constexpr int    HOURS_PER_DAY = 24; ///< Number of hours in a day
        constexpr int    MINUTES_PER_DAY = HOURS_PER_DAY * MINUTES_PER_HOUR; ///< Number of minutes in a day
        constexpr int    SECONDS_PER_DAY = MINUTES_PER_DAY * SECONDS_PER_MINUTE; ///< Number of seconds in a day
        constexpr int    MILLISECONDS_PER_DAY = SECONDS_PER_DAY * MILLISECONDS_PER_SECOND; ///< Number of milliseconds in a day
        constexpr int    SECONDS_PER_WEEK = SECONDS_PER_DAY * DAYS_PER_WEEK; ///< Number of seconds in a week
    }  // namespace constants
}  // namespace nd

#endif