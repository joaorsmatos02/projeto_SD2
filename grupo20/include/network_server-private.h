#include <arpa/inet.h>

int read_all(int sock, const uint8_t* buf, int len);

int write_all(int sock, const uint8_t* buf, int len);