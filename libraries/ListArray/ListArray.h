/*
    ListArray.h

   Library implementing a generic, queue (array version).

    ---

    Copyright (C) 2017  Steven Lian (steven.lian@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// header defining the interface of the source.
#ifndef _LISTARRAY_H
#define _LISTARRAY_H

// include Arduino basic header.
#include <Arduino.h>

// the definition of the queue class.
class ListArray
{
  public:
    // init the queue (constructor).
    ListArray ();
    ListArray (int num, int unitSize);
    int begin(int num, int unitSize);

    // clear the queue (destructor).
    ~ListArray ();

    //clean list 
    int clean();
    
    // add an item to the queue.
    int push (void *ptr);//==rpush
    int lpush(void *ptr);
    int rpush(void *ptr);

    // remove an item from the queue.
    int pop(void *ptr);//==rpop
    int rpop(void *ptr);
    int lpop(void *ptr);

    // get the data with given index,without delete
    int index(void *ptr, int index);
    // get the front of the queue.
    int front (void *ptr);

    // check if the queue is empty.
    bool isEmpty () const;

    // get the number of items in the queue.
    int count () const;
    int len() const;

    // check if the queue is full.
    bool isFull () const;
    
    
    //
    int items;       // the number of items of the queue.
    int head;        // the head of the queue.
    int tail;        // the tail of the queue.

    //  private:
    int sizeOfMemory;  //the memory allcated for this queue.
    int maxItems;
    int sizeOfUnit;        // the size of the queue.
    void *contents;   // the array of the queue.

};

#endif // _LISTARRAY_H


