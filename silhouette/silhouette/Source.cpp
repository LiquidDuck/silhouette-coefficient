#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

//double calculateSilhouette()
//{
//	return 1;
//}

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
	ifstream infile("C:\\Users\\Anna\\Desktop\\Study\\Clustering\\output.csv");
	int countOfClusters;
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

		centroids[c] = centroid;
		clusters.push_back(cluster);
	}
}

void main()
{
	vector<vector<double>> objects;
	vector<vector<vector<double>>> clusters;
	vector<vector<double>> centroids;

	readObjects(objects);
	calculateCentroids(objects, clusters, centroids);

	system("pause");
}