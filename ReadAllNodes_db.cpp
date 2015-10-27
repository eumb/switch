#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "mongo/client/dbclient.h" // for the driver
#include <ctime>
//#include <chrono>
//#include <curl/curl.h>
//#include "./RF24/librf24-rpi/librf24/RF24.h"
#include <RF24/RF24.h>
#include <unistd.h>
#define NODE_ID "Server"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/time_zone_base.hpp"


using namespace std;
using namespace mongo;
using mongo::BSONObj;
using mongo::BSONElement;
using mongo::BSONObj;
using mongo::BSONObjBuilder;
using namespace boost::posix_time;
// Radio pipe addresses for the 2 nodes to communicate.
// First pipe is for writing, 2nd, 3rd, 4th, 5th & 6th is for reading...
// Pipe0 in bytes is "serv1" for mirf compatibility
const uint64_t pipes[4] = {0xF0F0F0F0D1LL, 0xF0F0F0F0D2LL,0xF0F0F0F0D3LL,0xF0F0F0F0D9LL};
// CE and CSN pins On header using GPIO numbering (not pin numbers)
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

enum TYPES {
  BASE  = 100,
  SWITCH   = 1,
  TEMPERATURE   = 2,
  SOUND      = 3,
  SMOKE       = 4,
  PRESENCE    = 5,
  NEST = 7
};

//DATA
typedef struct {
  // id of the device -> max is 16 characters
  char id[16];
  // type of the device -> a simple int
  int16_t type;
  //data
  int16_t val[7];
} Payload;
Payload p;


typedef struct {
  // id of the device -> max is 16 characters
  char id[16];
  // type of the device -> a simple int
  int16_t type;
  //data
  int16_t val[7];
} Payload_sent;
Payload_sent sent;

void setup(void)
{

       // Refer to RF24.h or nRF24L01 DS for settings
        radio.begin();
        radio.enableDynamicPayloads();
        radio.setAutoAck(0);
        radio.setRetries(15,15);
        radio.setDataRate(RF24_250KBPS);
        radio.setPALevel(RF24_PA_MAX);
        radio.setChannel(64);
        radio.setCRCLength(RF24_CRC_16);


        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1,pipes[1]);
        radio.openReadingPipe(2,pipes[2]);
	    radio.openReadingPipe(3,pipes[3]);
        


        // Start Listening
        radio.startListening();

        radio.printDetails();

       /* sleep(1000);*/
        strcpy(sent.id, NODE_ID);
        sent.type = BASE;

}

/*
void connectDB(){
    BSONObj info;
    mongo::DBClientConnection c;
    mongo::client::initialize();
    try {
        c.connect("localhost:27017");
        //std::cout << "connected ok" << std::endl;
    }catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
    try {
        std::auto_ptr<mongo::DBClientCursor> cursor = c.query("readall.states", BSONObj());
                    
                    while (cursor->more())
                    {
                        mongo::BSONObj pi=cursor->next().getOwned();
                        //std::cout << pi.toString() << std::endl;

                    }

    }catch( const mongo::DBException &e ) {
        //std::cout << "caught " << e.what() << std::endl;
    }
}

*/


mongo::Date_t convert(const boost::posix_time::ptime& pt)
{
boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::date_time::Jan,1));
boost::posix_time::time_duration d = pt - epoch;
return mongo::Date_t(d.total_milliseconds());
}


void loop(){
    //To use in c++11
    // current date/time based on current system
    //std::time_t timeNow = std::time(0);
    // convert now to string form
    //char* dt = std::ctime(&timeNow);
    //std::cout << "The local date and time is: " << dt << endl;
    

    //time_t timer;
    //std::time_t result = std::time(&timer);
    
    boost::posix_time::ptime pt ( boost::posix_time::second_clock::universal_time());
    mongo::Date_t dt = convert(pt);

    int id_switch;
    string line;
    std::string value = "100";
    string id_switch_str;

   

    /* code below is used when dealing with command from file
    ifstream myfile ("command");
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            value = atoi(line.c_str());
            printf("Value %i...\n\r", value);
        }
        myfile.close();
    }else cout << "Unable to open file";

    */


    // looking for commad in DB

    
    /*
    try {
        c.logout("localhost:27017",info);
    }catch( const mongo::DBException &e ) {
        //std::cout << "caught " << e.what() << std::endl;
    }
    */


    BSONObj info;
    mongo::DBClientConnection c;
    mongo::client::initialize();
    try {
        c.connect("localhost:27017");
        std::cout << "connected ok" << std::endl;
    }catch( const mongo::DBException &e ) {
        std::cout << "caught " << e.what() << std::endl;
    }
    try {
        std::string type("switch");
        //std::auto_ptr<mongo::DBClientCursor> cursor = c.query("readall.states", MONGO_QUERY("id" << id ).sort("createdAt",-1),1);
          std::auto_ptr<mongo::DBClientCursor> cursor = c.query("readall.states", MONGO_QUERY("type" << type ).sort("createdAt",-1),1);               
                    while (cursor->more())
                    {
                        mongo::BSONObj pi=cursor->next().getOwned();
                        //std::cout << pi.toString() << std::endl;
                        std::cout << pi.getStringField("state") << endl;
                        value = pi.getStringField("state");
                        id_switch_str= pi.getStringField("id");
                        //std::cout << id_switch << std::endl;
                    }

    }catch( const mongo::DBException &e ) {
        //std::cout << "caught " << e.what() << std::endl;
    }

    
    id_switch = atoi(id_switch_str.c_str());


    stringstream ss;
    ss << id_switch;
    string id_to_insert = ss.str();


    if (value == "0"){
        radio.stopListening();
        sent.val[0]=0;
        sent.id[0]=id_switch;
        
        bool ok = radio.write( &sent, sizeof(sent) );
        printf("Now sending %i...", sent.val[0]);
        if (ok)
            printf("ok...");
        else
            printf("failed.\n\r");
		/*
        //Listen for ACK
		radio.startListening();
		
        //Let's take the time while we listen
		unsigned long started_waiting_at = millis();
		bool timeout = false;
		while ( ! radio.available() && ! timeout ) {
			usleep(5);
			if (millis() - started_waiting_at > 200 )
				timeout = true;
		}

		if( timeout ){
			//If we waited too long the transmission failed
			printf("Failed, response timed out.\n\r");
		}else{
        //If we received the message in time, let's read it and print it
        unsigned long got_time;
        radio.read( &got_time, sizeof(unsigned long) );
        printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
        //printf("Got response %lu",got_time);

        */

        radio.stopListening();

            // Take the time, and send it.  This will block until complete

            printf("Now sending...\n");
            unsigned long time = millis();

            bool ok1 = radio.write( &time, sizeof(unsigned long) );

            if (!ok){
                printf("failed.\n");
            }
            // Now, continue listening
            radio.startListening();

            // Wait here until we get a response, or timeout (250ms)
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while ( ! radio.available() && ! timeout ) {
                if (millis() - started_waiting_at > 200 )
                    timeout = true;
            }


            // Describe the results
            if ( timeout )
            {
                printf("Failed, response timed out.\n");
            }
            else
            {
                // Grab the response, compare, and send to debugging spew
                unsigned long got_time;
                radio.read( &got_time, sizeof(unsigned long) );

                // Spew it
                printf("Got response %lu, round-trip delay: %lu\n",got_time,millis()-got_time);
            


        value="100";

        mongo::DBClientConnection c;
        mongo::client::initialize();
        try {
            c.connect("localhost:27017");
            std::cout << "connected ok" << std::endl;
        }catch( const mongo::DBException &e ) {
            std::cout << "caught " << e.what() << std::endl;
        }
        try {
            BSONObj info;
            BSONObjBuilder b;
            b.appendDate("createdAt",dt);
            b.append("id", id_to_insert);
            b.append("state", value);
            b.append("type", "switch");
            b.append("lastValueSent","0");
            BSONObj toinsert = b.obj();
            c.insert("readall.states", toinsert);
            std::cout << "switch state inserted ok" << std::endl;
            c.logout("localhost:27017",info);
        }catch( const mongo::DBException &e ) {
            std::cout << "caught " << e.what() << std::endl;
        }




        ///////
        /*    
		value="100";
		ofstream myfile ("command");
		if (myfile.is_open()){
			myfile << "100";
			myfile.close();
		}
		else cout << "Unable to open file";
        */
		radio.startListening();
		}
	}


    if (value == "1"){

        radio.stopListening();
        sent.val[0]=1;
        sent.id[0]=id_switch;
      
        bool ok = radio.write( &sent, sizeof(sent) );
        printf("Now sending %i...", sent.val[0]);
        if (ok)
            printf("ok...");
        else
            printf("failed.\n\r");
        /*
        //Listen for ACK
        radio.startListening();
        //Let's take the time while we listen
        unsigned long started_waiting_at = millis();
        bool timeout = false;
        while ( ! radio.available() && ! timeout ) {
        usleep(1);
        if (millis() - started_waiting_at > 100 )
            timeout = true;
        }
        if( timeout ){
			//If we waited too long the transmission failed
			printf("Failed, response timed out.\n\r");
        }else{
			//If we received the message in time, let's read it and print it
			unsigned long got_time;
			radio.read( &got_time, sizeof(unsigned long) );
			printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
        */

            
        radio.stopListening();

            // Take the time, and send it.  This will block until complete

            printf("Now sending...\n");
            unsigned long time = millis();

            bool ok1 = radio.write( &time, sizeof(unsigned long) );

            if (!ok){
                printf("failed.\n");
            }
            // Now, continue listening
            radio.startListening();

            // Wait here until we get a response, or timeout (250ms)
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while ( ! radio.available() && ! timeout ) {
                if (millis() - started_waiting_at > 200 )
                    timeout = true;
            }


            // Describe the results
            if ( timeout )
            {
                printf("Failed, response timed out.\n");
            }
            else
            {
                // Grab the response, compare, and send to debugging spew
                unsigned long got_time;
                radio.read( &got_time, sizeof(unsigned long) );

                // Spew it
                printf("Got response %lu, round-trip delay: %lu\n",got_time,millis()-got_time);
            

        value="100";


        mongo::DBClientConnection c;
        mongo::client::initialize();
        try {
            c.connect("localhost:27017");
            std::cout << "connected ok" << std::endl;
        }catch( const mongo::DBException &e ) {
            std::cout << "caught " << e.what() << std::endl;
        }
        try {
            BSONObj info;
            BSONObjBuilder b;
            b.appendDate("createdAt",dt);
            b.append("id", id_to_insert);
            b.append("state", value);
            b.append("type", "switch");
            b.append("lastValueSent","1");
            BSONObj toinsert = b.obj();
            c.insert("readall.states", toinsert);
            std::cout << "switch state inserted ok" << std::endl;
            c.logout("localhost:27017",info);
        }catch( const mongo::DBException &e ) {
            std::cout << "caught " << e.what() << std::endl;
        }




        /*
        value="100";
        ofstream myfile ("command");
        if (myfile.is_open())
        {
            myfile << "100";
            myfile.close();
        }else cout << "Unable to open file";
        */
        radio.startListening();
		}
    }


    if (value=="100"){
		//char receivePayload[32];
        uint8_t pipe = 0;
        if ( radio.available(&pipe) ) {
            printf("%s\n","Receiving" );
            uint8_t len = radio.getDynamicPayloadSize();
            radio.read( &p, sizeof(p) );
            char state[5];
            char temp[5];
            char outBuffer[1024]="";
            strcat(outBuffer, "");
            strcat(outBuffer, "key=");
            strcat(outBuffer, p.id);
            char outBuffer1[1024]="";
            strcat(outBuffer1, "");
            strcat(outBuffer1, "key=");
            strcat(outBuffer1, p.id);
            int val = 0;
            if (p.type == SWITCH) {
                strcat(outBuffer," switch state= ");
                sprintf(state, "%i", p.val[val++]);
                strcat(outBuffer, state);
                // Display it on screen
                printf("\n\rRecv: size:%i pipe:%i type:%d data:%s\n\r",len,pipe,p.type,outBuffer);
                mongo::DBClientConnection c;
                mongo::client::initialize();
                try {
                    c.connect("localhost:27017");
                    std::cout << "connected ok" << std::endl;
                }catch( const mongo::DBException &e ) {
                    std::cout << "caught " << e.what() << std::endl;
                }
                try {
                    BSONObj info;
					BSONObjBuilder b;
		            //Using C++11  b.appendTimeT("updated_at",std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
					//mongo::Date_t dt = convert(pt);
                    //b.appendTimeT("updated_at",mongo::Date_t(pt));
                    b.appendDate("updated_at",dt);
                    b.append("type","Switch");
                    b.append("switch_id", p.id);
                    b.append("switch_state", p.val[0]);
                    BSONObj value = b.obj();
                    c.insert("readall.values", value);
                    std::cout << "switch state inserted ok" << std::endl;
                    c.logout("localhost:27017",info);
                }catch( const mongo::DBException &e ) {
                  std::cout << "caught " << e.what() << std::endl;
                }
			}else if (p.type == NEST){
                if ((p.type & TEMPERATURE) == TEMPERATURE) {
                        strcat(outBuffer1," temp=");
                        sprintf(temp, "%i", p.val[val++]);
                        strcat(outBuffer1, temp);
                }
                if ((p.type & SOUND) == SOUND) {
                        strcat(outBuffer1," sound=");
                        sprintf(temp, "%i", p.val[val++]);
                        strcat(outBuffer1, temp);
                }
                if ((p.type & SMOKE) == SMOKE) {
                        strcat(outBuffer1," smoke=");
                        sprintf(temp, "%i", p.val[val++]);
                        strcat(outBuffer1, temp);
                }
                if ((p.type & PRESENCE) == PRESENCE) {
                        strcat(outBuffer1," presence=");
                        sprintf(temp, "%i", p.val[val++]);
                        strcat(outBuffer1, temp);
                }
                // Display it on screen
                printf("\n\rRecv: size:%i pipe:%i type:%d data:%s\n\r",len,pipe,p.type,outBuffer1);
                //std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;  
                mongo::DBClientConnection c;
                mongo::client::initialize();
                try {
                    c.connect("localhost:27017");
                    std::cout << "connected ok" << std::endl;
                } catch( const mongo::DBException &e ) {
                    std::cout << "caught " << e.what() << std::endl;
                }
                try {
                    BSONObj info;
                    BSONObjBuilder b;
		            //b.appendTimeT("updated_at",std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
                    b.appendDate("updated_at",dt);
                    b.append("type","Nest");
		            b.append("temperature", p.val[0]);
                    b.append("sound", p.val[1]);
                    b.append("smoke", p.val[2]);
                    b.append("presence", p.val[3]);
                    BSONObj value = b.obj();
                    c.insert("readall.values", value);
                    std::cout << "node sensor values inserted ok" << std::endl;
		            c.logout("localhost:27017",info);
                }catch( const mongo::DBException &e ) {
                std::cout << "caught " << e.what() << std::endl;
                }
            }
        }



        //read on all pipes
        pipe++;
        if ( pipe > 4 ) pipe = 1;

        sleep(2);
}
}


int main(int argc, char** argv)
{
    setup();
    //connectDB();
    while(1)
        loop();

    return 0;
}


