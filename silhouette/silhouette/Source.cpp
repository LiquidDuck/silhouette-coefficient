#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

double evklidDistance(vector<double> object1, vector<double> object2)
{
	double sum = 0;
	int countOfDimensions = object1.size();

	for (int d = 0; d < countOfDimensions; d++)
	{
		sum += pow(object1[d] - object2[d], 2);
	}

	return sqrt(sum);
}

vector<vector<vector<double>>> caclulateLocalDistances(vector<vector<vector<double>>> clusters)
{
	vector<vector<vector<double>>> localDistances;

	for (int c = 0; c < clusters.size(); c++)
	{
		vector<vector<double>> clusterDistances;
		vector<double> zeroDistances(clusters[c].size());

		for (int o = 0; o < clusters[c].size(); o++)
		{
			clusterDistances.push_back(zeroDistances);
		}

		for (int o = 0; o < clusters[c].size(); o++)
		{
			for (int d = o+1; d < clusters[c].size(); d++) 
			{
				clusterDistances[o][d] = evklidDistance(clusters[c][o], clusters[c][d]);
				clusterDistances[d][o] = clusterDistances[o][d];
			}
		}

		localDistances.push_back(clusterDistances);
	}

	return localDistances;
}

void readObjects(vector<vector<double>>& objects)
{
	ifstream infile("C:\\Users\\Anna\\Desktop\\Study\\Clustering\\input(1).csv");
	
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
	/*ifstream infile("C:\\Users\\Anna\\Desktop\\Study\\Clustering\\output.csv");*/
	int countOfClusters = 3;
	clusteringResults.push_back(0);
	clusteringResults.push_back(0);
	clusteringResults.push_back(0);
	clusteringResults.push_back(0);
	clusteringResults.push_back(2);
	clusteringResults.push_back(1);
	clusteringResults.push_back(1);
	clusteringResults.push_back(1);
	clusteringResults.push_back(1);
	clusteringResults.push_back(2);
	clusteringResults.push_back(2);
	clusteringResults.push_back(2);
	clusteringResults.push_back(2);

	return countOfClusters;
}

void calculateCentroids(vector<vector<double>> objects, vector<vector<vector<double>>>& clusters, vector<vector<double>>& centroids)
{
	vector<int> clusteringResults;
	int countOfClusters = readClustingResults(clusteringResults);
	int countOfDimensions = objects[0].size();
	
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

		centroids.push_back(centroid);
		clusters.push_back(cluster);
	}
}

vector<int> calculateNearestClusters(vector<vector<double>> centroids)
{
	int countOfClusters = centroids.size();
	int countOfDimensions = centroids[0].size();
	vector<int> nearestClusters(countOfClusters);

	for (int c = 0; c < countOfClusters; c++)
	{
		int minNum = -1;
		double minDis = -1;

		for (int n = 0; n < countOfClusters; n++)
		{
			if (c != n)
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

vector<vector<vector<double>>> calculateInterclusterDistances(vector<vector<vector<double>>> clusters, vector<vector<double>> centroids)
{
	int countOfClusters = clusters.size();
	vector<vector<vector<double>>> interclusterDistances;
	vector<int> nearestClusters = calculateNearestClusters(centroids);

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

double calculateSilhouette(vector<vector<vector<double>>> clusters, vector<vector<double>> centroids)
{
	int countOfClusters = clusters.size();
	vector<double> localAvg, interclusterAvg;
	vector<vector<vector<double>>> localDistances = caclulateLocalDistances(clusters);
	vector<vector<vector<double>>> interclusterDistances = calculateInterclusterDistances(clusters, centroids);
	double S = 0;

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
	
	for (int o = 0; o < localAvg.size(); o++)
	{
		double max = localAvg[o] > interclusterAvg[o] ? localAvg[o] : interclusterAvg[o];
		S += (interclusterAvg[o] - localAvg[o]) / max;
	}

	return S / localAvg.size();
}

void main()
{
	vector<vector<double>> objects;
	vector<vector<vector<double>>> clusters;
	vector<vector<double>> centroids;

	readObjects(objects);
	calculateCentroids(objects, clusters, centroids);
	cout << calculateSilhouette(clusters, centroids) << endl;

	system("pause");
}