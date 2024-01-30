#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include "eda_GA.h"

using namespace std;

//全局变量
double mutation_prob = 0.1; // 变异概率设置为0.05
int itr = 2000;

struct solution {
    vector<int> mem_of_group;
    int groupNum;
    //double energy = 0;
};

//打印到文件
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
            outPutFile << unit.name <<  endl;
        }
        outPutFile <<  endl;
        //outPutFile << "\nsumPower = " << sumPower << endl<<endl;
    }
}

//dis距离是否满足
bool distance_dis(const Memory& Mem, const Memory& b, double maxDis) {
    double dis = 0;
    
    double dx = Mem.x - b.x;
    double dy = Mem.y - b.y;
    dis = sqrt(dx * dx + dy * dy);
    if(dis > maxDis){
        return false;
    } 
    return true;
}

//每组功耗
double calculateTotalPower(const vector<Memory>& group) {
    double totalPower = 0.0;
    for (const Memory& unit : group) {
        totalPower += unit.power;
    }
    return totalPower;
}

// 变异操作 - 组内成员变异
void mutation(solution& offspring) {
    for (int j = 0; j < offspring.mem_of_group.size(); j++) {
        if (rand() < mutation_prob * RAND_MAX) {
            int new_group = rand() % offspring.groupNum + 1; // 随机选择一个新的组
            offspring.mem_of_group[j] = new_group;
        }
    }
}

// 变异操作 - 顺序移位
void mutationShift(solution& offspring) {
    if (rand() < mutation_prob * RAND_MAX) {
        int length = int((rand() % 1000 / 1000.0) * offspring.mem_of_group.size());
        int off = rand() % offspring.mem_of_group.size() + 1;
        vector<int> m;
        unordered_map<int, bool> selected;
        for (int i = off; i < offspring.mem_of_group.size() && i < off + length; i++) {
            m.push_back(offspring.mem_of_group[i]);
            selected[offspring.mem_of_group[i]] = true;
        }
        for (int i = 0; i < offspring.mem_of_group.size(); i++) {
            if (selected.count(offspring.mem_of_group[i]) == 0) {
                m.push_back(offspring.mem_of_group[i]);
            }
        }
        for (int i = 0; i < m.size(); i++) {
            offspring.mem_of_group[i] = m[i];
        }
    }
}

//计算分组数
int calculateGroupNumber(solution offspring){
    //max_group_number = 0;
    bool isGroup[10000] = {false};
    int groupNumber = 0;
    for(int i=0; i<offspring.mem_of_group.size(); i++){
        //max_group_number = max(max_group_number, offspring.mem_of_group[i]);
        if(!isGroup[offspring.mem_of_group[i]]){
            isGroup[offspring.mem_of_group[i]] = true;
            groupNumber++;
        }
    }
    return groupNumber;
}

void genetic_algorithm_dis(vector<Memory>& Memorys, double distanceLimit, double powerLimit, vector<vector<Memory>>& result, int size_population) {
    //分组大小
    int size_of_mem = Memorys.size();
    //编号
    for(int i=0; i<size_of_mem; i++){
        Memorys[i].id = i;
    }
    //不超过1，超过0
    int dis_every[size_of_mem][size_of_mem] = {1};
    // char **dis_every;
    // dis_every = new char *[size_of_mem]; 
    // for(int i=0; i<3000; ++i) 
    // dis_every[i] = new char[size_of_mem]; 
    for(int i=0; i<size_of_mem; i++){
        for(int j=i+1; j<size_of_mem; j++){
            if(distance_dis(Memorys[i], Memorys[j], distanceLimit)){
                dis_every[Memorys[i].id][Memorys[j].id] = 1;
                dis_every[Memorys[j].id][Memorys[i].id] = 1;
            }
            else{
                dis_every[Memorys[i].id][Memorys[j].id] = 0;
                dis_every[Memorys[j].id][Memorys[i].id] = 0;
            }
        }
    }

    //贪心生成初始种群
    vector<solution> init_population;
    
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
            bool flag = false;
            for(int i=0; i<currentGroup.size(); i++){
                if(dis_every[currentGroup[i].id][(*it).id] == 0){
                    flag = false;
                }
                else{
                    flag = true;
                }
            }
            
            if (flag && calculateTotalPower(currentGroup) + it->power <= powerLimit) {
                currentGroup.push_back(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }
        // 将当前组加入结果
        result_1.push_back(currentGroup);
    }
    solution chromosome;
    chromosome.mem_of_group.resize(size_of_mem); // n 是你想要的大小
    chromosome.groupNum = result_1.size();
    for(int i=0; i<result_1.size(); i++){
        for(int j=0; j<result_1[i].size(); j++){
            chromosome.mem_of_group[result_1[i][j].id] = i+1;    //组数从1开始
        }
    }
    for(int i=0; i<size_population; i++){
        init_population.push_back(chromosome);
    }    
    int max_group_num = chromosome.groupNum;

    //进入遗传算法
    vector<solution> current_population = init_population;
    vector<solution> temp_population = current_population;
    //vector<vector<Memory>> best_result;

    int temp_itr = 1;
    cout<<"dd"<<endl;
    while(temp_itr < itr){
        
        bool isValid = true;
        // 锦标赛选择生成父代和母代
        vector<solution> parents;
        for (int i = 0; i < size_population; i++) {
            int index1 = rand() % size_population;
            int index2 = rand() % size_population;
            parents.push_back(current_population[index1].groupNum < current_population[index2].groupNum ? current_population[index1] : current_population[index2]);
        }

        // 交叉生成子代种群
        vector<solution> offspring_population;
        for (int i = 0; i < size_population; i += 2) {
            solution parent1 = parents[i];
            solution parent2 = parents[i + 1];
            int crossover_point = rand() % size_of_mem;

            solution offspring1;
            offspring1.mem_of_group = parent1.mem_of_group;
            for (int j = crossover_point; j < size_of_mem; j++) {
                offspring1.mem_of_group[j] = parent2.mem_of_group[j];
            }
            offspring1.groupNum = calculateGroupNumber(offspring1);

            solution offspring2;
            offspring2.mem_of_group = parent2.mem_of_group;
            for (int j = crossover_point; j < size_of_mem; j++) {
                offspring2.mem_of_group[j] = parent1.mem_of_group[j];
            }
            offspring2.groupNum = calculateGroupNumber(offspring2);

            offspring_population.push_back(offspring1);
            offspring_population.push_back(offspring2);
        }

        // 变异操作
        for (int i = 0; i < size_population; i++) {
            solution& offspring = offspring_population[i];
            double prob_select = (double)rand() / RAND_MAX;
            
            //组内变异
            if(prob_select < 0.9){
                mutation(offspring);
                offspring.groupNum = calculateGroupNumber(offspring);
            } 
            // 执行顺序位移变异操作
            else {
                mutationShift(offspring);
                offspring.groupNum = calculateGroupNumber(offspring);
            }
        }
        cout<<"lala"<<endl;
        // 计算适应度和分组情况
        for (int i = 0; i < size_population; i++) {
            solution& offspring = offspring_population[i];
            vector<vector<Memory>> offspring_result;
            vector<double> groupPower;

            // 计算 groupNum（分组数）
            //int max_group_num;
            int groupNum = calculateGroupNumber(offspring); // 自定义函数，根据个体的分组方案计算分组数
            offspring.groupNum = groupNum;
            offspring_result.resize(max_group_num);
            groupPower.resize(max_group_num, 0.0);

            // 将内存按照分组情况进行分配
            for (int j = 0; j < size_of_mem; j++) {
                int group = offspring.mem_of_group[j] - 1;
                offspring_result[group].push_back(Memorys[j]);
                groupPower[group] += Memorys[j].power; // 累加每个组的功耗
            }

            // 检查限制条件
            for (int group = 0; group < max_group_num; group++) {
                if (groupPower[group] > powerLimit) {
                    isValid = false;
                    break;
                }
                // 如果不满足其他限制条件，设置 isValid = false; 并跳出循环
                for(int i=0; i<offspring_result[group].size(); i++){
                    for(int j=i+1; j<offspring_result[group].size(); j++){
                        if(!dis_every[offspring_result[group][i].id][offspring_result[group][j].id]){
                            isValid = false;
                            break;
                        }
                    }
                }
            }

            if (!isValid) {
                break;
            }
            
        }

        if(!isValid){
            temp_itr++;
            current_population = temp_population;
            continue;
        }

        // 精英选择
        for (int i = 0; i < size_population; i++) {
            solution& current = current_population[i];
            solution& offspring = offspring_population[i];
            if (offspring.groupNum > current.groupNum) {
                offspring = current;
            }
        }

        // 更新当前代的种群
        current_population = offspring_population;
        temp_population = offspring_population;

        temp_itr++;    

    }
    //选择里面最好的
    int index = 0;
    int minNum = current_population[0].groupNum;
    for(int i=0; i<size_population; i++){
        if(current_population[i].groupNum < minNum){
            index = i;
        }
    }
    
    vector<vector<Memory>> temp(max_group_num+1);
    for(int i=0; i<size_of_mem; i++){
        temp[current_population[index].mem_of_group[i]].push_back(Memorys[i]);
    }
    
    for(int i=0; i<(max_group_num+1); i++){
        if(!empty(temp[i])){
            result.push_back(temp[i]);
        }
    }
}


void genetic_algorithm_block(vector<Memory>& Memorys, int distanceLimit, double powerLimit, vector<vector<Memory>>& result, int size_population) {
    //分组大小
    int size_of_mem = Memorys.size();
    //编号
    for(int i=0; i<size_of_mem; i++){
        Memorys[i].id = i;
    }
    //不超过1，超过0
    //int dis_every[size_of_mem][size_of_mem] = {1};
    vector<vector<int>> dis_every(size_of_mem);
    for(int i=0; i<size_of_mem; i++){
        vector<int> temp(size_of_mem, 1);
        dis_every[i] = temp;
    }
    for(int i=0; i<size_of_mem; i++){
        for(int j=i+1; j<size_of_mem; j++){
            int dis, k=0;
            while(k<Memorys[i].level.size() && k<Memorys[j].level.size()){
                if(Memorys[i].level[k] != Memorys[j].level[k]){
                    break;
                }  
                else k++;
            }       
            dis = max(Memorys[i].level.size(), Memorys[j].level.size()) - k -1;
            if (dis > distanceLimit) {
                dis_every[Memorys[i].id][Memorys[j].id] = 0;
                dis_every[Memorys[j].id][Memorys[i].id] = 0;
            }
            else{
                dis_every[Memorys[i].id][Memorys[j].id] = 1;
                dis_every[Memorys[j].id][Memorys[i].id] = 1;
            }
        }
        
    } 

    //贪心生成初始种群
    vector<solution> init_population;
    
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
            bool flag = false;
            for(int i=0; i<currentGroup.size(); i++){
                if(dis_every[currentGroup[i].id][(*it).id] == 0){
                    flag = false;
                }
                else{
                    flag = true;
                }
            }
            
            if (flag && calculateTotalPower(currentGroup) + it->power <= powerLimit) {
                currentGroup.push_back(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }
        // 将当前组加入结果
        result_1.push_back(currentGroup);
    }
    solution chromosome;
    chromosome.mem_of_group.resize(size_of_mem); // n 是你想要的大小
    chromosome.groupNum = result_1.size();
    for(int i=0; i<result_1.size(); i++){
        for(int j=0; j<result_1[i].size(); j++){
            chromosome.mem_of_group[result_1[i][j].id] = i+1;    //组数从1开始
        }
    }
    for(int i=0; i<size_population; i++){
        init_population.push_back(chromosome);
    }    
    int max_group_num = chromosome.groupNum;

    //进入遗传算法
    vector<solution> current_population = init_population;
    vector<solution> temp_population = current_population;
    //vector<vector<Memory>> best_result;

    int temp_itr = 1;
    cout<<"dd"<<endl;
    while(temp_itr < itr){
        
        bool isValid = true;
        // 锦标赛选择生成父代和母代
        vector<solution> parents;
        for (int i = 0; i < size_population; i++) {
            int index1 = rand() % size_population;
            int index2 = rand() % size_population;
            parents.push_back(current_population[index1].groupNum < current_population[index2].groupNum ? current_population[index1] : current_population[index2]);
        }

        // 交叉生成子代种群
        vector<solution> offspring_population;
        for (int i = 0; i < size_population; i += 2) {
            solution parent1 = parents[i];
            solution parent2 = parents[i + 1];
            int crossover_point = rand() % size_of_mem;

            solution offspring1;
            offspring1.mem_of_group = parent1.mem_of_group;
            for (int j = crossover_point; j < size_of_mem; j++) {
                offspring1.mem_of_group[j] = parent2.mem_of_group[j];
            }
            offspring1.groupNum = calculateGroupNumber(offspring1);

            solution offspring2;
            offspring2.mem_of_group = parent2.mem_of_group;
            for (int j = crossover_point; j < size_of_mem; j++) {
                offspring2.mem_of_group[j] = parent1.mem_of_group[j];
            }
            offspring2.groupNum = calculateGroupNumber(offspring2);

            offspring_population.push_back(offspring1);
            offspring_population.push_back(offspring2);
        }

        // 变异操作
        for (int i = 0; i < size_population; i++) {
            solution& offspring = offspring_population[i];
            double prob_select = (double)rand() / RAND_MAX;
            
            //组内变异
            if(prob_select < 0.9){
                mutation(offspring);
                offspring.groupNum = calculateGroupNumber(offspring);
            } 
            // 执行顺序位移变异操作
            else {
                mutationShift(offspring);
                offspring.groupNum = calculateGroupNumber(offspring);
            }
        }
        cout<<"lala"<<endl;
        // 计算适应度和分组情况
        for (int i = 0; i < size_population; i++) {
            solution& offspring = offspring_population[i];
            vector<vector<Memory>> offspring_result;
            vector<double> groupPower;

            // 计算 groupNum（分组数）
            //int max_group_num;
            int groupNum = calculateGroupNumber(offspring); // 自定义函数，根据个体的分组方案计算分组数
            offspring.groupNum = groupNum;
            offspring_result.resize(max_group_num);
            groupPower.resize(max_group_num+1, 0.0);

            // 将内存按照分组情况进行分配
            for (int j = 0; j < size_of_mem; j++) {
                int group = offspring.mem_of_group[j] - 1;
                offspring_result[group].push_back(Memorys[j]);
                groupPower[group] += Memorys[j].power; // 累加每个组的功耗
            }

            // 检查限制条件
            for (int group = 0; group < max_group_num; group++) {
                if (groupPower[group] > powerLimit) {
                    isValid = false;
                    break;
                }
                // 如果不满足其他限制条件，设置 isValid = false; 并跳出循环
                for(int i=0; i<offspring_result[group].size(); i++){
                    for(int j=i+1; j<offspring_result[group].size(); j++){
                        if(!dis_every[offspring_result[group][i].id][offspring_result[group][j].id]){
                            isValid = false;
                            break;
                        }
                    }
                }
            }

            if (!isValid) {
                break;
            }
            
        }

        if(!isValid){
            temp_itr++;
            current_population = temp_population;
            continue;
        }

        // 精英选择
        for (int i = 0; i < size_population; i++) {
            solution& current = current_population[i];
            solution& offspring = offspring_population[i];
            if (offspring.groupNum > current.groupNum) {
                offspring = current;
            }
        }

        // 更新当前代的种群
        current_population = offspring_population;
        temp_population = offspring_population;

        temp_itr++;    

    }
    //选择里面最好的
    int index = 0;
    int minNum = current_population[0].groupNum;
    for(int i=0; i<size_population; i++){
        if(current_population[i].groupNum < minNum){
            index = i;
        }
    }
    
    vector<vector<Memory>> temp(max_group_num+1);
    for(int i=0; i<size_of_mem; i++){
        temp[current_population[index].mem_of_group[i]].push_back(Memorys[i]);
    }
    
    for(int i=0; i<(max_group_num+1); i++){
        if(!empty(temp[i])){
            result.push_back(temp[i]);
        }
    }
}

int main(int argc, char ** argv){

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

    //int itr = 1;   //最大迭代次数
    int size_population = 4;   //种群大小  

    if(if_have_def && choice=="-dis"){
        distanceLimit = stod(dis_1);
        for(int i=0; i<init_group.size(); i++){
            
            genetic_algorithm_dis(init_group[i], distanceLimit, powerLimit, result, size_population);
            
        }
        
        printMemorys_file(result, "./output/tc"+file_name_case+"_memlist.group");
        cout<<endl<<"**********************************OUTPUT**********************************"<<endl;
        cout<<"*"<<endl;
        cout<< "* Total number of groups: " << result.size() <<endl;
        cout<< "* The MBIST Partition Data has been output to " << "\"tc"+file_name_case+"_memlist.group\"" <<endl;
        cout<<"*"<<endl;
        cout<<"**************************************************************************"<<endl;
        
        
  
    }
    
    if(choice=="-block"){
        distanceLimit = stod(dis_1);
        for(int i=0; i<init_group.size(); i++){
            
            genetic_algorithm_block(init_group[i], distanceLimit, powerLimit, result, size_population);
            
        }
        
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