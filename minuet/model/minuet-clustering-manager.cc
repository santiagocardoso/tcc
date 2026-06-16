/*
 * minuet-clustering-manager.cc
 *
 *  Created on: 22 de nov de 2018
 *      Author: everaldo
 */

#include "minuet-clustering-manager.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ClusteringManager");

/*********************** ClusteringManager ******************************/
	NS_OBJECT_ENSURE_REGISTERED (ClusteringManager);


	TypeId ClusteringManager::GetTypeId(void) {
		static TypeId tid =
				TypeId("ns3::ClusteringManager")
				.SetParent<Object>()
				.AddConstructor<ClusteringManager>()
				.AddAttribute("Node",
						"The Node of the Node", PointerValue(),
						MakePointerAccessor(&ClusteringManager::m_node),
						MakePointerChecker<Node>())
				.AddAttribute("CommunicationLayer",
						"The Communication Instance", PointerValue(),
						MakePointerAccessor(&ClusteringManager::m_communicationLayer),
						MakePointerChecker<CommunicationLayer>());

		return tid;
	}

	ClusteringManager::ClusteringManager() {
		NS_LOG_FUNCTION(this);
	}

	ClusteringManager::~ClusteringManager() {
		NS_LOG_FUNCTION(this);
	}

	void ClusteringManager::StartClustering() {
		//NS_LOG_FUNCTION(this);

		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					if(!m_dca->IsStarted()) {
						m_dca->StopClustering();
						PrintInLog("Starting Clustering Algorithm DCA Again");
					} else {
						NS_LOG_INFO("Impossible Start Clustering: Clustering already started!");
					}
				} else {
					m_dca = CreateObjectWithAttributes<DCA>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));
					m_dca->StartClustering();
					PrintInLog("Starting Clustering Algorithm DCA");
				}
				break;
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					if(!m_pctt->IsStarted()) {
						m_pctt->StartClustering(event);
						PrintInLog("Trying Starting Clustering Algorithm PCTT Again");
					} else {
						NS_LOG_INFO("Impossible Start Clustering: Clustering already started!");
					}

				} else {
					m_pctt = CreateObjectWithAttributes<PCTT>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));
					m_pctt->StartClustering(event);
					PrintInLog("Starting Clustering Algorithm PCTT");
				}
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					if(!m_social->IsStarted()) {
						m_social->StopClustering();
						PrintInLog("Starting Clustering Algorithm SOCIAL Again");
					} else {
						NS_LOG_INFO("Impossible Start Clustering: Clustering already started!");
					}
				} else {
					m_social = CreateObjectWithAttributes<SOCIAL>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));
					m_social->StartClustering();
					PrintInLog("Starting Clustering Algorithm SOCIAL");
				}
				break;
			default:
				cerr << "## ERROR: In StartClustering method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	void ClusteringManager::StopClustering() {
		NS_LOG_FUNCTION(this);

		PrintInLog("Stopping Clustering");
		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					if(m_dca->IsStarted()) {
						m_dca->StopClustering();
						m_dca->Dispose();
						m_dca = nullptr;
						PrintInLog("Stopping Clustering Algorithm DCA");
					} else {
						NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
					}
				} else {
					NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
				}
			break;
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					if(m_pctt->IsStarted()) {
						m_pctt->StopClustering();
						m_pctt->Dispose();
						m_pctt = nullptr;
						PrintInLog("Stopping Clustering Algorithm PCTT");
					} else {
						NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
					}
				} else {
					NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
				}
				break;
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					if(m_social->IsStarted()) {
						m_social->StopClustering();
						m_social->Dispose();
						m_social = nullptr;
						PrintInLog("Stopping Clustering Algorithm SOCIAL");
					} else {
						NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
					}
				} else {
					NS_LOG_INFO("Impossible Stop Clustering: Clustering already stopped!");
				}
				break;
			default:
				cerr << "## ERROR: In StopClustering method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	bool ClusteringManager::IsStarted() {
		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					return m_dca->IsStarted();
				} else {
					return false;
				}
			break;
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					return m_pctt->IsStarted();
				} else {
					return false;
				}
				break;
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					return m_social->IsStarted();
				} else {
					return false;
				}
			break;
			default:
				cerr << "## ERROR: In StopClustering method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	bool ClusteringManager::IsRelay() {
		//NS_LOG_FUNCTION(this);

		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					return m_dca->IsClusterHead();
				} else {
					cerr << "## ERROR: In IsClusterHead method DCA algorithm not started" << endl;
					exit(1);
				}
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					return m_pctt->IsClusterHead();
				} else {
					cerr << "## ERROR: In IsClusterHead method PCTT algorithm not started" << endl;
					exit(1);
				}
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					return m_social->IsRelay();
				} else {
					cerr << "## ERROR: In IsRelay method SOCIAL algorithm not started" << endl;
					exit(1);
				}

			default:
				cerr << "## ERROR: In IsRelay method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	bool ClusteringManager::IsClusterMember() {
		//NS_LOG_FUNCTION(this);

		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					return m_dca->IsClusterMember();
				} else {
					cerr << "## ERROR: In IsClusterMember method DCA algorithm not started" << endl;
					exit(1);
                }
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					return m_pctt->IsClusterMember();
				} else {
					cerr << "## ERROR: In IsClusterMember method DCA algorithm not started" << endl;
					exit(1);
				}
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					return m_social->IsClusterMember();
				} else {
					cerr << "## ERROR: In IsClusterMember method SOCIAL algorithm not started" << endl;
					exit(1);
				}
				break;
			default:
				cerr << "## ERROR: In IsClusterMember method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	bool ClusteringManager::IsIsolated() {
		//NS_LOG_FUNCTION(this);

		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					return m_dca->IsIsolated();
				} else {
					cerr << "## ERROR: In IsIsolated method DCA algorithm not started" << endl;
					exit(1);
				}
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					return m_pctt->IsIsolated();
				} else {
					cerr << "## ERROR: In IsIsolated method DCA algorithm not started" << endl;
					exit(1);
				}
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					return m_social->IsIsolated();
				} else {
					cerr << "## ERROR: In IsIsolated method SOCIAL algorithm not started" << endl;
					exit(1);
				}
			default:
				cerr << "## ERROR: In IsIsolated method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	bool ClusteringManager::IsGateway() {
			//NS_LOG_FUNCTION(this);

			switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
				case MinuetConfig::CLUSTERING_ALGORITHM::dca:
					if(m_dca) {
						return m_dca->IsIsolated();
					} else {
						cerr << "## ERROR: In IsIsolated method DCA algorithm not started" << endl;
						exit(1);
					}
                /*
				case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
					if(m_pctt) {
						return m_pctt->IsIsolated();
					} else {
						cerr << "## ERROR: In IsIsolated method DCA algorithm not started" << endl;
						exit(1);
					}
				case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
					// Empty //
					break;*/
				case MinuetConfig::CLUSTERING_ALGORITHM::social:
					if(m_social) {
						return m_social->IsGateway();
					} else {
						cerr << "## ERROR: In IsIsolated method SOCIAL algorithm not started" << endl;
						exit(1);
					}
				default:
					cerr << "## ERROR: In IsIsolated method CLUSTERING ALGORITHM not set correctly" << endl;
					exit(1);
			}
		}

	uint32_t ClusteringManager::GetRelayId(uint64_t op) {
		//NS_LOG_FUNCTION(this);

		switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
			case MinuetConfig::CLUSTERING_ALGORITHM::dca:
				if(m_dca) {
					//return m_dca->GetRelayId();
                    return m_dca->GetClusterId();
				} else {
					cerr << "## ERROR: In GetClusterId method DCA algorithm not started" << endl;
					exit(1);
				}
            /*
			case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
				if(m_pctt) {
					return m_pctt->GetClusterId();
				} else {
					cerr << "## ERROR: In GetClusterId method DCA algorithm not started" << endl;
					exit(1);
				}
			case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
				// Empty //
				break;*/
			case MinuetConfig::CLUSTERING_ALGORITHM::social:
				if(m_social) {
					return m_social->GetRelayId(op);
				} else {
					cerr << "## ERROR: In GetClusterId method SOCIAL algorithm not started" << endl;
					exit(1);
				}
			default:
				cerr << "## ERROR: In GetClusterId method CLUSTERING ALGORITHM not set correctly" << endl;
				exit(1);
		}
	}

	void ClusteringManager::SetIsGateway(bool isGateway) {
			//NS_LOG_FUNCTION(this);

			switch (MinuetConfig::SELECTED_CLUSTERING_ALGORITHM) {
				case MinuetConfig::CLUSTERING_ALGORITHM::dca:
					if(m_dca) {
						//return m_dca->GetRelayId();
                        return;
					} else {
						cerr << "## ERROR: In GetClusterId method DCA algorithm not started" << endl;
						exit(1);
					}
                /*
				case MinuetConfig::CLUSTERING_ALGORITHM::pctt:
					if(m_pctt) {
						return m_pctt->GetClusterId();
					} else {
						cerr << "## ERROR: In GetClusterId method DCA algorithm not started" << endl;
						exit(1);
					}
				case MinuetConfig::CLUSTERING_ALGORITHM::msdc:
					// Empty //
					break;*/
				case MinuetConfig::CLUSTERING_ALGORITHM::social:
					if(m_social) {
						m_social->SetIsGateway(isGateway);
						return;
					} else {
						cerr << "## ERROR: In node " << m_node->GetId() << " SetIsGateway method SOCIAL algorithm not started" << endl;
						exit(1);
					}
				default:
					cerr << "## ERROR: In SetisGateway method CLUSTERING ALGORITHM not set correctly" << endl;
					exit(1);
			}
		}

	void ClusteringManager::PrintInLog(string message) {
		ofstream os;
		os.open (MinuetConfig::LOG_FILE_CLUSTERING_MANAGER.c_str(), ofstream::out | ofstream::app);
		os << Simulator::Now().GetNanoSeconds() << "ns - ClusteringManager - Node #" << m_node->GetId() <<  ": " << message << endl;
		os.close();
		return; //removi LOG
	}

/*********************** END ClusteringManager ******************************/
}
