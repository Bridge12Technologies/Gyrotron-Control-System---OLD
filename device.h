#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <termios.h>      // POSIX terminal control definitions
#include <fstream>

class ethdevice
{
private:
    int portno, socket, recon_count;
    std::string name, hostname;
    bool preread, pause;
public:
	ethdevice();
    ethdevice(std::string n, std::string h, int p, bool pr, bool pa);
	~ethdevice();
    bool getpreread();
	int getportno();
	std::string gethostname();
    std::string getname();
    int getsock();
    void setsock(int s);
    bool get_pause();
    int get_recon_count();
    void recon_tick();
    void reset_recon_count();
};

class serdevice
{
private:
	std::string name, port; 
	speed_t baud; 
    bool isRS, sCR, sLF, pread, sRead, pause;
    int fd, recon_count;
    std::ifstream in_stream;
public:
	serdevice();
    serdevice(std::string n, std::string po, speed_t b, bool cr, bool lf, bool intr, bool sr);
	~serdevice();
	std::string getport();
    speed_t getbaud();
	bool sendCR();
	bool sendLF();
    std::string getname();
    bool getpreread();
    int get_fd();
    void set_fd(int f);
    int open_stream();
    int close_stream();
    std::string read_stream();
    bool using_str();
    int get_recon_count();
    void recon_tick();
    void reset_recon_count();
};

extern serdevice SD_XGS600;
extern serdevice SD_SCITECH;
extern ethdevice ED_SPCE;
extern ethdevice ED_SG385;
extern ethdevice ED_DXM;
extern ethdevice ED_SLM;
extern ethdevice ED_RIGOL;
extern ethdevice ED_FMS;

#endif
