#ifndef DCA_UTILS_H
#define DCA_UTILS_H

#include "ns3/core-module.h"

#define PI 3.14159265

namespace ns3 {

class DCAUtils {

public:

	virtual ~DCAUtils ();

	//Tempo de rechamar o método CleanUp
	static constexpr double_t CLEAN_UP_TIMER = 1.0;

	//Tempo de reenviar a mensagem beacon
	static constexpr double_t BEACON_MESSAGE_TIMER = 1.0;

	//Tempo de manutenção da lista de eventos agendados
	static constexpr double_t EVENTS_LIST_MAINTENANCE_TIMER = 1.0;

	//Tempo de incrementar o expire time
	static constexpr double_t EXPIRY_TIME_INTERVAL_1 = 10.0;
	static constexpr double_t EXPIRY_TIME_INTERVAL_2 = 9.0;
	static constexpr double_t EXPIRY_TIME_INTERVAL_3 = 7.0;
	static constexpr double_t EXPIRY_TIME_INTERVAL_4 = 5.0;
	static constexpr double_t EXPIRY_TIME_INTERVAL_5 = 3.0;

	//Separação para máxima CloseNeighbors
	static constexpr double_t MAX_SEPARATION_CLOSENEIGHBORS = 100.0;

	//Tempo de nova checagem
	static constexpr double_t CHECK_AGAIN = 1.0;

	//Threshold
	static constexpr double_t THRESHOLD = 0.2;

};

}


#endif
