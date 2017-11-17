#pragma once

//NTP time server URL and port (the time servers are listening on port number 123 using UDP).
#define URL "time-a.nist.gov"
#define PORT 123

//NTP time-stamp of 1 Jan 1970.
#define NTP_TIMESTAMP_DELTA 2208988800ull

//Delay for timer (milliseconds).
#define DELAY 1000

//ID for timer.
#define ID_TIMER_CLOCK 1

//Time between each query to the time server (seconds).
#define SECONDS_BETWEEN_EACH_QUERY 64

//Time difference compared to UTC (hours).
#define NEW_YORK (-5)
#define STOCKHOLM (+1)
#define MELBOURNE (+11)

//Fields in the NTP packet that need to be set before sending the packet.
#define LEAP_INDICATOR 0
#define VERSION_NUMBER 3
#define MODE 3

//NTP packet.
typedef struct
{
	unsigned char li : 2;            // Only two bits. Leap indicator.
	unsigned char vn : 3;            // Only three bits. Version number of the protocol.
	unsigned char mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

	unsigned __int8 stratum;         // Eight bits. Stratum level of the local clock.
	unsigned __int8 poll;            // Eight bits. Maximum interval between successive messages.
	unsigned __int8 precision;       // Eight bits. Precision of the local clock.

	unsigned __int32 rootDelay;      // 32 bits. Total round trip delay time.
	unsigned __int32 rootDispersion; // 32 bits. Max error aloud from primary clock source.
	unsigned __int32 refId;          // 32 bits. Reference clock identifier.

	unsigned __int32 refTm_s;        // 32 bits. Reference time-stamp seconds.
	unsigned __int32 refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

	unsigned __int32 origTm_s;       // 32 bits. Originate time-stamp seconds.
	unsigned __int32 origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

	unsigned __int32 rxTm_s;         // 32 bits. Received time-stamp seconds.
	unsigned __int32 rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

	unsigned __int32 txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
	unsigned __int32 txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;                        // Total: 384 bits or 48 bytes.