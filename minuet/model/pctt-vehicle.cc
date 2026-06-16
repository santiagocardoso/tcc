#include "pctt-vehicle.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PCTTVehicle");

/*****************  PCTTVehicle  **********************/
NS_OBJECT_ENSURE_REGISTERED(PCTTVehicle);

TypeId PCTTVehicle::GetTypeId () {
	static TypeId typeId = TypeId("ns3::PCTTVehicle")
							.SetParent<Object> ()
							.AddConstructor<PCTTVehicle>()
							.AddAttribute("Id",
										  "The Id of the Node", IntegerValue(),
										  MakeIntegerAccessor(&PCTTVehicle::m_id),
										  MakeIntegerChecker<int32_t>())
							.AddAttribute("TargetDetectionValue",
										  "Detection status of the vehicle", BooleanValue(false),
										  MakeBooleanAccessor(&PCTTVehicle::m_targetDetectionValue),
										  MakeBooleanChecker())
							.AddAttribute("CHID",
										  "The Id of the Cluster Head", IntegerValue(-1),
										  MakeIntegerAccessor(&PCTTVehicle::m_chid),
										  MakeIntegerChecker<int32_t>())
							.AddAttribute("TargetPosition",
										 "The target position of the vehicle", VectorValue(),
										 MakeVectorAccessor(&PCTTVehicle::m_targetPosition),
										 MakeVectorChecker())
							.AddAttribute("TargetVelocity",
										 "The target velocity of the vehicle", VectorValue(),
										  MakeVectorAccessor(&PCTTVehicle::m_targetVelocity),
										  MakeVectorChecker())
							.AddAttribute ("MobilityModel",
										   "The mobility model of the node", PointerValue(),
										   MakePointerAccessor(&PCTTVehicle::m_mobilityModel),
										   MakePointerChecker<MobilityModel>());
	return typeId;
}

PCTTVehicle::PCTTVehicle() {/* Empty */}

PCTTVehicle::~PCTTVehicle() {
	NS_LOG_DEBUG("Class PCTTVehicle: ~PCTTVehicle Method");
}

//SERVE APENAS PARA CALCULAR OBT DE ALVOS FIXOS
Time PCTTVehicle::CalculateOBT(Vector targetPosition, Vector targetVelocity, Vector vehiclePosition, Vector vehicleVelocity) {
	Time obt;

	double radius = PCTTUtils::VIEW_RADIUS;
	double_t theta = GetAngle(vehicleVelocity); //ANGULO DO OBSERVADOR

	double vc = vehicleVelocity.GetLength();
	double xc = vehiclePosition.x;
	double xt = targetPosition.x;
	double yc = vehiclePosition.y;
	double yt = targetPosition.y;

	double a = (vc*vc) * ((cos(theta)*cos(theta)) + (sin(theta)*sin(theta)));

	double b = (2 * vc) * ((xc*cos(theta)) - (xt*cos(theta)) + (yc*sin(theta)) - (yt * sin(theta)));

	double c = (xt*xt) + (xc*xc) + (yt*yt) + (yc*yc) - (2*xt*xc) - (2*yt*yc) - (radius*radius);

	double delta = (b*b) - 4*a*c;

	if (delta < 0) {
		obt = Seconds (0);
	} else {

		if (a == 0.0) {
			if (b == 0.0 ) {
				if (CalculateDistance(vehiclePosition, targetPosition) < radius) {
					obt = Seconds (10.0);
				} else {
					obt = Seconds (0);
				}
			} else {
				obt = Seconds ((-c)/(b));
			}
		} else {

			double x1 = (-b + sqrt(delta))/(2*a);
			double x2 = (-b - sqrt(delta))/(2*a);

			if((x1 < 0 && x2 < 0) || (x1 > 0 && x2 > 0) || (x1 == 0 && x2 == 0)) { //OUT OF FOV
				obt = Seconds (0);
			} else { //IN FOV

				if (CalculateDistance(vehiclePosition, targetPosition) > radius) {
					std::cerr << "## ERROR: Vehicle out of FOV, but with different sign roots." << std::endl;
					exit(EXIT_FAILURE);
				}

				if (x1 >= 0) {
					obt = Seconds (x1);
				} else {
					obt = Seconds (x2);
				}
			}
		}
	}

	return obt;
}

int32_t PCTTVehicle::GetId() {
	return m_id;
}

void PCTTVehicle::SetID (int32_t id) {
	m_id = id;
}

int32_t PCTTVehicle::GetCHID() {
	return m_chid;
}

void PCTTVehicle::SetCHID (int32_t chid) {
	m_chid = chid;
}

Vector PCTTVehicle::GetTargetPosition() {
	return m_targetPosition;
}

void PCTTVehicle::SetTargetPosition(Vector targetPosition) {
	m_targetPosition = targetPosition;
}

Vector PCTTVehicle::GetTargetVelocity() {
	return m_targetVelocity;
}

void PCTTVehicle::SetTargetVelocity(Vector targetVelocity) {
	m_targetVelocity = targetVelocity;
}

Ptr<MobilityModel> PCTTVehicle::GetMobilityModel() {
	return m_mobilityModel;
}

void PCTTVehicle::SetMobilityModel(Ptr<MobilityModel> mobilityModel) {
	m_mobilityModel = mobilityModel;
}

bool PCTTVehicle::GetTDV (Vector targetPosition, Vector targetVelocity) {
	bool tdv = false;

	Time observationTime = CalculateOBT(targetPosition, targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());

	if (observationTime.Compare(Seconds(0)) == 1) {
		tdv = true;
	}

	return tdv;
}

void PCTTVehicle::SetTDV (bool tdv) {
	m_targetDetectionValue = tdv;
}

bool PCTTVehicle::SimpleGetTDV () {
	return m_targetDetectionValue;
}

Vector PCTTVehicle::PredictNextPosition (Vector lastPos, Vector currentPos) {
	Vector predictedPos;

	predictedPos.y  = currentPos.y + (currentPos.y - lastPos.y);
	predictedPos.x  = currentPos.x + (currentPos.x - lastPos.x);

	return predictedPos;
}

Ptr<PCTTClusterHead> PCTTVehicle::ToClusterHead() {
	return CreateObjectWithAttributes<PCTTClusterHead>("Id", IntegerValue(GetId()), "CHID", IntegerValue(GetId()),
				"TargetPosition", VectorValue(m_targetPosition), "TargetVelocity", VectorValue(m_targetVelocity),
				"MobilityModel", PointerValue(m_mobilityModel));
}

Ptr<PCTTClusterMember> PCTTVehicle::ToClusterMember() {
	// Hook Method
	return nullptr;
}

Ptr<PCTTNonMember> PCTTVehicle::ToNonMember() {
	return CreateObjectWithAttributes<PCTTNonMember>("Id", IntegerValue(GetId()), "CHID", IntegerValue(GetId()),
		"TargetPosition", VectorValue(m_targetPosition), "TargetVelocity", VectorValue(m_targetVelocity), "MobilityModel", PointerValue(m_mobilityModel));
}

double_t PCTTVehicle::GetAngle(Vector v1) {
	NS_LOG_FUNCTION(this);

	double_t diff = 0.0;

	Vector v2;
	v2.x = 1;
	v2.y = 0;

	double l1 = v1.GetLength();
	double l2 = v2.GetLength();

	if ((l1 != 0) && (l2 != 0)) {
		diff = acos(((v1.x*v2.x) + (v1.y*v2.y)) / (l1*l2));
	}

	return diff;//RETORNANDO EM RADIANOS
}

/*****************  PCTTVehicle END  **********************/

/*****************  PCTTClusterHead  **********************/
NS_OBJECT_ENSURE_REGISTERED(PCTTClusterHead);

TypeId PCTTClusterHead::GetTypeId () {
	static TypeId typeId = TypeId("ns3::PCTTClusterHead")
						.SetParent<PCTTVehicle> ()
						.AddConstructor<PCTTClusterHead>()
						.AddAttribute("CCHID",
									  "The Id of the Candidate Cluster Head", IntegerValue(-1),
									  MakeIntegerAccessor(&PCTTClusterHead::m_cchid),
									  MakeIntegerChecker<int32_t>());
	return typeId;
}

PCTTClusterHead::PCTTClusterHead() {/* Empty */}

PCTTClusterHead::~PCTTClusterHead() {
	NS_LOG_DEBUG("Class PCTTClusterHead: ~PCTTClusterHead Method");
}

int32_t PCTTClusterHead::GetCCHID() {
	NS_LOG_FUNCTION(this);

	return m_cchid;
}

void PCTTClusterHead::SetCCHID (int32_t cchid) {
	NS_LOG_FUNCTION(this);

	m_cchid = cchid;
}

vector<PCTTClusterHead::CM>& PCTTClusterHead::GetMemberList() {
	NS_LOG_FUNCTION(this);

	return m_memberList;
}

void PCTTClusterHead::SetMemberList(vector<CM> memberList) {
	NS_LOG_FUNCTION(this);

	m_memberList = memberList;
}

void PCTTClusterHead::SearchCHAndCCHInMemberList() {
	NS_LOG_FUNCTION(this);

	if(!m_memberList.empty()) {
		PCTTClusterHead::CM ch = *m_memberList.begin();
		PCTTClusterHead::CM cch = ch;

		for(PCTTClusterHead::CM vehicle : m_memberList) {
			if (vehicle.obt.Compare(ch.obt) == 1) {
				cch = ch;
				ch = vehicle;
			}
		}

		Time myOBT = CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());

		/// REMOVE Only Validation
		double_t distance = CalculateDistance(m_targetPosition, m_mobilityModel->GetPosition());
		if((distance < 10) && (myOBT.GetNanoSeconds() == 0)) {
			cout << "OBT = 0 but in a short distance" << endl;
			exit(EXIT_FAILURE);
		}
		////////////////////

		if(ch.obt.Compare(myOBT) == 1) {
			m_chid = ch.id;
			m_cchid = ch.id;
		} else {
			m_chid = GetId();
			(ch.obt.Compare(Seconds(0)) == 0) ? m_cchid = -1 : m_cchid = ch.id;
		}

	} else { // Vehicle isolated
		m_chid = GetId();
		m_cchid = -1;
	}
}

Ptr<PCTTClusterMember> PCTTClusterHead::ToClusterMember() {
	NS_LOG_FUNCTION(this);

	return CreateObjectWithAttributes<PCTTClusterMember>("Id", IntegerValue(GetId()), "CHID", IntegerValue(GetCCHID()),
		"TargetPosition", VectorValue(m_targetPosition), "TargetVelocity", VectorValue(m_targetVelocity), "MobilityModel", PointerValue(m_mobilityModel),
		"LastPosition", VectorValue(m_mobilityModel->GetPosition()), "LastVelocity", VectorValue(m_mobilityModel->GetVelocity()));
}

void PCTTClusterHead::AddToMemberList(PCTTClusterHead::CM cm) {
	NS_LOG_FUNCTION(this);

	bool alreadyIn = false;

	if (!m_memberList.empty()) {
		for (vector<PCTTClusterHead::CM>::iterator cm2 = m_memberList.begin() ; cm2 != m_memberList.end() ; cm2++) {
			if ((*cm2).id == cm.id) {
				alreadyIn = true;
				(*cm2).lastPosition = cm.lastPosition;
				(*cm2).lastVelocity = cm.lastVelocity;
				(*cm2).obt = cm.obt;
				(*cm2).answered = true;
			}
		}
	}

	if (!alreadyIn) {
		cm.answered = false;
		m_memberList.push_back(cm);
	}

}

void PCTTClusterHead::UpdateNodePosition(int32_t nodeId, Vector lastPosition, Vector lastVelocity) {
	NS_LOG_FUNCTION(this);

	if(!m_memberList.empty()) {
		for (PCTTClusterHead::CM cm : m_memberList) {
			if (cm.id == nodeId) {
				cm.lastPosition = lastPosition;
				cm.lastVelocity = lastVelocity;
			}
		}
	}
}

double_t PCTTClusterHead::GetAngleDiff (Vector v1, Vector v2) {
	NS_LOG_FUNCTION(this);

	double_t diff = 0.0;

	double l1 = v1.GetLength();
	double l2 = v2.GetLength();

	if ((l1 != 0) && (l2 != 0)) {
		diff = acos(((v1.x*v2.x) + (v1.y*v2.y)) / (l1*l2));
	}

	return (diff * 180) / PI;   //Convert from Radius to Degrees
}

bool PCTTClusterHead::IsEqualTargetDirection (Vector targetVelocity, Vector targetPosition, Vector vehicleVelocity, Vector vehiclePosition) {

	if((targetVelocity.GetLength() != 0) && (vehicleVelocity.GetLength() != 0)) { // If the target and vehicle are moving

		if (GetAngleDiff(targetVelocity, vehicleVelocity) < PCTTUtils::SAME_DIR_ANGLE)
			return true;
		else
			return false;

	} else if(vehicleVelocity.GetLength() != 0) { // If only the target is stopped

		double_t Tx = 0.0;
		double_t Ty = 0.0;

		Tx = ((targetPosition.x - vehiclePosition.x) / (vehicleVelocity.x));
		Ty = ((targetPosition.y - vehiclePosition.y) / (vehicleVelocity.y));

		if((0 <= Tx) && (0 <= Ty))
			return true;
		else
			return false;

	} else if(targetVelocity.GetLength() != 0) { // If only the vehicle is stopped

		double_t Tx = 0.0;
		double_t Ty = 0.0;

		Tx = ((vehiclePosition.x - targetPosition.x) / (targetVelocity.x));
		Ty = ((vehiclePosition.y - targetPosition.y) / (targetVelocity.y));

		if((0 <= Tx) && (0 <= Ty))
			return true;
		else
			return false;
	}

	return true; // If the target and vehicle are moving
}

void PCTTClusterHead::CheckAnswered() {
	NS_LOG_FUNCTION(this);

	if(!m_memberList.empty()) {

		for(vector<PCTTClusterHead::CM>::iterator it = m_memberList.begin(); it < m_memberList.end() ; it++) {
			if(!(*it).answered)
				it = m_memberList.erase(it);
		}

		for(vector<PCTTClusterHead::CM>::iterator it = m_memberList.begin(); it != m_memberList.end() ; it++) {
			(*it).answered = false;
		}

		SearchCHAndCCHInMemberList();
	}
}

/*****************  PCTTClusterHead END  **********************/

/*****************  PCTTClusterMember  **********************/
NS_OBJECT_ENSURE_REGISTERED(PCTTClusterMember);

TypeId PCTTClusterMember::GetTypeId () {
	static TypeId typeId = TypeId("ns3::PCTTClusterMember")
							.SetParent<PCTTVehicle> ()
							.AddConstructor<PCTTClusterMember>()
							.AddAttribute("LastPosition",
										  "Define the last position of the Node", VectorValue(),
										  MakeVectorAccessor(&PCTTClusterMember::m_lastPosition),
										  MakeVectorChecker())
							.AddAttribute("LastVelocity",
										  "Define the last velocity of the Node", VectorValue(),
										  MakeVectorAccessor(&PCTTClusterMember::m_lastVelocity),
										  MakeVectorChecker());
	return typeId;
}

PCTTClusterMember::PCTTClusterMember() {/* Empty */}

PCTTClusterMember::~PCTTClusterMember () {
	NS_LOG_DEBUG("Class ClusterMember: ~ClusterMember Method");
}

Vector PCTTClusterMember::GetLastPosition() {
	return m_lastPosition;
}

void PCTTClusterMember::SetLastPosition (Vector pos) {
	m_lastPosition = pos;
}

Vector PCTTClusterMember::GetLastVelocity() {
	return m_lastVelocity;
}

void PCTTClusterMember::SetLastVelocity(Vector lastVelocity) {
	m_lastVelocity = lastVelocity;
}

/*****************  PCTTClusterMember END  **********************/

/*****************  PCTTNonMember  **********************/
NS_OBJECT_ENSURE_REGISTERED(PCTTNonMember);

TypeId PCTTNonMember::GetTypeId() {
	static TypeId typeId = TypeId("ns3::PCTTNonMember")
									.SetParent<PCTTVehicle>()
									.AddConstructor<PCTTNonMember>();
	return typeId;
}

PCTTNonMember::PCTTNonMember() {/* Empty */}

PCTTNonMember::~PCTTNonMember() {
	NS_LOG_DEBUG("Class NonMember: ~NonMember Method");
}

Ptr<PCTTClusterMember> PCTTNonMember::ToClusterMember(int32_t chid) {
	return CreateObjectWithAttributes<PCTTClusterMember>("Id", IntegerValue(GetId()), "CHID", IntegerValue(chid),
			"TargetPosition", VectorValue(m_targetPosition), "TargetVelocity", VectorValue(m_targetVelocity), "MobilityModel", PointerValue(m_mobilityModel),
			"LastPosition", VectorValue(m_mobilityModel->GetPosition()), "LastVelocity", VectorValue(m_mobilityModel->GetVelocity()));
}

/*****************  PCTTNomMember END  **********************/

}
