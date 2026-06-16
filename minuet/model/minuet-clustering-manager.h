/*
 * minuet-clustering-manager.h
 *
 *  Created on: 22 de nov de 2018
 *      Author: everaldo
 */

#ifndef MINUET_CLUSTERING_MANAGER_H_
#define MINUET_CLUSTERING_MANAGER_H_

#include "dca.h"
#include "pctt.h"
#include "social.h"

namespace ns3 {

class DCA;
class PCTT;
class SOCIAL;
class CommunicationLayer;

class ClusteringManager : public Object {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	ClusteringManager();
	virtual ~ClusteringManager();

	void StartClustering();
	void StopClustering();

	bool IsStarted();

	bool IsRelay();
	bool IsClusterMember();
	bool IsIsolated();
	bool IsGateway();
	void SetIsGateway(bool isGateway);
	uint32_t GetRelayId(uint64_t op);

private:
	void PrintInLog(string message);

	Ptr<Node> m_node;
	Ptr<CommunicationLayer> m_communicationLayer;

	Ptr<DCA> m_dca;
	Ptr<PCTT> m_pctt;
	Ptr<SOCIAL> m_social;
};

} // Namespace ns3

#endif /* MINUET_CLUSTERING_MANAGER_H_ */
