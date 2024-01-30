#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <map>
#include <dirent.h>
#include <utility>
#include <algorithm> 
#include <iterator>
#include <regex>
using namespace std;

bool if_have_def = 1;

map<string, string> find_dataClk;  // 用于存储 submemory 的时钟信息

struct SubMem {
    string name;   // 子存储器的名称
    string type;   // 子存储器的类型
    pair<int, int> position;  // 子存储器的位置
    string clockDomain;  // 子存储器的时钟域
    vector<string> level;         //存储器层次
};

map<string, SubMem> subMap;  

void setSubMap_position(const string& subMemKey, int x, int y) {
    	// 查找给定的 subMemKey 是否存在于 subMap 中
    	auto it = subMap.find(subMemKey);

    	if (it == subMap.end()) {
       	 	// 如果不存在，创建新的 SubMem 实例并设置位置
        	SubMem newSubMem;
        	newSubMem.position = make_pair(x, y);
        	subMap[subMemKey] = newSubMem;
        	cout<<"error to creat new subMemMap when initting the position!"<<endl;
   	 } else {
       		 // 如果已存在，修改现有实例的位置
        	it->second.position = make_pair(x, y);
   	 }
}
// 存储子存储器信息
void setSubMemory( string subMemoryName, SubMem& subMemoryInfo) {
       subMap[subMemoryName] = subMemoryInfo;
}

 // 在 const 成员函数中输出 map 中存储器的信息
void printMemoryMap(){
        for (const auto& memory : subMap) {
            cout << "Sub Memory Name: " << memory.second.name << endl;
            cout << "Sub Memory position: (" <<memory.second.position.first<<"," <<memory.second.position.second<<")"<< endl;
            cout << "Sub Memory clk: " << memory.second.clockDomain << endl;
            cout << "Sub Memory type: " << memory.second.type << endl<<endl<<endl;
            // 输出其他信息...
        }
}

class FirstMem {
private:
    double power;  // 功耗
    string MemoryType;  // 存储器类型
    string name;  // 存储器名称
    double area;  // 面积
    int NumberofWords = 0; //地址线
    int NumberofBits = 0;  //数据线
    string Algorithm = "";  //测试算法
    string LogicalPorts = ""; 
    string OperationSet = "";
    string ShadowRead = "";
    string ShadowWrite = "";
    string ShadowWriteOK = "";
    string WriteOutOfRange = "";
    vector<string> LogicalAddressMaP;
    bool RedundancyAnalysis = false;

public:
    FirstMem(){
    	power = 0.0;
    }	
    // 设置方法
    void setPower(double power) {
        this->power = power;
    }

    void setLogicalAddressMaP(const string& strr){
        this->LogicalAddressMaP.push_back(strr);
    }

    void setMemoryType(const string& MemoryType) {
        this->MemoryType = MemoryType;
    }
    
    void setNumberofWords(int NumberofWords) {
        this->NumberofWords = NumberofWords;
    }

    void setNumberofBits(int NumberofBits) {
        this->NumberofBits = NumberofBits;
    }


    void setAlgorithm(const string& Algorithm) {
        this->Algorithm = Algorithm;
    }

    void setLogicalPorts(const string& LogicalPorts) {
        this->LogicalPorts = LogicalPorts;
    }

    void setOperationSet(const string& OperationSet) {
        this->OperationSet = OperationSet;
    }

    void setShadowRead(const string& ShadowRead) {
        this->ShadowRead = ShadowRead;
    }

    void setShadowWrite(const string& ShadowWrite) {
        this->ShadowWrite = ShadowWrite;
    }

    void setShadowWriteOK(const string& ShadowWriteOK) {
        this->ShadowWriteOK = ShadowWriteOK;
    }

    void setWriteOutOfRange(const string& WriteOutOfRange) {
        this->WriteOutOfRange = WriteOutOfRange;
    }


    void setName(const string& name) {
        this->name = name;
    }

    void setArea(double area) {
        this->area = area;
    }

    void setRedundancyAnalysis(bool iff){
        this->RedundancyAnalysis = iff;
    }

    

    bool getRedundancyAnalysis(){
        return RedundancyAnalysis;
    }

    // 获取功耗
    double getPower() const {
        return power;
    }

    // 获取存储器类型
    string getMemoryType() const {
        return MemoryType;
    }

    vector<string> getLogicalAddressMaP() const {
        return LogicalAddressMaP;
    }

    int getNumberofWords() const {
        return NumberofWords;
    }

    int getNumberofBits() const {
        return NumberofBits;
    }

    string getAlgorithm() const {
        return Algorithm;
    }


    string getLogicalPorts() const {
        return LogicalPorts;
    }

    string getOperationSet() const {
        return OperationSet;
    }

    string getShadowRead() const {
        return ShadowRead;
    }

    string getShadowWrite() const {
        return ShadowWrite;
    }

    string getShadowWriteOK() const {
        return ShadowWriteOK;
    }

    string getWriteOutOfRange() const {
        return WriteOutOfRange;
    }


    // 获取存储器名称
    string getName() const {
        return name;
    }

    // 获取存储器面积
    double getArea() const {
        return area;
    }

   

   
};





bool isWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}


bool isLineEmpty(const std::string& line) {
    return std::all_of(line.begin(), line.end(), isWhitespace);
}
//读取时钟信息，并存储
void read_clk(const string& filename) {
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        //std::cerr << "Unable to open clockDomain file,perhaps the file does not exist !!!" << std::endl;
        cout<<"* parser  --------------------------> clk.txt File Not Found "<<endl;
        return ;
    }

    cout<<"* parser  --------------------------> "<<filename<<endl;

    std::string line;
    std::string current_key;
    while (1) {
        std::getline(file, line);
       
        if (isLineEmpty(line)) {
            break;
        }
    }
    
    while (std::getline(file, line)) {
    	
        if (!isLineEmpty(line)) {
        	
            if (line.find("clk_") != std::string::npos) {
                //dataClk[line] = std::vector<std::string>();
                size_t pos = line.find(':');
    		if (pos != std::string::npos) {
        		current_key = line.substr(0, pos);
    		}
                
                
            }
            else {
            	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                find_dataClk[line] = current_key;
                
                
                
            }
           
            
        }
        
    }

    file.close();
}

// map用于存储各类存储器的信息

std::map<std::string, FirstMem> memoryMap;
//////////
vector<std::string> stroe_result;

///
string mytolower(string s){
    int len=s.size();
    for(int i=0;i<len;i++){
        if(s[i]>='A'&&s[i]<='Z'){
            s[i]+=32;//+32转换为小写
            //s[i]=s[i]-'A'+'a';
        }
    }
    return s;
}

void readMemoryInformation01(const std::string& folderDsPath) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(folderDsPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            FirstMem newMem;
            std::string filePath = folderDsPath + "/" + ent->d_name;

            
            std::string key;
            
            if (ent->d_type == DT_REG) {  // 如果是文件
                
                std::ifstream file(filePath);
                if (file.is_open()) {
                    std::string line;

                    bool find_MilliWattsPerMegaHertz = false;
                    bool find_LogicalPorts = false;
                    bool find_Algorithm = false;
                    bool find_OperationSet = false;
                    bool find_ShadowRead = false;
                    bool find_ShadowWrite = false;
                    bool find_ShadowWriteOK = false;
                    bool find_WriteOutOfRange = false;
                    bool find_LogicalAddressMaP = false;
                    bool find_NumberofWords = false;
                    bool find_NumberofBits = false;
                    bool find_MemoryType = false;
                    bool find_RedundancyAnalysis = false;

                    while (std::getline(file, line)) {
                    	
                    	if (mytolower(line).find("cellname") != std::string::npos) {
                    		
                    		
                    		std::string s = line;
    
    				// 找到冒号的位置
    				size_t start_index = s.find(":") + 1;

   				// 找到分号的位置
    				size_t end_index = s.find(";");

    				// 提取冒号和分号之间的子串
    				std::string result = s.substr(start_index, end_index - start_index);

    				// 去除可能的额外空格
    				size_t first_not_space = result.find_first_not_of(" \t\n\r");
    				size_t last_not_space = result.find_last_not_of(" \t\n\r");
    				result = result.substr(first_not_space, last_not_space - first_not_space + 1);

    				
            
            			///////////
            			stroe_result.push_back(result);
            
	
            			key = result;  //the key of map,mark a memory.
            
            			newMem.setName(result);
                    		
                    		
                    	}
                    	
                    	
                        
                        if (!find_RedundancyAnalysis && (mytolower(line).find("redundancyanalysis") != std::string::npos)) {
                            //line = mytolower(line);
                        	find_RedundancyAnalysis = true;
                            newMem.setRedundancyAnalysis(find_RedundancyAnalysis);    
                        	
                        }

                        if (!find_MemoryType && (mytolower(line).find("memorytype") != std::string::npos)) {
                            //line = mytolower(line);
                        	find_MemoryType = true;
                            std::string inputString = line;
                        	// 查找冒号的位置
    				        size_t colonPos = inputString.find(':');
    				        if (colonPos == std::string::npos) {
        				        std::cerr << "找不到冒号" << std::endl;
        				
    				        }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());
                            newMem.setMemoryType(valueString);    
          
                        	
                        }

                        if (!find_MilliWattsPerMegaHertz && (mytolower(line).find("milliwattspermegahertz") != std::string::npos)) {
                            //line = mytolower(line);
                        	find_MilliWattsPerMegaHertz = true;
                            std::string inputString = line;
                        	// 查找冒号的位置
    				        size_t colonPos = inputString.find(':');
    				        if (colonPos == std::string::npos) {
        				        std::cerr << "找不到冒号" << std::endl;
        				
    				        }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 将提取的字符串转换为浮点数
                            double value;
                            std::istringstream(valueString) >> value;
                            newMem.setPower(value);    
          
                        	
                        }

                        if (!find_LogicalPorts && (mytolower(line).find("logicalports") != std::string::npos)) {
                            //line = mytolower(line);
                            find_LogicalPorts = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setLogicalPorts(valueString);
                            
                        }

                        if (!find_NumberofWords && (mytolower(line).find("numberofwords") != std::string::npos)) {
                            //line = mytolower(line);
                        	find_NumberofWords = true;
                            std::string inputString = line;
                        	// 查找冒号的位置
    				        size_t colonPos = inputString.find(':');
    				        if (colonPos == std::string::npos) {
        				        std::cerr << "找不到冒号" << std::endl;
        				
    				        }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 将提取的字符串转换为浮点数
                            int value;
                            std::istringstream(valueString) >> value;
                            newMem.setNumberofWords(value);                 
                        	
                        }

                        if (!find_NumberofBits && (mytolower(line).find("numberofbits") != std::string::npos)) {
                            //line = mytolower(line);
                        	find_NumberofBits = true;
                            std::string inputString = line;
                        	// 查找冒号的位置
    				        size_t colonPos = inputString.find(':');
    				        if (colonPos == std::string::npos) {
        				        std::cerr << "找不到冒号" << std::endl;
        				
    				        }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 将提取的字符串转换为浮点数
                            int value;
                            std::istringstream(valueString) >> value;
                            newMem.setNumberofBits(value);        
                                     
                        	
                        }
                        
                        if (!find_Algorithm && (mytolower(line).find("algorithm") != std::string::npos)) {
                            find_Algorithm = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setAlgorithm(valueString);
                            
                        }

                        if (!find_OperationSet && (mytolower(line).find("operationset") != std::string::npos)) {
                            find_OperationSet = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setOperationSet(valueString);
                            
                        }

                        if (!find_ShadowRead && (mytolower(line).find("shadowread") != std::string::npos)) {
                            find_ShadowRead = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setShadowRead(valueString);
                            
                        }
                        

                        if (!find_ShadowWrite && (mytolower(line).find("shadowwrite") != std::string::npos)) {
                            find_ShadowWrite = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setShadowWrite(valueString);
                            
                        }

                        if (!find_ShadowWriteOK && (mytolower(line).find("shadowwriteok") != std::string::npos)) {
                            find_ShadowWriteOK = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setShadowWriteOK(valueString);
                            
                        }

                        if (!find_WriteOutOfRange && (mytolower(line).find("writeoutofrange") != std::string::npos)) {
                            find_WriteOutOfRange = true;
                            std::string inputString = line;
                            // 查找冒号的位置
                            size_t colonPos = inputString.find(':');
                            if (colonPos == std::string::npos) {
                                std::cerr << "找不到冒号" << std::endl;
                                // 处理找不到冒号的情况
                            }

                            // 查找分号的位置
                            size_t semicolonPos = inputString.find(';', colonPos);
                            if (semicolonPos == std::string::npos) {
                                std::cerr << "找不到分号" << std::endl;
                                // 处理找不到分号的情况
                            }

                            // 提取冒号和分号之间的部分
                            std::string valueString = inputString.substr(colonPos + 1, semicolonPos - colonPos - 1);

                            // 去除多余空格
                            valueString.erase(std::remove_if(valueString.begin(), valueString.end(), ::isspace), valueString.end());

                            newMem.setWriteOutOfRange(valueString);
                            
                        }

                        if (!find_LogicalAddressMaP && (mytolower(line).find("logicaladdressmap") != std::string::npos)) {
                            find_LogicalAddressMaP = true;
                            while(1){
                                std::getline(file, line);
                                if(line.find('}') != std::string::npos){
                                    break;
                                }
                                std::string inputString = line;
                                std::string extractedContent;
                                // 查找分号的位置
                                size_t semicolonPos = inputString.find(';');
                                if (semicolonPos == std::string::npos) {
                                    size_t semicolonPos_1 = inputString.rfind(':');
                                    if (semicolonPos_1 == std::string::npos){
                                        std::cerr << "找不到分号and':'" << std::endl;
                                    }
                                    
                                    extractedContent = inputString.substr(0, semicolonPos_1 + 1);
                                        
                                    std::getline(file, line);
                                    inputString = line;
                                    size_t semicolonPos_2;
                                    semicolonPos_2 = inputString.find(';');
                                    if (semicolonPos_2 == std::string::npos){
                                        std::cerr << "找不到分号" << std::endl;
                                    }
                                    
                                    extractedContent.append(inputString.substr(0, semicolonPos_2));
                                    extractedContent.erase(std::remove_if(extractedContent.begin(), extractedContent.end(), ::isspace), extractedContent.end());
                                    
                                    
        
                                }
                                if (semicolonPos != std::string::npos) {
                                    extractedContent = inputString.substr(0, semicolonPos);
                                    extractedContent.erase(std::remove_if(extractedContent.begin(), extractedContent.end(), ::isspace), extractedContent.end());
                                }
                                // 去除多余空格
                                
                                newMem.setLogicalAddressMaP(extractedContent);
                            }
                            
                        
                        }
                        
                    }
                    file.close();
                
                }
                else {
                    //std::cerr << "Unable to open file: " << ent->d_name << '\n';
                }
                memoryMap[key] = newMem;
            }
        }
        closedir(dir);
    }
    else {
        cout<<"* parser  --------------------------> .lvlib File Not Found "<<endl;
    }
}

void readMemoryInformation02(const std::string& folderDsPath) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(folderDsPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            
            std::string filePath = folderDsPath + "/" + ent->d_name;

           
	

            std::string key;  //the key of map,mark a memory.
           
            if (ent->d_type == DT_REG) {  // 如果是文件
                std::ifstream file(filePath);
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                    
                    
                    	if (mytolower(line).find("memory name") != std::string::npos){
                    		
                    		
                    		std::string s = line;

    				// 找到冒号的位置
    				size_t colon_index = s.find(":");
    
    				// 如果找到了冒号
    				if (colon_index != std::string::npos) {
        				// 提取冒号后面的部分
        				std::string after_colon = s.substr(colon_index + 1);

        				// 使用字符串流来提取第一个连续的字符串
        				std::istringstream iss(after_colon);
        				std::string result;
        				iss >> result;

        				key = result;
    				} else {
        				std::cout << "冒号未找到" << std::endl;
    				}
    				
    				
                    		
                    		
                    	}
                        
                        
                        if (line.find("CONFIGURATION") != std::string::npos ) {
                        	std::getline(file, line);
                        	std::string inputString = line;

				std::istringstream iss(inputString);
				std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

    	
    				double lastDouble = 0.0;
    				for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        				std::istringstream iss(*it);
        				if (iss >> lastDouble) {
            	
            					break;
        				}
    				}
				
				
				
    				if(memoryMap[key].getPower() == 0.0){
    					
    					memoryMap[key].setPower(lastDouble);
    					
    				}
    				
                        	
                        	
                        	
                        	
                        	
                        	
                        }
                        
                        
                    }
                    file.close();
                }
                else {
                   // std::cerr << "Unable to open file: " << ent->d_name << '\n';
                }
                
            }
        }
        closedir(dir);
    }
    else {
        cout<<"* parser  --------------------------> .summ File Not Found "<<endl;
    }
}



//std::string folderPath = "your_folder_path"; // 更改为文件夹的路径
//readMemoryInformation(folderPath);
void readMemoryInformation03(const std::string& folderDsPath) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(folderDsPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
           
            std::string filePath = folderDsPath + "/" + ent->d_name;

          

	



            std::string key;  //the key of map,mark a memory.
            
            if (ent->d_type == DT_REG) {  // 如果是文件
                std::ifstream file(filePath);
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                    	
                    	
                    	if (mytolower(line).find("memory name") != std::string::npos){
                    		
                    		
                    		std::string s = line;

    				// 找到冒号的位置
    				size_t colon_index = s.find(":");
    
    				// 如果找到了冒号
    				if (colon_index != std::string::npos) {
        				// 提取冒号后面的部分
        				std::string after_colon = s.substr(colon_index + 1);

        				// 使用字符串流来提取第一个连续的字符串
        				std::istringstream iss(after_colon);
        				std::string result;
        				iss >> result;

        				key = result;
    				} else {
        				std::cout << "冒号未找到" << std::endl;
    				}
    				
    				
                    		
                    		
                    	}
                    
                    	
                        if (line.find("Area(um^2)") != std::string::npos) {
                            std::getline(file, line);
                            std::getline(file, line);

                            std::stringstream ss(line);
                            std::string token;


                            std::getline(ss, token, '|');
                            std::getline(ss, token, '|');
                            std::getline(ss, token, '|');
                            std::getline(ss, token, '|');

                            memoryMap[key].setArea(stod(token));
                        }
                        
                        if (line.find("Activity") != std::string::npos) {
                        	std::getline(file, line);
                        	if (line.find("Factor") != std::string::npos) {
                        		double power = 0.0;
                        		std::getline(file, line);
                        		for(int i=0;i<7;++i){
                        			std::getline(file, line);
                        			std::istringstream ss(line);
    						std::string token;

    						// 按空格分割每个部分，然后获取最后一个部分
    						while (ss >> token) {
     						   // 读取最后一个部分作为数字
    						}
    
   						 if(power<stod(token)){
   						 	power = stod(token) ;
   						 }
                        			
                        			
                        		}
                        		if(memoryMap[key].getPower() == 0.0){
    						memoryMap[key].setPower(power);
    					}
                        		
                        		
                        	}
                        }
                        
                        
                    }
                    file.close();
                }
                else {
                    //std::cerr << "Unable to open file: " << ent->d_name << '\n';
                }
                
            }
        }
        closedir(dir);
    }
    else {
        cout<<"* parser  --------------------------> .ds File Not Found "<<endl;
    }
}



bool setDataFromFiles(const string& filename1, const string& filename2) {
    ifstream file1(filename1);
    ifstream file2(filename2);

    if(!file2.is_open()){
        if_have_def = false;
        cout<<"* parser  --------------------------> .def File Not Found "<<endl;
    }
    
    string line;
    string keyString;
    if(file1.is_open()){
    // 从第一个文件读取数据并设置属性
    while (getline(file1, line)) {
        // 解析文件1中的数据并设置到对象属性中
        // 例如：this->power = valueFromFile1;
        
        if (isLineEmpty(line)) {
       
            continue;  // 忽略包含 "topd" 的行
            
        }
        else {
            
            if (line.find("top") != string::npos) {
            	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            	
                struct SubMem newSubMemInstance;
                
                newSubMemInstance.name = line;
                newSubMemInstance.type = keyString;
                //cout<<find_dataClk[line]<<endl;
                newSubMemInstance.clockDomain = find_dataClk[line];
                
                std::string s = line;
    
    	
    		std::istringstream iss(s);
    		std::string level_index;

    		// 使用getline和'/'作为分隔符切割字符串
    		while (std::getline(iss, level_index, '/')) {
        		newSubMemInstance.level.push_back(level_index);
    		}

    		
                // 输出切割后的结果
    		// for (const auto& l : newSubMemInstance.level) {
        	// 	std::cout << l << std::endl;
    		// }
                
                
                
                //cout<<newSubMemInstance.clockDomain<<endl;
                setSubMemory(line, newSubMemInstance);
               
              
                   
            }
            else {
                size_t found = line.find(":");
                
                if (found != string::npos) {
                    
                    
                    keyString = line.substr(0, found);
                   
                   
                   

                }

            }
        }
    }
    }
	
	
    if(file2.is_open()){
    // 从第二个文件读取数据并设置属性
    while (getline(file2, line)) {
        // 解析文件2中的数据并设置到对象属性中
        // 例如：this->location = valueFromFile2;
        
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        string input = line;
        
        string subMemoryMap_key;
	
    
    	 
    	 
    	 //acquir the key of subMap
    	size_t firstSpace = input.find(' ');
    	if (firstSpace != string::npos) {
        	size_t secondSpace = input.find(' ', firstSpace + 1);
        	if (secondSpace != string::npos) {
            		subMemoryMap_key = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            		
        	}
    	} 
    	
    	
    	//acquir the position
    	int firstNum, secondNum;
    	size_t leftBracket = input.find('(');
   	size_t rightBracket = input.find(')');
   	

    	if (leftBracket != std::string::npos && rightBracket != std::string::npos) {
       		 std::string numbers = input.substr(leftBracket + 1, rightBracket - leftBracket - 1);

        	 std::istringstream iss(numbers);
        	 
       		 iss >> firstNum >> secondNum;
	
       	} else {
        	//std::cout << "Brackets not found or complete information inside brackets not present." << std::endl;
    	}
    	
    	
	setSubMap_position(subMemoryMap_key, firstNum, secondNum);
        
    }
    }

    file1.close();
    file2.close();
    return true;
}

std::vector<std::vector<std::string>> initMemPart;
std::vector<std::vector<std::string>> endMemPart;

void partInitMem(const string& filename){
	ifstream file(filename);
    	

   	 if (!file.is_open() ) {
       	 cerr << "Unable to open file." << endl;
       	 return;
   	 }

    	string line;
    	string clk_name;
    	std::vector<std::string> clk_name_vector;
    	std::map<std::string, std::vector<std::string>> clk_map_vector;
    	bool flag = false;
    	
   	 // 从文件读取数据
   	 while (getline(file, line)) {
   	 	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
   	 	if (isLineEmpty(line)) {
       
            		continue;  // 忽略包含 "topd" 的行
            
        	}
        	else {
        		if (line.find("top") != string::npos) {
        			flag = false;
        			clk_name = find_dataClk[line];
        			auto it = std::find(clk_name_vector.begin(), clk_name_vector.end(), clk_name);

   				 // 判断字符串是否找到
    				if (it != clk_name_vector.end()) {
        				flag = true;
    				} 
    				
    				if(flag == true){
    					clk_map_vector[clk_name].push_back(line);
    				}else{
    					clk_name_vector.push_back(clk_name);
    					std::vector<std::string> tempVector;
    					tempVector.push_back(line);
    					clk_map_vector[clk_name] = tempVector;
    				}
        			
        			
        		}else{
        			
        			
        			if (!clk_name_vector.empty()) {
        				// 使用范围-based for 循环遍历向量
        				for (const auto& str : clk_name_vector) {
            					initMemPart.push_back(clk_map_vector[str]);
        				}
        				
        				//delete
        				// 清空向量
    					clk_name_vector.clear();
    					// 清空映射
    					clk_map_vector.clear();
   				}
        			
        			
        			
        		}
        	}
   	 }
}




bool is_merge(string block_name1, string block_name2){

    if(subMap[block_name1].clockDomain != subMap[block_name2].clockDomain){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getNumberofWords() != memoryMap[subMap[block_name2].type].getNumberofWords()){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getMemoryType() != memoryMap[subMap[block_name2].type].getMemoryType() ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getRedundancyAnalysis() != memoryMap[subMap[block_name2].type].getRedundancyAnalysis() ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getAlgorithm() != memoryMap[subMap[block_name2].type].getAlgorithm()  ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getLogicalPorts() != memoryMap[subMap[block_name2].type].getLogicalPorts()   ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getOperationSet() != memoryMap[subMap[block_name2].type].getOperationSet()   ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getShadowRead() != memoryMap[subMap[block_name2].type].getShadowRead()  ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getShadowWrite() != memoryMap[subMap[block_name2].type].getShadowWrite() ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getShadowWriteOK() != memoryMap[subMap[block_name2].type].getShadowWriteOK() ){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getWriteOutOfRange()!= memoryMap[subMap[block_name2].type].getWriteOutOfRange()){
        return false;
    }
    if(memoryMap[subMap[block_name1].type].getLogicalAddressMaP().size() != memoryMap[subMap[block_name2].type].getLogicalAddressMaP().size()){
        return false;
    }else{
        for(int i=0; i < memoryMap[subMap[block_name1].type].getLogicalAddressMaP().size(); ++i){
            if(memoryMap[subMap[block_name1].type].getLogicalAddressMaP()[i] != memoryMap[subMap[block_name2].type].getLogicalAddressMaP()[i] ){
                return false;
            }
        }
    }

    return true;
    

}

void repartition(){
    for(int i=0;i < initMemPart.size();i++){
        for(int j = i+1; j < initMemPart.size(); j++){
            if(is_merge(initMemPart[i].front(),initMemPart[j].front())){
                initMemPart[i].insert(
                    initMemPart[i].end(),
                    initMemPart[j].begin(),
                    initMemPart[j].end()
                );
                initMemPart.erase(initMemPart.begin()+j);
                j--;
            }
        }

    }
}

void eda_parser(string filefolder){

    cout<<endl<<"**********************************PARSER**********************************"<<endl;
    cout<<"*"<<endl;
    string lvlib = filefolder + "/" + "lvlib";
    string summ = filefolder + "/" + "summ";
    string ds = filefolder + "/" + "ds";
    string clk_txt;
    string c_f;
    string c_def;
    readMemoryInformation01(lvlib);
    readMemoryInformation02(summ);
    readMemoryInformation03(ds);
	
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(filefolder.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
           if(ent->d_name[0] != '.'){
                cout<<"* parser  --------------------------> "<<filefolder+"/"+ ent->d_name<<endl;
           }
           

           	std::string myString = ent->d_name;

    		// 定义可能的后缀
    		std::string suffix1 = ".txt";
    		std::string suffix2 = ".f";
  	  	    std::string suffix3 = ".def";

    		// 判断是以哪一个后缀结尾，并输出结果
    		if (myString.length() >= suffix1.length() && myString.compare(myString.length() - suffix1.length(), suffix1.length(), suffix1) == 0) {
        		clk_txt = filefolder + "/" + ent->d_name;
        		
    		} else if (myString.length() >= suffix2.length() && myString.compare(myString.length() - suffix2.length(), suffix2.length(), suffix2) == 0) {
        		c_f = filefolder + "/" + ent->d_name;
        		
    		} else if (myString.length() >= suffix3.length() && myString.compare(myString.length() - suffix3.length(), suffix3.length(), suffix3) == 0) {
        		c_def = filefolder + "/" + ent->d_name;
        		
    		} 

                          
           
        }
        closedir(dir);
    }
    else {
        std::cerr << "Unable to open directory: " << filefolder << std::endl;
    }
    
	
	
    read_clk(clk_txt);
    
    
    setDataFromFiles(c_f, c_def);
    
    
    partInitMem(c_f);

    

    repartition();
    cout<<"*"<<endl;
    cout<<"**************************************************************************"<<endl;
    
    
}

struct Memory
{
    string id;
    double power;
    double x;
    double y;
    vector<string> level;
};

vector<vector<Memory>> tran_to_Mem(const vector<vector<string>>& initMemPart){
    vector<vector<Memory>> result;
    for(int i=0; i<initMemPart.size(); ++i){
        vector<Memory> temp_group;
        for(int j=0; j<initMemPart[i].size(); ++j){
            Memory temp_mem;
            temp_mem.id = initMemPart[i][j];
            temp_mem.x = subMap[initMemPart[i][j]].position.first;
            temp_mem.y = subMap[initMemPart[i][j]].position.second;
            temp_mem.power = memoryMap[subMap[initMemPart[i][j]].type].getPower();
            
            for(int m = 0; m<subMap[initMemPart[i][j]].level.size(); m++){
                temp_mem.level.push_back(subMap[initMemPart[i][j]].level[m]);
            }
            temp_group.push_back(temp_mem);
        }
        result.push_back(temp_group);
    }
    return result;
}
