#ifndef MCDA_UTILS_H
#define MCDA_UTILS_H

#include <vector>
#include <limits>
#include <cmath>
#include <map>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sys/stat.h>
#include "ns3/node.h"
#include "ns3/vector.h"

using namespace std;
using namespace ns3;

// TOPSIS
vector<vector<double>> normalize(vector<vector<double>> &matrix);
vector<vector<double>> multiplyWeights(vector<vector<double>> &matrix, vector<double> &weights);
vector<double> idealSolution(vector<vector<double>> &matrix);
vector<double> negativeIdealSolution(vector<vector<double>> &matrix);
vector<double> euclideanDistance(vector<vector<double>> &matrix, vector<double> &target);
vector<double> closenessCoefficients(vector<double> &euclideanDistanceIdeal, vector<double> &negativeEuclidianDistanceIdeal);
int bestAlternative(vector<double> coefficients);
void printMatrix(vector<vector<double>> &matrix);

// AHP
vector<vector<double>> getDefaultAHPJudgmentMatrix();
vector<vector<double>> normalizeAHPMatrix(vector<vector<double>> &judgmentMatrix);
vector<double> calculateAHPWeights(vector<vector<double>> &normalizedMatrix);
vector<double> scoreAHPAlternativesSimple(vector<vector<double>> &evaluationMatrix, vector<double> &weights);
vector<double> scoreAHPAlternatives(const vector<vector<double>> &evaluationMatrix, const vector<double> &criteriaWeights);

// PROMETHEE
double preferenceFunction(double diff, double p = 0.1); // limiar 0.3 aqui
vector<vector<double>> calculatePreferenceMatrix(const vector<vector<double>>& evaluationMatrix, const vector<double>& weights, double preferenceThreshold = 0.1); // limiar 0.3 aqui
vector<double> calculatePositiveFlows(const vector<vector<double>>& preferenceMatrix);
vector<double> calculateNegativeFlows(const vector<vector<double>>& preferenceMatrix);
vector<double> calculateNetFlows(const vector<double>& phiPlus, const vector<double>& phiMinus);
int bestAlternativePromethee(const vector<double>& netFlows);

// BORDA
void UpdateBordaScores(const std::vector<double>& scores, const std::vector<int>& ids, int n, std::map<int, int>& bordaScores);

#endif // MCDA_UTILS_H