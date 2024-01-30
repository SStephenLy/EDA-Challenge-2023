#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <string>
#include "eda.h"
// Structure for storage units
struct StorageUnit {
    std::string name;
    std::pair<double, double> position;
    double power;
    
};

std::vector<std::vector<StorageUnit>> convert_storageUnit(const std::vector<std::vector<std::string>>& initMemPart){
    std::vector<std::vector<StorageUnit>> result;
    for(int i=0; i<initMemPart.size(); ++i){
        std::vector<StorageUnit> temp_group;
        for(int j=0; j<initMemPart[i].size(); ++j){
            StorageUnit temp_mem;
            temp_mem.name = initMemPart[i][j];
            temp_mem.position = subMap[initMemPart[i][j]].position;
            temp_mem.power = memoryMap[subMap[initMemPart[i][j]].type].getPower();
            temp_group.push_back(temp_mem);
        }
        result.push_back(temp_group);
    }
    return result;
}

// K-means clustering class
class KMeans {
private:
    int k; // Number of clusters
    std::vector<std::pair<double, double>> centroids; // Cluster centers
    std::vector<int> clusterAssignments; // Cluster assignments for storage units

public:
    //KMeans(int k) : k(k) {}
    KMeans(){
    	this->k = 0;
    }

    // Initialize cluster centers
    void initializeCentroids(const std::vector<std::pair<double, double>>& data) {
        centroids.clear();
        std::srand(std::time(0));
        std::vector<int> indices(data.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_shuffle(indices.begin(), indices.end());

        for (int i = 0; i < k; ++i) {
            centroids.push_back(data[indices[i]]);
        }
    }

    // Calculate Euclidean distance
    double calculateDistance(const std::pair<double, double>& a, const std::pair<double, double>& b) {
        return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
    }

    // Execute K-means clustering
    void fit(const std::vector<std::pair<double, double>>& data, int maxIterations = 100) {
        initializeCentroids(data);

        for (int iter = 0; iter < maxIterations; ++iter) {
            // Assign each storage unit to the nearest cluster
            clusterAssignments.clear();
            for (const auto& point : data) {
                int closestCentroid = 0;
                double minDistance = calculateDistance(point, centroids[0]);

                for (int i = 1; i < k; ++i) {
                    double distance = calculateDistance(point, centroids[i]);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestCentroid = i;
                    }
                }

                clusterAssignments.push_back(closestCentroid);
            }

            // Update cluster centers
            for (int i = 0; i < k; ++i) {
                double sumX = 0.0, sumY = 0.0;
                int count = 0;

                for (int j = 0; j < data.size(); ++j) {
                    if (clusterAssignments[j] == i) {
                        sumX += data[j].first;
                        sumY += data[j].second;
                        count++;
                    }
                }

                if (count > 0) {
                    centroids[i].first = sumX / count;
                    centroids[i].second = sumY / count;
                }
            }
        }
    }


    //The result of cluster is divided by kmeans,storaging to the two dimension vector.
    std::vector<std::vector<StorageUnit>> getKmeansResult(std::vector<StorageUnit> storageUnits, int k){
        
        // Extract storage unit positions
        std::vector<std::pair<double, double>> positions;
        for (const auto& unit : storageUnits) {
            positions.push_back(unit.position);
        }
        
        // Create K-means clustering object
        this->k = k;
        
        // Execute K-means clustering
        fit(positions);
        
        // Store storage units by cluster
        std::vector<std::vector<StorageUnit>> clusteredStorageUnits(this->k);
        for (int i = 0; i < storageUnits.size(); ++i) {
            clusteredStorageUnits[clusterAssignments[i]].push_back(storageUnits[i]);
        }
        
        return clusteredStorageUnits;
    }


    // Get the cluster assignments for storage units
    const std::vector<int>& getClusterAssignments() const {
        return clusterAssignments;
    }
    
    // Get the number of clusters
    const int getClusterNumber() const {
        return k;
    }
    // Check if distances within each cluster satisfy the given constraint  ---- power
    bool checkClusterDistances(std::vector<std::vector<StorageUnit>> clusteredStorageUnits, double distanceThreshold, double powerThreshold) {
        for(const auto& cluster : clusteredStorageUnits){

            double sumPower = 0.0;
            for(int i = 0; i < cluster.size(); i ++){
                for(int j = i+1; j < cluster.size(); j++){
                    if(calculateDistance(cluster[i].position, cluster[j].position) > distanceThreshold){
                        return false;
                    }
                }
                
                sumPower += cluster[i].power;
            }
            if(sumPower > powerThreshold){
                return false;
            }
          
        }
        
        return true;
    }
};

// // Generate random storage units
// std::vector<StorageUnit> generateRandomStorageUnits(int count) {
//     std::vector<StorageUnit> units;
//     for (int i = 0; i < count; ++i) {
//         StorageUnit unit;
//         unit.name = "StorageUnit" + std::to_string(i);
//         unit.position.first = static_cast<double>(rand()) / RAND_MAX * 100.0;  // Random x-coordinate between 0 and 100
//         unit.position.second = static_cast<double>(rand()) / RAND_MAX * 100.0; // Random y-coordinate between 0 and 100
//         unit.power = static_cast<double>(rand()) / RAND_MAX * 10.0;             // Random power between 0 and 10
//         units.push_back(unit);
//     }
//     return units;
// }

std::vector<std::vector<StorageUnit>> _KMeans(std::vector<StorageUnit> storageUnits, double distanceThreshold, double powerThreshold) {
  

    KMeans kmeans;
    
    bool flag;
    // Store storage units by cluster
    std::vector<std::vector<StorageUnit>> clusteredStorageUnits;
    std::vector<std::vector<StorageUnit>> pre_clusteredStorageUnits;
    int low = 1, high = storageUnits.size();
    while(low < high){
        int middle = (low + high)/2;
        
        clusteredStorageUnits = kmeans.getKmeansResult(storageUnits, middle);
        if (kmeans.checkClusterDistances(clusteredStorageUnits, distanceThreshold, powerThreshold)) {
            high = middle-1;
            flag = true;
            pre_clusteredStorageUnits = clusteredStorageUnits;
        } else {
            flag = false;
            low = middle+1;
        }
    }
    
    if(pre_clusteredStorageUnits.empty()){
    	std::vector<std::vector<StorageUnit>> cluster_Storage(1);
    	cluster_Storage[0] = storageUnits;
    	return cluster_Storage;
    }
    
    if(flag){
    	return clusteredStorageUnits;
    }else{
    	return pre_clusteredStorageUnits;
    }
    
    
    

}


void MBIST_of_KMeans(double distanceThreshold, double powerThreshold){
    std::vector<std::vector<StorageUnit>> init_storageUnits = convert_storageUnit(initMemPart);
    std::vector<std::vector<StorageUnit>> last_storageUnits;
    for(const auto& storageUnits : init_storageUnits){
       std::vector<std::vector<StorageUnit>> temp_storageUnits =  _KMeans(storageUnits, distanceThreshold, powerThreshold);
       for(const auto& it : temp_storageUnits){
           last_storageUnits.push_back(it);
       }
    }
    /*
    // Output storage units in each cluster
    std::cout << "Storage units grouped by cluster:" << std::endl;
    for (int i = 0; i < last_storageUnits.size(); ++i) {
        if (!last_storageUnits[i].empty()) {
            std::cout << "Cluster " << i + 1 << " contains storage units:" << std::endl;
            for (const auto& unit : last_storageUnits[i]) {
                std::cout << "Name: " << unit.name << ", Position: (" << unit.position.first << ", " << unit.position.second << "), Power: " << unit.power << std::endl;
            }
            std::cout << std::endl;
         }
    }
    */
    // Check if distances within each cluster satisfy the given constraint
    
    KMeans kmeans;
    
    if (kmeans.checkClusterDistances(last_storageUnits, distanceThreshold, powerThreshold)) {
        std::cout << "Each cluster satisfy the constraint." << std::endl;
    } else {
        std::cout << "The result of cluster do not satisfy the constraint." << std::endl;
    }
    
    std::cout<<"The number of cluster :"<<last_storageUnits.size()<<std::endl;
    std::cout<<"**************************************************************************"<<std::endl;
    
}

int main(int argc, char ** argv){
    
    if(argc != 4){
   	    std::cout<<"USAGE:  ./progrom filefolder powerlimit distancelimit"<<std::endl;
   	    return 0;
    }
    eda_parser(argv[1]);
    double powerThreshold = std::stod(argv[2]);   
    double distanceThreshold = std::stod(argv[3]);  
    
    if(!if_have_def){
    	std::cout<<std::endl<<"\033[1;31m"<<"[waring]"<<"\033[0m"<<"This case does not have .def file !!!"<<std::endl<<std::endl;
    	std::cout<<"**************************************************************************"<<std::endl;
    	return 0;
    }
    
    MBIST_of_KMeans(distanceThreshold, powerThreshold);
    
    return 0;
}