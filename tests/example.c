#include <stdlib.h>
#include <string.h>

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
  // C2
  int prefix_length = strlen("https://www.ibm.com/");
  memcpy(buffer, "https://www.ibm.com/", prefix_length);
  size -= prefix_length;
  *length = rand() % (size-1);
  for (int i = 0; i < *length; ++i)
    // C3
    buffer[prefix_length+i] = (char) rand();
  // C4
  return 0;
}

int check_address(char* address, int length)
{
  // C8
  int prefix_length = strlen("https://www.ibm.com/");
  return (prefix_length < length)
    ? /* C9 */ (strncmp(address, "https://www.ibm.com/", length) != 0) : /* C10 */ 1;
  // C11
}

int send_data_at(char* address, int address_len)
{
  // address should start with a string coming from the text segment (see read_address2)
  // address should start with "https://www.ibm.com/"
  // C16
  send_sensitive_data_at(address, address_len);
  // C17
  return 0;
}

int main(int argc, char** argv)
{
  char buffer[500], buffer2[500];
  int buffer_size = 500;
  int address_len;

  // int ko = read_address(buffer, buffer_size, &address_len);
  // C1
  int ko = read_address2(buffer, buffer_size, &address_len);
  // C5
  char* address = buffer;
  if (ko)
    // C6
    return 1;
  // C7
  if (!check_address(address, address_len))
    // C8
    return 2;
  // C12
  f(); /* f may have buffer overflow that write in the address buffer */
  if (rand() % 2)
  {
    // C13
    strcpy(buffer2, address);
    address = buffer2;
  }
  // C14
  g(); /* g may have buffer overflow that write in the address buffer */
  // C15

  /* a long_jmp may go here */
  ko = send_data_at(address, address_len);
  // C18
  if (ko)
    return 3;
  return 0;
}
