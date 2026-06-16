#ifndef PCTT_UTILS_H
#define PCTT_UTILS_H

#include "ns3/core-module.h"

#define PI 3.14159265

namespace ns3 {

class PCTTUtils {

public:

	virtual ~PCTTUtils ();
	//Raio de visão de um carro
	static constexpr double_t VIEW_RADIUS = 10.0;

	//Número máximo de saltos de uma mensagem
	const static int32_t MAX_HOPS = 2;

	//Tempo de convergência do cluster
	static constexpr double_t CONVERGENCE_TIME = 1.0;

	// Intervalo de tempo para envio de CHM antes da convergência
	static constexpr double_t SEND_CH_MESSAGE_BEFORE_CONVERGENCE_TIME_INTERVAL = 0.5;

	//Intervalo de tempo para o CH atualizar a lista de membros
	static constexpr double_t RESET_TIME_INTERVAL = 1.0;

	// Intervalo de tempo para manutenção da lista de membros apos um RESET TIME
	static constexpr double_t CHECK_ANSWERED_TIME = 0.2;

	//Intervalo de tempo para o CH procurar na lista de membros um CCH
	static constexpr double_t SEARCH_MEMBERS_LIST_TIME = 0.4;

	//Intervalo de tempo para os nós preverem suas posições
	static constexpr double_t PREDICTION_TIME_INTERVAL = 0.5;

	// Taxa de variação máxima de predição no eixo X em %
	static constexpr double_t MAX_VARIATION_RATE_IN_X = 1.0;

	// Taxa de variação máxima de predição no eixo Y em %
	static constexpr double_t MAX_VARIATION_RATE_IN_Y = 1.0;

	//Angulação máxima para que dois veículos estejam indo no mesmo sentido
	static constexpr double_t SAME_DIR_ANGLE = 35.0;

	// Tempo em que um NM deve esperar para se tronar CH após detectar um alvo
	static constexpr double_t CHANGE_NM_TO_CH_TIME = 0.2;

	//Tempo de manutenção da lista de eventos agendados
	const static uint64_t EVENTS_LIST_MAINTENANCE_TIMER = 1;
};

}


#endif
