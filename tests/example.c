#include "stdlib.h"
#include "string.h"

void g();
void f();
void send_sensitive_data_at(char* address, int address_len);

int read_address(char* buffer, int size, int* length)
{
  *length = rand() % (size-1);
  for (int i = 0; i < *length; ++i)
    buffer[i] = (char) rand();
  return 0;
}


int read_address2(char* buffer, int size, int* length)
{
  int prefix_length = strlen("https://www.ibm.com/");
  memcpy(buffer, "https://www.ibm.com/", prefix_length);
  size -= prefix_length;
  *length = rand() % (size-1);
  for (int i = 0; i < *length; ++i)
    buffer[prefix_length+i] = (char) rand();
  return 0;
}

int check_address(char* address, int length)
{
  int prefix_length = strlen("https://www.ibm.com/");
  return (prefix_length < length)
    ? (strncmp(address, "https://www.ibm.com/", length) != 0) : 1;
}

int send_data_at(char* address, int address_len)
{
  // address should start with a string coming from the text segment (see read_address2)
  // address should start with "https://www.ibm.com/"
  send_sensitive_data_at(address, address_len);
  return 0;
}

int main()
{
  char buffer[500], buffer2[500];
  int buffer_size = 500;
  int address_len;

  int ko = read_address(buffer, buffer_size, &address_len);
  /* int ko = read_address2(buffer, buffer_size, &address_len); */
  char* address = buffer;
  if (ko)
    return 1;
  if (!check_address(address, address_len))
    return 2;
  f(); /* f may have buffer overflow that write in the address buffer */
  if (rand() % 2)
  {
    strcpy(buffer2, address);
    address = buffer2;
  }
  g(); /* g may have buffer overflow that write in the address buffer */

  /* a long_jmp may go here */
  ko = send_data_at(address, address_len);
  if (ko)
    return 3;
  return 0;
}

    
    
