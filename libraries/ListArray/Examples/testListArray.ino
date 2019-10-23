// include queue library header.
#include <ListArray.h>
//----------------------------------------------


//----------------------------------------------

//a struct
typedef struct {
  int num;
  char text[30];
} stTest;


// declare a string message.
const char *msg = "Happy";

#define TEST_SIZE 10

// create a queue of characters.
ListArray queue(TEST_SIZE, sizeof(stTest));

void debug_print_queue()
{
  Serial.println("\n---- Queue Info begin----");
  Serial.printf("max items:%d, sizOfUnit:%d, contents:%x", queue.maxItems, queue.sizeOfUnit, queue.contents);
  Serial.printf("\nitems:%d, head:%d, tail:%d\n", queue.items, queue.head, queue.tail);
  Serial.println("---- Queue Info end----");
}

// startup point entry (runs once).
void setup ()
{
  short num;
  short beginValue;
  beginValue = 0;
  // start serial communication.
  Serial.begin (115200);
  debug_print_queue();

  char buff[50];
  // enqueue all the message's characters to the queue.
  Serial.println("===PUSH===");
  num = TEST_SIZE + 1;
  Serial.printf("loop Count:%d\n", num);
  for (int i = 0; i < num; i++)
  {
    short nT1;
    sprintf(buff, "No.%d, %s", i + beginValue, msg);
    Serial.println(buff);
    nT1 = queue.push(buff);
    Serial.printf("\nret:%d,", nT1);
    debug_print_queue();
  }

  beginValue = 10;
  Serial.println("===POP 3/4===");
  num = TEST_SIZE * 3 / 4 + 1;
  Serial.printf("loop Count:%d\n", num);
  for (int i = 0; i < num; i++)
  {
    short nT1;
    //sprintf(buff, "No.%d, %s", i + beginValue, msg);
    //Serial.println(buff);
    nT1 = queue.pop(buff);
    Serial.printf("\nret:%d, %s", nT1, buff);
    debug_print_queue();
  }


  beginValue = 20;

  Serial.println("===PUSH HALF===");
  num = TEST_SIZE / 2 + 1;
  Serial.printf("loop Count:%d\n", num);
  for (int i = 0; i < num; i++)
  {
    short nT1;
    sprintf(buff, "No.%d, %s", i + beginValue, msg);
    Serial.println(buff);
    nT1 = queue.lpush(buff);
    Serial.printf("\nret:%d,", nT1);
    debug_print_queue();
  }

  Serial.println("===POP===");
  while (!queue.isEmpty ()) {
    debug_print_queue();
    queue.pop(buff);
    Serial.println(buff);
  }
  debug_print_queue();
  Serial.println ();
}

// loop the main sketch.
void
loop () {
  // nothing here.
}
