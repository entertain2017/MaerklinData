
#include <Arduino.h>

typedef struct { 
  uint8_t id;
  String loconame;
  String proto;
  String addr;
} locoDic;




class MaerklinData
{
public:
	MaerklinData();
	void begin(bool debug = false, bool verbose = false);
	void sendData();
	void getLocos();
	locoDic* get();
	


private:
	void readcanframes();
	void cangetloconame(int nr);
	int getnumloco();
	String printlocodata(int mode, int id);
	void cangetlocoaddr(String data);
	void clearlocodata();
};
