#include <ListArray.h>

// init the queue (constructor).
ListArray::ListArray ()
{
  items = 0;     // set the number of items of queue to zero.

  head = 0;       // set the head of the queue to zero.
  tail = 0;       // set the tail of the queue to zero.  
  contents=NULL;
  sizeOfUnit = 0;
  maxItems = 0;
  sizeOfMemory = 0;
}


int ListArray::begin (int num, int unitSize)
{
  sizeOfUnit = unitSize;       // set the size of queue to zero.
  maxItems = num;
  sizeOfMemory = sizeOfUnit * (maxItems);
  items = 0;     // set the number of items of queue to zero.

  head = 0;       // set the head of the queue to zero.
  tail = 0;       // set the tail of the queue to zero.

  // allocate enough memory for the array.
  if (contents!=NULL){
    free(contents);    
  }
  contents = (void *) malloc (sizeOfMemory);

  //error, cannot get memory;
  if (contents == NULL) {
    sizeOfUnit = 0;
    maxItems = 0;
    sizeOfMemory = 0;
  }
  return maxItems;
}


ListArray::ListArray (int num, int unitSize)
{
  begin(num,unitSize);
}

// clear the queue (destructor).
ListArray::~ListArray ()
{
  if (contents!=NULL)
    free (contents); // deallocate the array of the queue.

  contents = NULL; // set queue's array pointer to nowhere.

  sizeOfUnit = 0;        // set the size of queue to zero.
  maxItems = 0;
  items = 0;       // set the number of items of queue to zero.

  head = 0;        // set the head of the queue to zero.
  tail = 0;        // set the tail of the queue to zero.
}


// add an item to the queue,return the position in array of the item
int ListArray::clean ()
{
  items = 0;     // set the number of items of queue to zero.
  head = 0;       // set the head of the queue to zero.
  tail = 0;       // set the tail of the queue to zero.
  return items;  
}


// add an item to the queue,return the position in array of the item
int ListArray::rpush (void *ptr)
{
  int ret = -1; //full or fail
  // check if the queue is full.
  if (!isFull ())
  {
    // store the item to the array.
    ret = tail;
    memcpy((contents + tail * sizeOfUnit), ptr, sizeOfUnit);
    tail++;
    // wrap-around index.
    if (tail == maxItems) {
      tail = 0;
    }
    // increase the items.
    items++;
  }
  return ret;
}


// add an item to the queue,return the position in array of the item
int ListArray::lpush (void *ptr)
{
  int ret = -1; //full or fail
  // check if the queue is full.
  if (!isFull ())
  {
    // store the item to the array.
    head--;
    // wrap-around index.
    if (head < 0 ) {
      head = maxItems - 1;
    }
    ret = head;
    memcpy((contents + head * sizeOfUnit), ptr, sizeOfUnit);
    // increase the items.
    items++;
  }
  return ret;
}


// push an item to the queue.
int ListArray::push (void *ptr)
{
  return rpush(ptr);
}

// remove an item from the queue.
int ListArray::lpop (void *ptr)
{
  int ret = -1; //empty or fail
  // check if the queue is empty.
  if (!isEmpty ())
  {
    ret = head;
    memcpy(ptr, (contents + head * sizeOfUnit), sizeOfUnit);
    head++;
    // wrap-around index.
    if (head == maxItems) {
      head = 0;
    }
    // decrease the items.
    items--;
  }
  return ret;
}


// remove an item from the queue.
int ListArray::rpop (void *ptr)
{
  int ret = -1; //empty or fail
  // check if the queue is empty.
  if (!isEmpty ())
  {
    tail--;
    // wrap-around index.
    if (tail < 0) {
      tail = maxItems - 1;
    }
    ret = tail;
    memcpy(ptr, (contents + tail * sizeOfUnit), sizeOfUnit);
    // decrease the items.
    items--;
  }
  return ret;
}




// pop an item from the queue.

int ListArray::pop (void *ptr)
{
  return rpop(ptr);
}

// get the given position data,without delete;
int ListArray::index (void *ptr, int index)
{
  int ret = -1;//no data in this position or fail
  if (!isEmpty ()) {
    if (index < items) {
      int pos;
      pos = head + index;
      if (pos >= maxItems) {
        pos -= maxItems;
      }
      ret = pos;
      memcpy(ptr, (contents + pos * sizeOfUnit), sizeOfUnit);
    }
    return ret;
  }
}



// get the front of the queue.
int ListArray::front (void *ptr)
{
  return index(ptr, 0);
}


// check if the queue is empty.
bool ListArray::isEmpty () const
{
  return items == 0;
}

// check if the queue is full.
bool ListArray::isFull () const {
  return items == maxItems;
}

// get the number of items in the queue.
int ListArray::count () const {
  return items;
}

int ListArray::len () const {
  return items;
}


