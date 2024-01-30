#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "eda.h"

using namespace std;


bool distance(const vector<Memory>& Mem, const Memory& b, double maxDis) {
    double dis = 0;
    for(int i=0; i<Mem.size(); i++){
        double dx = Mem[i].x - b.x;
        double dy = Mem[i].y - b.y;
        dis = sqrt(dx * dx + dy * dy);
        if(dis > maxDis){
            return false;
        }
    }
    return true;
}

bool distance_block(const vector<Memory>& Mem, const Memory& b, int maxDis){
    int dis = 0;
    for(int i=0; i<Mem.size(); i++){
        int j=0;
        while(j<Mem[i].level.size() && j<b.level.size()){
            if(Mem[i].level[j] != b.level[j]){
                break;
            }
            else j++;
        }
        dis = max(Mem[i].level.size(), b.level.size()) - j-1;
        if(dis > maxDis){
            return false;
        }
    }
    return true;
}

double calculateTotalPower(const vector<Memory>& group) {
    double totalPower = 0.0;
    for (const Memory& unit : group) {
        totalPower += unit.power;
    }
    return totalPower;
}

void printMemorys(const vector<Memory>& Memorys) {
    double sumPower = 0;
    for (const Memory& unit : Memorys) {
        cout << "ID: " << unit.id << ", Power: " << unit.power
            << ", X: " << unit.x << ", Y: " << unit.y << ", Block: " << unit.level.size() << endl;
        sumPower += unit.power;
    }
    cout << "\nsumPower = " << sumPower << endl;
}

void printMemorys_file(const vector<vector<Memory>>& Memorys, string filename) {
    //double sumPower = 0;
    ofstream outPutFile(filename);
    for(int i=0 ; i<Memorys.size(); i++){
        //sumPower = 0;
        outPutFile << "Controller_" << i+1 << ":" << endl;
        for (const Memory& unit : Memorys[i]) {
            // outPutFile << "ID: " << unit.id << ", Power: " << unit.power
            //     << ", X: " << unit.x << ", Y: " << unit.y << ", Block: " << unit.level.size() << endl;
            //sumPower += unit.power;
            outPutFile << unit.id <<  endl;
        }
        outPutFile <<  endl;
        //outPutFile << "\nsumPower = " << sumPower << endl<<endl;
    }
}

//get new fit energy 
double getCurrentEnergy(vector<vector<Memory>>& solution, double powerLimit){
	double energy = 0.0;
	for(int i=0 ; i<solution.size(); i++){
		energy += solution[i].size()/(powerLimit - calculateTotalPower(solution[i]));
	}
	return solution.size() + energy/1000000;
}

void simulatedAnnealing(vector<vector<Memory>>& solution, double temperature, double coolingRate, double distanceLimit, double powerLimit) {
    srand(static_cast<unsigned>(time(nullptr)));
    
    double currentEnergy = 0.0;  // 当前解的能量（总功耗）
    
    currentEnergy = getCurrentEnergy(solution, powerLimit);
    
    double max_temp = temperature;

    while (temperature > 0.1) {  // 温度降至足够低
        // 随机选择一种操作：合并、拆分、移动存储器
        int operation;
        if(temperature > max_temp/2){
            int operation1 = rand() % 100;
            if(operation1 <= 50){
                operation = 0;
            }
            else if(operation1 <= 80){
                operation = 2;
            }
            else{
                operation = 2;
            }
            
        }
        else{
            int operation1 = rand() % 100;
            if(operation1 <= 40){
                operation = 2;
            }
            else if(operation1 <= 90){
                operation = 0;
            }
            else{
                operation = 2;
            }
        }

        if (operation == 0 && solution.size() >= 2) {
            // 合并两个组
            int groupIndex1 = rand() % solution.size();
            int groupIndex2 = rand() % solution.size();
            if (groupIndex1 != groupIndex2) {
                auto newSolution = solution;  // 复制当前解
                newSolution[groupIndex1].insert(
                    newSolution[groupIndex1].end(),
                    std::make_move_iterator(newSolution[groupIndex2].begin()),
                    std::make_move_iterator(newSolution[groupIndex2].end())
                );
                

                // 判断新解是否满足约束条件
                bool isValidSolution = true;
                if (calculateTotalPower(newSolution[groupIndex1]) > powerLimit) {
                    isValidSolution = false;
                }
                if(isValidSolution){
                    for (int i = 0; i < newSolution[groupIndex1].size(); ++i) {
                        for (int j = i + 1; j < newSolution[groupIndex1].size(); ++j) {
                            double dx = newSolution[groupIndex1][i].x - newSolution[groupIndex1][j].x;
                            double dy = newSolution[groupIndex1][i].y - newSolution[groupIndex1][j].y;
                            double dis = sqrt(dx * dx + dy * dy);
                            if (dis > distanceLimit) {
                                isValidSolution = false;
                                break;
                            }
                        }
                        if (!isValidSolution) {
                            break;
                        }
                    }
                }
                
                newSolution.erase(newSolution.begin() + groupIndex2); 
            

                // 计算新解的能量
                double newEnergy = 0.0;

                newEnergy = getCurrentEnergy(newSolution, powerLimit);
                
                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (energyDifference < 0 || exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    solution = (newSolution);
                    currentEnergy = newEnergy;
                }
            }
        } else if (operation == 1 && solution.size() >= 2) {
            // 拆分一个组
            int groupIndex = rand() % solution.size();
            if (solution[groupIndex].size() >= 2) {
                auto newSolution = solution;  // 复制当前解
                int splitIndex = rand() % (solution[groupIndex].size() - 1) + 1;
                vector<Memory> newGroup(
                    std::make_move_iterator(newSolution[groupIndex].begin() + splitIndex),
                    std::make_move_iterator(newSolution[groupIndex].end())
                );
                newSolution[groupIndex].erase(newSolution[groupIndex].begin() + splitIndex, newSolution[groupIndex].end());

                // 判断新解是否满足功耗约束
                bool isValidSolution = true;
                

                // 计算新解的能量
                double newEnergy = 0.0;
                
                newEnergy = getCurrentEnergy(newSolution, powerLimit);

                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    newSolution.push_back(newGroup);
                    solution = (newSolution);
                    currentEnergy = newEnergy;
                }
            }
        } else {
            // 移动一个存储器到另一个组
            int sourceGroupIndex = rand() % solution.size();
            int destinationGroupIndex = rand() % solution.size();
            if (sourceGroupIndex != destinationGroupIndex && solution[sourceGroupIndex].size() >= 1) {
                auto newSolution = solution;  // 复制当前解
                int memoryIndex = rand() % solution[sourceGroupIndex].size();
                Memory movedMemory = newSolution[sourceGroupIndex][memoryIndex];
                newSolution[destinationGroupIndex].push_back(movedMemory);
                

                // 判断新解是否满足功耗约束
                bool isValidSolution = true;
                if (calculateTotalPower(newSolution[destinationGroupIndex]) > powerLimit) {
                    isValidSolution = false;
                }

                // 判断新解是否满足距离约束
                if(isValidSolution){
                    for (const auto& existingMemory : newSolution[destinationGroupIndex]) {
                        double dx = movedMemory.x - existingMemory.x;
                        double dy = movedMemory.y - existingMemory.y;
                        double dis = sqrt(dx * dx + dy * dy);
                        if (dis > distanceLimit) {
                            isValidSolution = false;
                            break;
                        }
                    }
                }
                newSolution[sourceGroupIndex].erase(newSolution[sourceGroupIndex].begin() + memoryIndex);
                if(newSolution[sourceGroupIndex].empty()){
                    newSolution.erase(newSolution.begin() + sourceGroupIndex);
                }
                // 计算新解的能量
                double newEnergy = 0.0;
                
                newEnergy = getCurrentEnergy(newSolution, powerLimit);

                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (energyDifference < 0 || exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    solution = newSolution;
                    currentEnergy = newEnergy;
                }
            }
        }

        // 降低温度
        temperature *= 1 - coolingRate;
    }
}

bool comp_power(Memory a, Memory b){
    return a.power > b.power;
}
bool comp_size(vector<Memory> a, vector<Memory> b){
    return a.size() < b.size();
}

void divideGroup(vector<Memory>& Memorys, double distanceLimit, double powerLimit, vector<vector<Memory>>& result, int itr) {
    
    vector<vector<Memory>> result_1;

    vector<Memory> remaining = Memorys;

    while (!remaining.empty()) {
        // 随机选择一个存储器作为起始点
        int startIndex = rand() % remaining.size();
        Memory startUnit = remaining[startIndex];

        // 构建一个组，并将起始点放入其中
        vector<Memory> currentGroup;
        currentGroup.push_back(startUnit);

        // 移除起始点
        remaining.erase(remaining.begin() + startIndex);

        // 找到与起始点距离在限制内的其他存储器，加入当前组
        auto it = remaining.begin();
        while (it != remaining.end()) {
            if (distance(currentGroup, *it, distanceLimit)  && calculateTotalPower(currentGroup) + it->power <= powerLimit) {
                currentGroup.push_back(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        // 将当前组加入结果
        result_1.push_back(currentGroup);
    }

    vector<vector<Memory>> result_temp = result_1;
    for(int i=0; i<itr; i++){
        simulatedAnnealing(result_1, 100.0, 0.001, distanceLimit, powerLimit);
        if(result_1.size() <= result_temp.size()){
            result_temp = result_1;
        }
        else{
            result_1 = result_temp;
        }
    }
    
    if(result_1.size() > 1){
        sort(result_1.begin(), result_1.end(), comp_size);
        for(int i=0; i<result_1.size(); i++){
            sort(result_1[i].begin(), result_1[i].end(), comp_power);
            for(int j=0; j<result_1[i].size(); j++){
                for(int k=result_1.size()-1; k>i; k--){
                    if(distance(result_1[k], result_1[i][j], distanceLimit) && (calculateTotalPower(result_1[k])+result_1[i][j].power <powerLimit)){
                        result_1[k].push_back(result_1[i][j]);
                        result_1[i].erase(result_1[i].begin()+j);
                        j--;
                        break;
                    }
                }
            }
            if(result_1[i].empty()){
                result_1.erase(result_1.begin()+i);
                i--;
            }
        }
    }
    

    for(int i=0; i<result_1.size(); i++){
        result.push_back(result_1[i]);
    }
}




///Block
void simulatedAnnealing_block(vector<vector<Memory>>& solution, double temperature, double coolingRate, int distanceLimit, double powerLimit) {
    srand(static_cast<unsigned>(time(nullptr)));

    int currentEnergy = 0.0;  // 当前解的能量（总功耗）
    //
    currentEnergy = getCurrentEnergy(solution, powerLimit);
    

    double max_temp = temperature;

    while (temperature > 0.1) {  // 温度降至足够低
        // 随机选择一种操作：合并、拆分、移动存储器
        int operation;
        if(temperature > max_temp/3){
            int operation1 = rand() % 100;
            if(operation1 <= 60){
                operation = 0;
            }
            else if(operation1 <= 80){
                operation = 2;
            }
            else{
                operation = 2;
            }
            
        }
        else{
            int operation1 = rand() % 100;
            if(operation1 <= 50){
                operation = 2;
            }
            else if(operation1 <= 90){
                operation = 0;
            }
            else{
                operation = 2;
            }
        }
        


        if (operation == 0 && solution.size() >= 2) {
            // 合并两个组
            int groupIndex1 = rand() % solution.size();
            int groupIndex2 = rand() % solution.size();
            if (groupIndex1 != groupIndex2) {
                auto newSolution = solution;  // 复制当前解
                newSolution[groupIndex1].insert(
                    newSolution[groupIndex1].end(),
                    std::make_move_iterator(newSolution[groupIndex2].begin()),
                    std::make_move_iterator(newSolution[groupIndex2].end())
                );
                

                // 判断新解是否满足约束条件
                bool isValidSolution = true;
                if (calculateTotalPower(newSolution[groupIndex1]) > powerLimit) {
                    isValidSolution = false;
                }
                if(isValidSolution){
                    for (int i = 0; i < newSolution[groupIndex1].size(); ++i) {
                        for (int j = i + 1; j < newSolution[groupIndex1].size(); ++j) {
                            int k=0, dis;
                            while(k<newSolution[groupIndex1][i].level.size() && k<newSolution[groupIndex1][j].level.size()){
                                if(newSolution[groupIndex1][i].level[k] != newSolution[groupIndex1][j].level[k]){
                                    break;
                                }
                                else k++;
                            }
                            dis = max(newSolution[groupIndex1][i].level.size(), newSolution[groupIndex1][j].level.size()) - k -1;
                            if (dis > distanceLimit) {
                                isValidSolution = false;
                                break;
                            }
                        }
                        if (!isValidSolution) {
                            break;
                        }
                    }
                }
                newSolution.erase(newSolution.begin() + groupIndex2);
                // 计算新解的能量
                double newEnergy = 0.0;

                newEnergy = getCurrentEnergy(newSolution, powerLimit);
                

                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (energyDifference < 0 || exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    solution = std::move(newSolution);
                    currentEnergy = newEnergy;
                }
            }
        } else if (operation == 1 && solution.size() >= 2) {
            // 拆分一个组
            int groupIndex = rand() % solution.size();
            if (solution[groupIndex].size() >= 2) {
                auto newSolution = solution;  // 复制当前解
                int splitIndex = rand() % (solution[groupIndex].size() - 1) + 1;
                vector<Memory> newGroup(
                    std::make_move_iterator(newSolution[groupIndex].begin() + splitIndex),
                    std::make_move_iterator(newSolution[groupIndex].end())
                );
                newSolution[groupIndex].erase(newSolution[groupIndex].begin() + splitIndex, newSolution[groupIndex].end());

                // 判断新解是否满足功耗约束
                bool isValidSolution = true;
               
             

                // 计算新解的能量
                double newEnergy = 0.0;
           
                newEnergy = getCurrentEnergy(newSolution, powerLimit);

                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    newSolution.push_back(std::move(newGroup));
                    solution = std::move(newSolution);
                    currentEnergy = newEnergy;
                }
            }
        } else {
            // 移动一个存储器到另一个组
            int sourceGroupIndex = rand() % solution.size();
            int destinationGroupIndex = rand() % solution.size();
            if (sourceGroupIndex != destinationGroupIndex && solution[sourceGroupIndex].size() >= 1) {
                auto newSolution = solution;  // 复制当前解
                int memoryIndex = rand() % solution[sourceGroupIndex].size();
                Memory movedMemory = newSolution[sourceGroupIndex][memoryIndex];
                newSolution[destinationGroupIndex].push_back(movedMemory);
                

                // 判断新解是否满足功耗约束
                bool isValidSolution = true;
                if (calculateTotalPower(newSolution[destinationGroupIndex]) > powerLimit) {
                    isValidSolution = false;
                }

                // 判断新解是否满足距离约束
                if(isValidSolution){
                    //isValidSolution = distance_block(newSolution[destinationGroupIndex], movedMemory, distanceLimit);
                    for(int i=0; i<newSolution[destinationGroupIndex].size(); i++){
                        int dis, k=0;
                        while(k<newSolution[destinationGroupIndex][i].level.size() && k<movedMemory.level.size()){
                            if(newSolution[destinationGroupIndex][i].level[k] != movedMemory.level[k]){
                                break;
                            }  
                            else k++;
                        }       
                        dis = max(newSolution[destinationGroupIndex][i].level.size(), movedMemory.level.size()) - k -1;
                        if (dis > distanceLimit) {
                            isValidSolution = false;
                            break;
                        }
                    } 

                }
                newSolution[sourceGroupIndex].erase(newSolution[sourceGroupIndex].begin() + memoryIndex);
                if(newSolution[sourceGroupIndex].empty()){
                    newSolution.erase(newSolution.begin() + sourceGroupIndex);
                }
                // 计算新解的能量
                double newEnergy = 0.0;
           
                newEnergy = getCurrentEnergy(newSolution, powerLimit);

                // 计算能量差异
                double energyDifference = newEnergy - currentEnergy;

                // 判断是否接受新解
                if (isValidSolution && (energyDifference < 0 || exp(-energyDifference / temperature) > (rand() % 1000) / 1000.0)) {
                    // 接受新解
                    solution = std::move(newSolution);
                    currentEnergy = newEnergy;
                }
            }
        }

        // 降低温度
        temperature *= 1 - coolingRate;
    }
}






void divideGroup_block(vector<Memory>& Memorys, int distanceLimit, double powerLimit, vector<vector<Memory>>& result, int itr) {
    
    vector<vector<Memory>> result_1;


    vector<Memory> remaining = Memorys;

    while (!remaining.empty()) {
        // 随机选择一个存储器作为起始点
        int startIndex = rand() % remaining.size();
        Memory startUnit = remaining[startIndex];

        // 构建一个组，并将起始点放入其中
        vector<Memory> currentGroup;
        currentGroup.push_back(startUnit);

        // 移除起始点
        remaining.erase(remaining.begin() + startIndex);

        // 找到与起始点距离在限制内的其他存储器，加入当前组
        auto it = remaining.begin();
        while (it != remaining.end()) {
            if (distance_block(currentGroup, *it, distanceLimit)  && calculateTotalPower(currentGroup) + it->power <= powerLimit) {
                currentGroup.push_back(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        // 将当前组加入结果
        result_1.push_back(currentGroup);
    }

    vector<vector<Memory>> result_temp = result_1;
    for(int i=0; i<itr; i++){
        simulatedAnnealing_block(result_1, 100.0, 0.001, distanceLimit, powerLimit);
        if(result_1.size() <= result_temp.size()){
            result_temp = result_1;
        }
        else{
            result_1 = result_temp;
        }
    }
    
    if(result_1.size() > 1){
        sort(result_1.begin(), result_1.end(), comp_size);
        for(int i=0; i<result_1.size(); i++){
            sort(result_1[i].begin(), result_1[i].end(), comp_power);
            for(int j=0; j<result_1[i].size(); j++){
                for(int k=result_1.size()-1; k>i; k--){
                    if(distance_block(result_1[k], result_1[i][j], distanceLimit) && (calculateTotalPower(result_1[k])+result_1[i][j].power <powerLimit)){
                        result_1[k].push_back(result_1[i][j]);
                        result_1[i].erase(result_1[i].begin()+j);
                        j--;
                        break;
                    }
                }
            }
            if(result_1[i].empty()){
                result_1.erase(result_1.begin()+i);
                i--;
            }
        }
    }
    

    for(int i=0; i<result_1.size(); i++){
        result.push_back(result_1[i]);
    }
}

int main(int argc, char ** argv) {
	
    if(argc != 5){
   	    cout<<"USAGE:  ./progrom filefolder -str powerlimit distancelimit"<<endl;
   	    return 0;
    }
    eda_parser(argv[1]);

    
    string file_name_case = argv[1]; 
    size_t lastSlashPos = file_name_case.find_last_of('/');
    file_name_case = file_name_case.substr(lastSlashPos + 1);
    size_t startPos = file_name_case.find_first_of("0123456789");
    file_name_case = file_name_case.substr(startPos);
    string power_1 = argv[3];     // 功耗上限
    string dis_1 = argv[4];  // 距离上限
    
    vector<vector<Memory>> init_group = tran_to_Mem(initMemPart);
    // 对存储器进行再分组
    double distanceLimit = 0;  // 距离上限
    double powerLimit = 0;     // 功耗上限
    int block_dis = 0;           // MAX BLOCK
    string choice = argv[2];   
    if(choice=="-dis"){
        if(!if_have_def){
            cout<< ".def File Not Found!"<<endl;
            exit(0);
        }
    }
    vector<vector<Memory>> result;
    
    powerLimit = stod(power_1); 

    int itr = 1;   //最大迭代次数
    int total_mem = 0;
    for(int i=0; i<init_group.size(); i++){
        total_mem += init_group[i].size();
    }
    if(total_mem<2000){
        itr = 5;
    }
    else if (total_mem < 5000){
        itr = 3;
    }
    else{
        itr = 1;
    }
    

    

    if(if_have_def && choice=="-dis"){
        distanceLimit = stod(dis_1);
        for(int i=0; i<init_group.size(); i++){
        
            divideGroup(init_group[i], distanceLimit, powerLimit, result, itr);
        
        }
        //每组最大距离
        vector<double> max_distance(result.size()+1);
        for(int i=0; i<result.size(); i++){
            double max_dis = 0;
            for(int j=0; j<result[i].size(); j++){
                for(int k=j+1; k<result[i].size(); k++){
                    double dx = result[i][j].x - result[i][k].x;
                    double dy = result[i][j].y - result[i][k].y;
                    double dis = sqrt(dx * dx + dy * dy);
                    max_dis = max(max_dis, dis);
                }
            }
            max_distance[i] = max_dis;
        }
        // for(int j=0; j<result.size(); j++){
        //     cout<<"The " << j+1 << " group" << endl;
        //     printMemorys(result[j]);
        //     // cout<<endl;
        //     cout<< "max distance: " << max_distance[j] <<endl;
            
        //     cout<<"\n\n"<<endl;
        // }
        printMemorys_file(result, "./output/tc"+file_name_case+"_memlist.group");
        cout<<endl<<"**********************************OUTPUT**********************************"<<endl;
        cout<<"*"<<endl;
        cout<< "* Total number of groups: " << result.size() <<endl;
        cout<< "* The MBIST Partition Data has been output to " << "\"tc"+file_name_case+"_memlist.group\"" <<endl;
        cout<<"*"<<endl;
        cout<<"**************************************************************************"<<endl;
        
        
  
    }
    
    if(choice=="-block"){
        block_dis = stoi(dis_1);
        for(int i=0; i<init_group.size(); i++){
        
            divideGroup_block(init_group[i], block_dis, powerLimit, result, itr);
        
        }

        //每组最大距离_____BLOCK
        vector<int> max_distance(result.size()+1);
        for(int i=0; i<result.size(); i++){
            int max_dis = 0;
            for(int j=0; j<result[i].size(); j++){
                for(int k=j+1; k<result[i].size(); k++){
                    int m=0, dis;
                    while(m<result[i][j].level.size() && m<result[i][k].level.size()){
                        if(result[i][j].level[m] != result[i][k].level[m]){
                            break;
                        }
                        else m++;
                    }
                    dis = max(result[i][j].level.size(), result[i][k].level.size()) - m -1;
                    max_dis = max(max_dis, dis);
                }
            }
            max_distance[i] = max_dis;
        }
        // for(int j=0; j<result.size(); j++){
        //     cout<<"The " << j+1 << " group" << endl;
        //     printMemorys(result[j]);
        //     cout<< "max distance: " << max_distance[j] <<endl;
            
        //     cout<<"\n\n"<<endl;
        // }
        printMemorys_file(result, "./output/tc"+file_name_case+"_memlist.group");
        cout<<endl<<"**********************************OUTPUT**********************************"<<endl;
        cout<<"*"<<endl;
        cout<< "* Total number of groups: " << result.size() <<endl;
        cout<< "* The MBIST Partition Data has been output to " << "\"tc"+file_name_case+"_memlist.group\"" <<endl;
        cout<<"*"<<endl;
        cout<<"**************************************************************************"<<endl;
        
    } 
    
    return 0;
}