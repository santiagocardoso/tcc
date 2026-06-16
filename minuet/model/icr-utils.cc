#include "icr-utils.h"

#include <cmath>
#include <ctime>
#include <chrono>

#include "ns3/simulator.h"
#include "ns3/vector.h"

#include <fstream>
#include <string>
#include <vector>
#include <map>

void LogAllScores(const std::string& methodName, 
                  const std::vector<int>& ids, 
                  const std::vector<std::vector<double>>& criteriaMatrix, 
                  const std::vector<double>& finalScores, 
                  uint64_t tempo) 
{
    const std::string folderPath = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/";
    const std::string filePath = folderPath + "score_history_" + methodName + ".csv";

    std::ofstream logFile(filePath, std::ios::app);
    
    logFile.seekp(0, std::ios::end);
    if (logFile.tellp() == 0) {
        logFile << "ns,ID,0,C,D,N,V,A,E,I,T,M,ICR\n";
    }
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i >= criteriaMatrix.size() || i >= finalScores.size()) continue;
        logFile << tempo << "," << ids[i];
        for (const double& criterionValue : criteriaMatrix[i]) {
            logFile << "," << criterionValue;
        }
        logFile << "," << finalScores[i] << "\n";
    }

    logFile.close();
}

// Veículos leves: (Potências x Faixas etárias)
// Idades: 18-29, 30-39, 40-49, 50-59, 60-69, 70-79, 80+
float faixa_matrix_leves[10][7] = {
    {1.00, 1.00, 1.00, 1.00, 0.75, 0.75, 0.25}, // 60-100 cv
    {0.75, 1.00, 1.00, 1.00, 0.75, 0.75, 0.25}, // 101-120 cv
    {0.75, 1.00, 1.00, 1.00, 0.75, 0.50, 0.25}, // 121-140 cv
    {0.75, 1.00, 1.00, 1.00, 0.75, 0.50, 0.25}, // 141-150 cv
    {0.50, 1.00, 1.00, 1.00, 0.50, 0.50, 0.00}, // 151-170 cv
    {0.50, 0.75, 1.00, 1.00, 0.50, 0.50, 0.00}, // 171-180 cv
    {0.25, 0.75, 1.00, 1.00, 0.50, 0.50, 0.00}, // 181-190 cv
    {0.25, 0.75, 1.00, 0.75, 0.50, 0.25, 0.00}, // 191-200 cv
    {0.00, 0.75, 1.00, 0.75, 0.25, 0.00, 0.00}, // 201-225 cv
    {0.00, 0.50, 0.75, 0.75, 0.25, 0.00, 0.00}  // 226+ cv
};

// Veículos pesados (Potências x Faixas etárias)
//                                 (18-29   30-49  50-59  60-64  65-66  67-69  70+)
float faixa_matrix_pesados[2][7] = {{18    , 30   , 50   , 60   , 65   , 67   , 70},
                                    {0.75  , 1    , 1    , 0.75 , 0.5  , 0.25 , 0}};

void printMatrix(vector<vector<double>> &matrix) {
	for (const auto &row : matrix) {
		for (double val : row)
			cout << val << " ";
		cout << endl;
	}
}

bool createDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), 0777) != 0) {
            return false;
        }
    }
    return true;
}

double VectorDistance(const ns3::Vector &v1, const ns3::Vector &v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

double CalculateAgeVsPotencyScore(int pv, int im, int lp) {
    if (!lp) { // Veículos Leves
        int p_idx = 0;
        if (pv >= 226) p_idx = 9;
        else if (pv >= 201) p_idx = 8;
        else if (pv >= 191) p_idx = 7;
        else if (pv >= 181) p_idx = 6;
        else if (pv >= 171) p_idx = 5;
        else if (pv >= 151) p_idx = 4;
        else if (pv >= 141) p_idx = 3;
        else if (pv >= 121) p_idx = 2;
        else if (pv >= 101) p_idx = 1;
        else p_idx = 0;

        int a_idx = 0;
        if (im >= 80) a_idx = 6;
        else if (im >= 70) a_idx = 5;
        else if (im >= 60) a_idx = 4;
        else if (im >= 50) a_idx = 3;
        else if (im >= 40) a_idx = 2;
        else if (im >= 30) a_idx = 1;
        else a_idx = 0;

        return faixa_matrix_leves[p_idx][a_idx];
        
    } else { // Veículos Pesados
        for (int i = 0; i < 6; i++) {
            if (im >= faixa_matrix_pesados[0][i] && im < faixa_matrix_pesados[0][i + 1]) {
                return faixa_matrix_pesados[1][i];
            }
        }
        if (im >= faixa_matrix_pesados[0][6]) {
            return faixa_matrix_pesados[1][6];
        }
    }
    return 0.0;
}

double CalculateVehicleAgeScore(int manufactureYear) {
    int currentYear = 2026;

    int vehicleAge = std::max(0, currentYear - manufactureYear);

    if (vehicleAge <= 10) {return 1.00;}
    else if (vehicleAge <= 20) {return 0.75;} 
    else if (vehicleAge <= 40) {return 0.50;}
    else {return 0.25;}
}

double CalculateTimeTraveledScore(int tt) {
    if (tt > 0 && tt <= 2) {return 1.00;} 
    else if (tt <= 4) {return 0.75;} 
    else if (tt <= 8) {return 0.50;} 
    else {return 0.25;}
    return 0.0;
}

double CalculateTimeLicensedScore(int tc) {
    if (tc > 10) return 1.0;
    if (tc > 5) return 0.75;
    if (tc > 2) return 0.5;
    if (tc >= 1) return 0.25;
    return 0.0;
}

double CalculateFuelEfficiencyScore(int ec) {
    switch (ec) {
        case 0: return 1.0;
        case 1: return 0.8;
        case 2: return 0.7;
        case 3: return 0.4;
        default: return 0.2;
    }
}

double CalculateAverageSpeedScore(double magnitudeVelocity_kmh) {
    if (magnitudeVelocity_kmh >= 80.0) {return 1.00;}
    else if (magnitudeVelocity_kmh >= 60.0) {return 0.75;} 
    else if (magnitudeVelocity_kmh >= 40.0) {return 0.50;}
    else if (magnitudeVelocity_kmh > 0.0) {return 0.25;} 
    return 0.00;
}

double CalculateClusterVelocityScore(double vehicle_kmh, double cluster_avg_kmh) {
    if (cluster_avg_kmh <= 1.0) {return (vehicle_kmh <= 1.0) ? 1.0 : 0.0;}

    double ratio = vehicle_kmh / cluster_avg_kmh;

    if (ratio >= 0.8 && ratio < 1.5) {return 1.00;}
    else if ((ratio >= 0.6 && ratio < 0.8) || (ratio >= 1.5 && ratio < 2.0)) {return 0.75;}
    else if ((ratio >= 0.4 && ratio < 0.6) || (ratio >= 2.0 && ratio < 2.5)) {return 0.50;}
    else if ((ratio > 0.0 && ratio < 0.4) || (ratio >= 2.5 && ratio < 3.0)) {return 0.25;}
    else {return 0.00;}
}

double CalculateVehicleTypeScore(int vt) {
    switch (vt) {
        case 0:
            return 1.00;
        case 1:
            return 0.75;
        case 2:
            return 0.50;
        case 3:
            return 0.25;
        default:
            return 0.25;
    }
}
