#ifndef LABJACK_H
#define LABJACK_H

class labjack
{
private:
    int handle, errorAddress;
    double * pnt;
public:
	labjack();
	~labjack();
    int lj_init();
    int lj_close();
    double * lj_read();
};

extern labjack daq;

#endif
