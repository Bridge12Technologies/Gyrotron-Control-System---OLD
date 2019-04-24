// the commented-out lines are for a later implimentation of pre-programmed command lists

#include "device.h"

ethdevice :: ethdevice()
{
    name = "";
    hostname = "";
    portno = 0;
    preread = false;
    socket = 0;
    pause = false;
    recon_count = -1;
}
//ethdevice :: ethdevice(std::string n, std::string h, int p, bool ir, bool pr) : name(n), hostname(h), portno(p), preread(pr) {}
ethdevice :: ethdevice(std::string n, std::string h, int p, bool pr, bool pa)
{
    name = n;
    hostname = h;
    portno = p;
    preread = pr;
    socket = 0;
    pause = pa;
    recon_count = -1;
}
ethdevice :: ~ethdevice() {}
bool ethdevice :: getpreread() { return preread; }
int ethdevice :: getportno() { return portno; }
std::string ethdevice :: gethostname() { return hostname; }
std::string ethdevice :: getname() { return name; }
int ethdevice :: getsock() { return socket; }
void ethdevice :: setsock(int s) { socket = s; }
bool ethdevice :: get_pause() { return pause; }
int ethdevice :: get_recon_count() { return recon_count; }
void ethdevice :: recon_tick() { recon_count++; }
void ethdevice :: reset_recon_count() { recon_count = -1; }

serdevice :: serdevice()
{
    name = "";
    port = "";
    baud = B9600;
    sCR = true;
    sLF = false;
    pread = false;
    fd = 0;
    sRead = false;
    recon_count = -1;
}
//serdevice :: serdevice(std::string n, std::string po, speed_t b, bool cr, bool lf) : name(n), port(po), baud(b), sCR(cr), sLF(lf) {}
serdevice :: serdevice(std::string n, std::string po, speed_t b, bool cr, bool lf, bool prd, bool sr)
{
    name = n;
    port = po;
    baud = b;
    sCR = cr;
    sLF = lf;
    pread = prd;
    fd = 0;
    sRead = sr; // stream read instead of recv
    recon_count = -1;
}
serdevice :: ~serdevice() {}
std::string serdevice :: getport() { return port; }
speed_t serdevice :: getbaud() { return baud; }
bool serdevice :: sendCR() { return sCR; }
bool serdevice :: sendLF() { return sLF; }
std::string serdevice :: getname() { return name; }
bool serdevice :: getpreread() { return pread; }
int serdevice :: get_fd() { return fd; }
void serdevice :: set_fd(int f) { fd = f; }
int serdevice :: open_stream() { in_stream.open(port); if(in_stream.is_open()) return 0; else return -1; }
int serdevice :: close_stream() { in_stream.close(); if(in_stream.is_open()) return -1; else return 0; }
bool serdevice :: using_str() { return sRead; }
std::string serdevice :: read_stream()
{
    std::string result;
    in_stream >> result;
    return result;
}
int serdevice :: get_recon_count() { return recon_count; }
void serdevice :: recon_tick() { recon_count++; }
void serdevice :: reset_recon_count() { recon_count = -1; }

serdevice SD_XGS600("XGS-600", "/dev/ttyUSB0", B9600, true, false, false, false);
serdevice SD_SCITECH("Scientech Power Meter", "/dev/ttyUSB0", B19200, true, true, false, true);
ethdevice ED_SPCE("Gamma Vac SPCe", (char *)"192.168.1.10", 23, true, false);
ethdevice ED_SG385("SRS SG385", (char *)"10.12.70.165", 5024, true, false);
ethdevice ED_DXM("Spellman DXM", (char *)"192.168.1.4", 50001, false, true);
ethdevice ED_SLM("Spellman SLM", (char *)"192.168.1.5", 50001, false, true);
ethdevice ED_RIGOL("Rigol DP821A", (char *)"192.168.1.3", 5555, false, false);
ethdevice ED_FMS("FMS", (char *)"10.12.70.121", 51717, false, true);
