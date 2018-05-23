/*
 * Copyright (c) , Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 
 *  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//--------------------------------------------------------------------------------
//! \file    AvgNew.h
//! \brief   Modified version of Average.h (no template, small footprint).
//! \date    2017-July
//! \author  My-Lab-odyssey
//! \version 0.3.0
//--------------------------------------------------------------------------------

#ifndef AVERAGE_NEW_H
#define AVERAGE_NEW_H

#if (ARDUINO >= 100) 
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#include <math.h>

inline static float sqr(float x) {
    return x*x;
}

class Average {
    private:
        // Private functions and variables here.  They can only be accessed
        // by functions within the class.
        uint16_t *_store;
        long _sum;                                         // _sum variable for faster mean calculation

        
        byte _position;                                   // _position variable for circular buffer
        byte _count;
        byte _size;

    public:
        // Public functions and variables.  These can be accessed from
        // outside the class.
        Average();
        ~Average();
        
        void init(uint16_t size);
        void freeMem();

        float rolling(uint16_t entry);
        void push(uint16_t entry);
        float mean();
        uint16_t mode();
        uint16_t minimum();
        uint16_t minimum(int16_t *);
        uint16_t maximum();
        uint16_t maximum(int16_t *);
        float stddev();
        uint16_t get(uint16_t);
        void leastSquares(float &m, float &b, float &r);
        int16_t getCount();
        uint16_t predict(int16_t x);
        uint16_t sum();
        void clear();
        void bubble_sort();
        int16_t percentile(int16_t pos);
        //Average<T> &operator=(Average<T> &a);

};

#endif //of #ifndef AVERAGE_NEW_H

