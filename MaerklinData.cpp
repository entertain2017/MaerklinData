#include "MaerklinData.h"
#include <Arduino.h>
#include <FlexCAN.h>

byte locodata[200][8] = {};
bool DEBUG = false;
bool VERBOSE = false;


static CAN_message_t outMsg1;
MaerklinData::MaerklinData(){

};

  locoDic dicArr[30] {
    {0, "Loconame", "Protocol", "Address"},

    

};


void MaerklinData::begin(bool debug, bool verbose){
// Setup CAN interface and CAN Filter for receivimg Extended CAN Frames
	if (debug == true) {
		DEBUG = true;
	} else {
		DEBUG = false;
	}
	if (verbose == true) {
		VERBOSE = true;
	} else {
		VERBOSE = false;
	}
	if (DEBUG == true) {              
		Serial.println("Function begin... Seting up CAN interface 250000bps");
	}
	// Set can interface speed 250000 for Maerklin CAN Bus
	Can1.begin(250000);
	CAN_filter_t allPassFilter;
	allPassFilter.id=0;
	// Set the parameter for extended CAN Frames NEEDED!!
	allPassFilter.ext=1;
	allPassFilter.rtr=0;

	//leave the first 4 mailboxes to use the default filter. Just change the higher ones
	for (uint8_t filterNum = 4; filterNum < 16;filterNum++){
		Can1.setFilter(allPassFilter,filterNum); 
	}
};



void MaerklinData::getLocos(){
// Call simple functions to get to Loco Data from MS2 (CS2 currently not working)
if (DEBUG == true) {
	Serial.println("Function getLocos... will call other functions to walk thru all Locos in MS2/CS2 Database");
}
	cangetloconame(0);
	char s = getnumloco() - '0';
	String a ;
	for (int x=0; x <= s-1; x++){
		cangetloconame(x);
		a = printlocodata(0,x);
		cangetlocoaddr(a);
		dicArr[x].loconame = a;
		printlocodata(1,x);

	};
	
};

locoDic* MaerklinData::get(){
// Return the collected Data as Struct
	return dicArr;
}


void MaerklinData::cangetlocoaddr(String data){
// Send the correct CAN Frame to get Data from MS2 ( works also for CS2.exe)

 if (DEBUG == true) { 
  Serial.print("Function cangetlocoddr... will send can frame to get Loco Data [lokinfo]. Will ask for Loco : ");
  Serial.println(data);
}
  outMsg1.id = 0x0040eb1b;
  outMsg1.ext = 1;
  outMsg1.len = 8;
  outMsg1.buf[0] = 0x6C;
  outMsg1.buf[1] = 0x6F;
  outMsg1.buf[2] = 0x6B;
  outMsg1.buf[3] = 0x69;
  outMsg1.buf[4] = 0x6E;
  outMsg1.buf[5] = 0x66;
  outMsg1.buf[6] = 0x6F;
  outMsg1.buf[7] = 0x00;

// Send Frame 1 Message [loknamen]
  Can1.write(outMsg1);
  outMsg1.buf[0] = data[0];
  outMsg1.buf[1] = data[1];
  outMsg1.buf[2] = data[2];
  outMsg1.buf[3] = data[3];
  outMsg1.buf[4] = data[4];
  outMsg1.buf[5] = data[5];
  outMsg1.buf[6] = data[6];
  outMsg1.buf[7] = data[7];
  Can1.write(outMsg1);
  if ( data.length() >= 8 ) {
    outMsg1.buf[0] = data[8];
    outMsg1.buf[1] = data[9];
    outMsg1.buf[2] = data[10];
    outMsg1.buf[3] = data[11];
    outMsg1.buf[4] = data[12];
    outMsg1.buf[5] = data[13];
    outMsg1.buf[6] = data[14];
    outMsg1.buf[7] = data[15];
    Can1.write(outMsg1);
  }
  outMsg1.buf[0] = 0x00;
  outMsg1.buf[1] = 0x00;
  outMsg1.buf[2] = 0x00;
  outMsg1.buf[3] = 0x00;
  outMsg1.buf[4] = 0x00;
  outMsg1.buf[5] = 0x00;
  outMsg1.buf[6] = 0x00;
  outMsg1.buf[7] = 0x00;
  Can1.write(outMsg1);
  

    

readcanframes();
};

void MaerklinData::readcanframes(){
// Wait for CAN Frame with Ext ID 0x0042EB1B and Store in 3D Byte Array
if (DEBUG == true) {              
	Serial.println("Function readcanframes... wait for Loco Data CAN Frames and store it into 3D Byte Array.. ID 0x0042EB1B");
}
  
int c = 0;
CAN_message_t inMsg;
  	while (true) {
    		while (Can1.available()) 
    		{
			Can1.read(inMsg);
			if (inMsg.id == 0x0042EB1B) {
				for (int i=0; i <= 7; i++){
		  			locodata[c][i] = inMsg.buf[i];
				}
			c++;
			}
		}
    
    		if ( locodata[0][2] == 0x00 ){ if ( locodata[0][3] / 8 + 1 == c) { break;} }
		if ( locodata[0][2] != 0x00 ) {
		      float f = int(locodata[0][2]) * 256 + int(locodata[0][3]);
		      if ( f / 8 +1  <= c) { break; }
  		}
	}  
c = 0;
};

void MaerklinData::cangetloconame(int nr){
	if (DEBUG == true) {              
		Serial.println("Function cangetloconame... will send CAN Frame to get Loco Data [loknamen] ");
	}
	outMsg1.id = 0x0040eb1b;
	outMsg1.ext = 1;
	outMsg1.len = 8;
	outMsg1.buf[0] = 0x6c;
	outMsg1.buf[1] = 0x6F;
	outMsg1.buf[2] = 0x6B;
	outMsg1.buf[3] = 0x6E;
	outMsg1.buf[4] = 0x61;
	outMsg1.buf[5] = 0x6D;
	outMsg1.buf[6] = 0x65;
	outMsg1.buf[7] = 0x6E; 
	Can1.write(outMsg1);

	outMsg1.id = 0x0040eb1b;
	outMsg1.ext = 1;
	outMsg1.len = 6;
	if (nr < 10) {
		if (DEBUG == true) {              
			Serial.println("Function cangetloconame... need to split into two CAN Frames");
		}
		outMsg1.buf[0] = uint8_t(nr)+48;
		outMsg1.buf[1] = 0x00;
	}
	else {
		int w = nr;
		w = w % 10;
		int h = nr - w / 10;

		Serial.print(w);
		Serial.print(h);
		outMsg1.buf[0] = uint8_t(h)+48;
		outMsg1.buf[1] = uint8_t(w)+48;
	}

	outMsg1.buf[2] = 0x20;
	outMsg1.buf[3] = 0x00;
	outMsg1.buf[4] = 0x00;
	outMsg1.buf[5] = 0x00;

	Can1.write(outMsg1);


	readcanframes();
};

int MaerklinData::getnumloco(){
  unsigned int numloco = 0;

  int ff = 0;
  ff = locodata[0][2];
  ff = ff*256;
  ff |= locodata[0][3];
  ff = ff / 8 ;
if (DEBUG == true) {

  Serial.print("Function getnumloco... Reads Loco Data [Loknamen] and extract number of locos. Count: ");
  Serial.println(ff);
  Serial.println(locodata[0][2],HEX);
}
      // MORE THE 9 LOCO STUPID SPLIT INTO TWO BYTES)

    
      for (int i=0; i <= 7; i++){
        
        if (locodata[ff][i] == 0x0A) {
            
            if ( i == 0) {
		if (DEBUG == true) {              
			Serial.println("Function getnumloco... Checking EOM.. need to jump to last line to read Loco count.");
		}
              if ( locodata[ff-1][7] >= 9 and locodata[ff-1][7] != 0 ){
			if (DEBUG == true) {
				Serial.println("Function getnumloco... Checking if loco count is split up in two bytes.. NO");
			}
                numloco = locodata[ff-1][7];
              }
              else {
              numloco = locodata[ff-1][6] * 256;
              numloco |= locodata[ff-1][7];
		if (DEBUG == true) {              
			Serial.println("Function getnumloco... Checking if loco count is split up in two bytes.. YES");
		}
              }
            }
            else {
              numloco = (locodata[ff-1][6] << 8 ) | ( locodata[ff-1][7] & 0xFF);
		if (DEBUG == true) {
			if (DEBUG == true) {              
			Serial.println("Function getnumloco...!§$%");
			}
		}
            }
        }
      
      
    
    }

  return numloco;
};

String MaerklinData::printlocodata(int mode, int id) {


          //DUMP FULL BYTE ARRAY 
         if (DEBUG == true and VERBOSE == true) {
     for (int x=0; x <= int(sizeof(locodata)/sizeof(locodata[0]))-1; x++){
                  char a[8]  ;
                  //char aa[50];
                  sprintf(a,"%02X%02X%02X%02X%02X%02X%02X%02X", locodata[x][0],locodata[x][1],locodata[x][2],locodata[x][3],locodata[x][4],locodata[x][5],locodata[x][6],locodata[x][7]);
                  //sprintf(aa,"%c%c%c%c%c%c%c%c", locodata[x][0],locodata[x][1],locodata[x][2],locodata[x][3],locodata[x][4],locodata[x][5],locodata[x][6],locodata[x][7]);
                  Serial.println(a);
                  //Serial.print("\t");
                  //Serial.println(aa);
                  if ( locodata[x][2] == 0x00 and locodata[x][0] == 0x00 and locodata[x][1] == 0x00 and locodata[x+1][0] == 0x00 ) { break; };
                  
              
              }
           } 
        int cc = 1;
      //Serial.println(FreeMem());
      //Serial.println(char(locodata[0][3]) ,DEC);
      String aaa = "";
      
        for (int x=1; x <= int(sizeof(locodata)/sizeof(locodata[0]))-1; x++){
          for (int i=0; i <= 7; i++){
            //Serial.print(char(locodata[x][i]));
            if ( locodata[x][i] != 0x0A ){
              //Serial.print(char(locodata[x][i]));
              
                
                aaa.concat(char(locodata[x][i]));
                //Serial.print(char(locodata[x][i]));
                //char(locodata[x][i]) 
                
                cc++;
             
            }


      
    }
  }
 
String addr, type = ""; 
//Serial.print("locodata: ");
//Serial.println(aaa);
if ( mode == 0) {
  if ( aaa.indexOf('[' == 0)){
    // MS2 SPLIT MAGIC
    int pos = aaa.indexOf('=');
    aaa.remove(0,pos+1);
    pos = aaa.indexOf('[');
    aaa.remove(pos, aaa.length());
    pos = aaa.indexOf('=');
    aaa.trim();
  }
}
if ( mode == 1) {
    if ( aaa.indexOf('[' == 0)){
      // MS2 SPLIT MAGIC
      int pos = aaa.indexOf('=');
      aaa.remove(0,pos+1);
      pos = aaa.indexOf('.');
      addr = aaa;
      addr.remove(pos, addr.length());
      addr.trim();
      pos = aaa.indexOf('.');
      aaa.remove(0,pos+1);
      pos = aaa.indexOf('.');
      aaa.remove(0,pos+1);
      pos = aaa.indexOf('.');
      aaa.remove(0,pos+1);
      pos = aaa.indexOf('=');
      aaa.remove(0,pos+1);
      pos = aaa.indexOf('.');
      aaa.remove(pos, aaa.length());
      aaa.trim();
     
     
  }
 
}
if (DEBUG == true) {
Serial.print("Function printlocldata... Loco Data Found..LOCO NAME : ");
Serial.print(dicArr[id].loconame);
Serial.print(" LOCO TYPE : ");
Serial.print(aaa);
Serial.print(" LOCO ADDR : ");
Serial.println(addr);
}
dicArr[id].proto = aaa;
dicArr[id].addr = addr;
clearlocodata();
return aaa;
};



void MaerklinData::clearlocodata() {
          int co = char(locodata[0][3])/8+1;
          for (int x=0; x <= co; x++){
          for (int i=0; i <= 7; i++){
            locodata[x][i] = 0;
          }
          }
};

