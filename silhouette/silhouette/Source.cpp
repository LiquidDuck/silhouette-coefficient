#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

class SilhouetteCoefficient
{

public:
	SilhouetteCoefficient(int _countOfClusters, vector<int> clusteringResults, vector<vector<double>> objects)
	{
		countOfClusters = _countOfClusters;
		countOfObjects = objects.size();
		countOfDimensions = objects[0].size();

		for (int c = 0; c < countOfClusters; c++)
		{
			vector<vector<double>> cluster;
			vector<double> centroid(countOfDimensions);

			for (int o = 0; o < objects.size(); o++)
			{
				if (clusteringResults[o] == c)
				{
					cluster.push_back(objects[o]);

					for (int d = 0; d < countOfDimensions; d++)
					{
						centroid[d] += objects[o][d];
					}
				}
			}

			for (int d = 0; d < countOfDimensions; d++)
			{
				centroid[d] /= cluster.size();
			}

			clusters.push_back(cluster);
			centroids.push_back(centroid);
		}
	}
	~SilhouetteCoefficient()
	{

	}

	double calculateSilhouette()
	{
		double S = 0;
		vector<double> localAvg, interclusterAvg;
		vector<vector<vector<double>>> localDistances = caclulateLocalDistances();
		vector<vector<vector<double>>> interclusterDistances = calculateInterclusterDistances();

		for (int c = 0; c < countOfClusters; c++)
		{
			for (int o1 = 0; o1 < clusters[c].size(); o1++)
			{
				double lAvg = 0, iAvg = 0;

				for (int o2 = 0; o2 < clusters[c].size(); o2++)
				{
					lAvg += localDistances[c][o1][o2];
				}

				for (int o2 = 0; o2 < interclusterDistances[c][o1].size(); o2++)
				{
					iAvg += interclusterDistances[c][o1][o2];
				}

				localAvg.push_back(lAvg / (clusters[c].size() - 1));
				interclusterAvg.push_back(iAvg / (interclusterDistances[c][o1].size()));
			}
		}

		for (int o = 0; o < countOfObjects; o++)
		{
			double max = localAvg[o] > interclusterAvg[o] ? localAvg[o] : interclusterAvg[o];
			S += (interclusterAvg[o] - localAvg[o]) / max;
		}

		return S / countOfObjects;
	}

private:
	int countOfClusters;
	int countOfObjects;
	int countOfDimensions;
	vector<vector<double>> centroids;
	vector<vector<vector<double>>> clusters;
	
	double evklidDistance(vector<double> object1, vector<double> object2)
	{
		double sum = 0;

		for (int d = 0; d < countOfDimensions; d++)
		{
			sum += pow(object1[d] - object2[d], 2);
		}

		return sqrt(sum);
	}
	vector<int> calculateNearestClusters()
	{
		vector<int> nearestClusters(countOfClusters);

		for (int c = 0; c < countOfClusters; c++)
		{
			int minNum = -1;
			double minDis = -1;

			for (int n = 0; n < countOfClusters; n++)
			{
				if (c != n && clusters[n].size() > 0)
				{
					double dis = evklidDistance(centroids[c], centroids[n]);

					if (dis < minDis || minNum < 0)
					{
						minDis = dis;
						minNum = n;
					}
				}
			}

			nearestClusters[c] = minNum;
		}

		return nearestClusters;
	}
	vector<vector<vector<double>>> caclulateLocalDistances()
	{
		vector<vector<vector<double>>> localDistances;

		for (int c = 0; c < countOfClusters; c++)
		{
			vector<vector<double>> clusterDistances;
			vector<double> zeroDistances(clusters[c].size());

			for (int o = 0; o < clusters[c].size(); o++)
			{
				clusterDistances.push_back(zeroDistances);
			}

			for (int o = 0; o < clusters[c].size(); o++)
			{
				for (int d = o + 1; d < clusters[c].size(); d++)
				{
					clusterDistances[o][d] = evklidDistance(clusters[c][o], clusters[c][d]);
					clusterDistances[d][o] = clusterDistances[o][d];
				}
			}

			localDistances.push_back(clusterDistances);
		}

		return localDistances;
	}
	vector<vector<vector<double>>> calculateInterclusterDistances()
	{
		vector<vector<vector<double>>> interclusterDistances;
		vector<int> nearestClusters = calculateNearestClusters();

		for (int c = 0; c < countOfClusters; c++)
		{
			int near = nearestClusters[c];
			vector<vector<double>> clusterDistances;

			for (int o1 = 0; o1 < clusters[c].size(); o1++)
			{
				vector<double> objectDistances;

				for (int o2 = 0; o2 < clusters[near].size(); o2++)
				{
					objectDistances.push_back(evklidDistance(clusters[c][o1], clusters[near][o2]));
				}

				clusterDistances.push_back(objectDistances);
			}

			interclusterDistances.push_back(clusterDistances);
		}

		return interclusterDistances;
	}
};

class Executor
{
public:
	Executor(int argc, char *argv[])
	{
		for (int i = 1; i < argc; i+=2)
		{
			if (argv[i] == "-i")
			{
				inputPath = argv[i + 1];
			}
			else
				if(argv[i] == "-r")
				{
					clusteringResultsPath = argv[i + 1];
				}
				else
					if (argv[i] == "-o")
					{
						outputPath = argv[i + 1];
					}
		}
	}
	~Executor()
	{

	}

	void StartTesting()
	{
		vector<vector<double>> objects;
		vector<int> clusteringResults;

		readObjects(objects);
		int countOfClusters = readClustingResults(clusteringResults);

		SilhouetteCoefficient *silhouette = new SilhouetteCoefficient(countOfClusters, clusteringResults, objects);
		writeResult(silhouette->calculateSilhouette());
	}

private:
	string inputPath;
	string clusteringResultsPath;
	string outputPath;

	void readObjects(vector<vector<double>>& objects)
	{
		ifstream infile(inputPath);

		while (infile)
		{
			string s;
			if (!getline(infile, s)) break;

			istringstream ss(s);
			vector <double> record;

			while (ss)
			{
				string s;
				double p;
				if (!getline(ss, s, ';'))
				{
					break;
				}

				istringstream iss(s);
				iss >> p;
				record.push_back(p);
			}

			objects.push_back(record);
		}
	}
	int readClustingResults(vector<int>& clusteringResults)
	{
		ifstream infile(clusteringResultsPath);

		int countOfClusters = 0;
		string s;
		double d;

		getline(infile, s);
		istringstream ss(s);
		ss >> d;

		if (ss)
		{
			double maxRes = d;
			int num = 0, maxNum = 0;
			countOfClusters++;

			while (ss)
			{
				countOfClusters++;
				num++;
				ss >> d;
				if (d > maxRes)
				{
					maxRes = d;
					maxNum = num;
				}
			}
			clusteringResults.push_back(maxNum);

			while (infile)
			{
				getline(infile, s);
				istringstream iss(s);

				maxRes = -1;
				num = -1;
				maxNum = 0;

				while (iss)
				{
					iss >> d;
					num++;
					if (d > maxRes)
					{
						maxRes = d;
						maxNum = num;
					}
				}

				clusteringResults.push_back(maxNum);
			}
		}
		else
		{
			clusteringResults.push_back(d - 1);

			while (infile)
			{
				getline(infile, s);
				int n;
				istringstream iss(s);
				iss >> n;
				clusteringResults.push_back(n - 1);

				if (n > countOfClusters)
				{
					n = countOfClusters;
				}
			}
		}

		return countOfClusters;
	}
	void writeResult(double S)
	{
		cout << S << endl;
	}
};

void main(int argc, char *argv[])
{
	Executor *executor = new Executor(argc, argv);
	executor->StartTesting();
	
	system("pause");
}