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
//! \file    AvgNew.cpp
//! \brief   Modified version of Average.h (no template, small footprint).
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.1.0
//--------------------------------------------------------------------------------
#include "AvgNew.h"

Average::Average() {
}

Average::~Average() {
    free(_store);
}

void Average::init(uint16_t size) {
    _size = size;
    _count = 0;
    _store = (uint16_t *) malloc(sizeof(uint16_t) * size);
    _position = 0;                                            // track position for circular storage
    _sum = 0;                                                 // track sum for fast mean calculation
    for (byte i = 0; i < size; i++) {
        _store[i] = 0;
    }
}

void Average::freeMem() {
  free(_store);
}

int Average::getCount() {
    return _count;
}

void Average::push(uint16_t entry) {
    if (_count < _size) {                                     // adding new values to array
        _count++;                                             // count number of values in array
    } else {                                                  // overwriting old values
        _sum = _sum -_store[_position];                       // remove old value from _sum
    }
    _store[_position] = entry;                                // store new value in array
    _sum += entry;                                            // add the new value to _sum
    _position += 1;                                           // increment the position counter
    if (_position >= _size) _position = 0;                    // loop the position counter
}


float Average::rolling(uint16_t entry) {
    this->push(entry);
    return this->mean();
}

float Average::mean() {
    if (_count == 0) {
        return 0;
    }
    return ((float)_sum / (float)_count);                     // mean calculation based on _sum
}

uint16_t Average::mode() {
  byte pos;
  byte inner;
  uint16_t most;
  byte mostcount;
  uint16_t current;
  byte currentcount;

  if (_count == 0) {
      return 0;
  }

  most = this->get(0);
  mostcount = 1;
  for(pos = 0; pos < _count; pos++) {
    current = this->get(pos);
    currentcount = 0;
    for(inner = pos + 1; inner < _count; inner++) {
      if(this->get(inner) == current) {
        currentcount++;
      }
    }
    if(currentcount > mostcount) {
      most = current;
      mostcount = currentcount;
    }
    // If we have less array slices left than the current
    // maximum count, then there is no room left to find
    // a bigger count.  We have finished early and we can
    // go home.
    if(_count - pos < mostcount) {
      break;
    }
  }
  return most;
}

uint16_t Average::minimum() {
    return this->minimum(NULL);
}

uint16_t Average::minimum(int *index) {
  uint16_t minval;

    if (index != NULL) {
        *index = 0;
    }

    if (_count == 0) {
        return 0;
    }

  minval = this->get(0);
  
  for(byte i = 0; i < _count; i++) {
      if(this->get(i) < minval) {
      minval = this->get(i);

              if (index != NULL) { 
                *index = i;
              }
      }

  }
  return minval;
}

uint16_t Average::maximum() {
    return this->maximum(NULL);
}

uint16_t Average::maximum(int *index) {
  uint16_t maxval;

    if (index != NULL) {
        *index = 0;
    }

    if (_count == 0) {
        return 0;
    }

  maxval = this->get(0);

  for(byte i = 0; i < _count; i++) {
    if(this->get(i) > maxval) {
      maxval = this->get(i);
            if (index != NULL) { 
                *index = i;
            }
    }
  }
  return maxval;
}

float Average::stddev() {
  float square;
  float sum;
  float mu;
  float theta;
  //int i;

    if (_count == 0) {
        return 0;
    }

  mu = this->mean();

  sum = 0;
  for(byte i = 0; i < _count; i++) {
    theta = mu - (float)this->get(i);
    square = theta * theta;
    sum += square;
  }
  return sqrt(sum/(float)_count);
}

uint16_t Average::get(uint16_t index) {
    if (index >= _count) {
        return 0;
    }
    byte cindex = _position-index;                         // position in circular buffer
    if (cindex < 0) cindex = _size + cindex;                  // need to loop around for negative cindex
    return _store[index];
}

void Average::leastSquares(float &m, float &c, float &r) {
    float   sumx = 0.0;                        /* sum of x                      */
    float   sumx2 = 0.0;                       /* sum of x**2                   */
    float   sumxy = 0.0;                       /* sum of x * y                  */
    float   sumy = 0.0;                        /* sum of y                      */
    float   sumy2 = 0.0;                       /* sum of y**2                   */

    for (byte i=0;i<_count;i++)   { 
        sumx  += i;
        sumx2 += sqr(i);  
        sumxy += i * this->get(i);
        sumy  += this->get(i);      
        sumy2 += sqr(this->get(i)); 
    } 

    float denom = (_count * sumx2 - sqr(sumx));
    if (denom == 0) {
        // singular matrix. can't solve the problem.
        m = 0;
        c = 0;
        r = 0;
        return;
    }

    m = 0 - (_count * sumxy  -  sumx * sumy) / denom;
    c = (sumy * sumx2  -  sumx * sumxy) / denom;
    r = (sumxy - sumx * sumy / _count) / sqrt((sumx2 - sqr(sumx)/_count) * (sumy2 - sqr(sumy)/_count));
}

uint16_t Average::predict(int x) {
    float m, c, r;
    this->leastSquares(m, c, r); // y = mx + c;

    uint16_t y = m * x + c;
    return y;
}

// Return the sum of all the array items
uint16_t Average::sum() {
    return _sum;
}

void Average::clear() {
    _count = 0;
    _sum = 0;
    _position = 0;
}

/*template <class T> Average<T> &Average<T>::operator=(Average<T> &a) {
    clear();
    for (int i = 0; i < _size; i++) {
        push(a.get(i));
    }
    return *this;
}*/

