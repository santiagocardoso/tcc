#ifndef ICR_UTILS_H
#define ICR_UTILS_H

#include "ns3/vector.h"

#include <vector>
#include <map>
#include <string>
#include <sys/stat.h>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iostream>

using namespace std;

void printMatrix(vector<vector<double>> &matrix);
bool createDirectory(const std::string& path);
double VectorDistance(const ns3::Vector &v1, const ns3::Vector &v2);
double mapeamentoRodoviaRua(double x, double y);

void LogAllScores(const std::string& methodName, 
                  const std::vector<int>& ids, 
                  const std::vector<std::vector<double>>& criteriaMatrix, 
                  const std::vector<double>& finalScores, 
                  uint64_t tempo);

// Funções de avaliação de atributos veiculares
double CalculateAverageSpeedScore(double magnitudeVelocity_kmh);
double CalculateAgeVsPotencyScore(int pv, int im, int lp);
double CalculateVehicleAgeScore(int manufactureYear);
double CalculateTimeLicensedScore(int tc);
double CalculateFuelEfficiencyScore(int ec);
double CalculateTimeTraveledScore(int tt);
double CalculateVehicleTypeScore(int vt);
double CalculateClusterVelocityScore(double vehicle_kmh, double cluster_avg_kmh);

// Matrizes globais
extern float faixa_matrix_leves[10][7];
extern float faixa_matrix_pesados[2][7];

#endif // ICR_UTILS_H
