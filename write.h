#ifndef WRITE_H
#define WRITE_H
#include "device.h"

int eth_connect(ethdevice *dev);
int eth_disconnect(ethdevice *dev);
std::string eth_IO(std::string cmd_in, ethdevice *dev);
std::string eth_read(ethdevice *dev);
std::string eth_write(std::string cmd_in, ethdevice *dev);
int ser_connect(serdevice *dev);
int ser_disconnect(serdevice *dev);
std::string ser_IO(std::string cmd_in, serdevice *dev);
std::string ser_sIO(std::string cmd_in, serdevice *dev);
std::string ser_read(serdevice *dev);
std::string ser_write(std::string cmd_in, serdevice *dev);
std::string exec_cmd(const char* cmd);

#endif
