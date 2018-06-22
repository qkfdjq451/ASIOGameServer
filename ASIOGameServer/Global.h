//전역 헤더
#pragma once

#pragma warning( disable : 4267 )		//  size_t <-> other type 변환 경고

#define BUFSIZE	2048
#define _WIN32_WINNT  0x0603

#define MIN_flt			(1.175494351e-38F)		
#define MAX_flt			(3.402823466e+38F)

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include <queue>
#include <string>
#include<utility>
#include <mutex>
#include <future>
#include <algorithm>
#include <random>
#include <fstream>
#include <stack>
#include <deque>
//전역 파일 위치
const std::string NaviDatas = "../NavigationData/";


//Typedef
typedef unsigned int	uint;
typedef unsigned char uint8;

typedef std::map<const std::string, std::vector<const char*>> QMap;
typedef std::vector<std::vector<std::pair<std::string,std::string>>> QVector;
typedef std::vector<std::shared_ptr<class Component>> ComVector;

#include "Math.h"
#include "./Packet.h"

#include "Flag.h"
#include "Structs.h"

#include "Log/LogManager.h"
#include "System/Time.h"

#include "Asio/Asio.h"

#include "EventManager/React.h"
#include "Mysql/MysqlManager.h"


#include "./Component/Component.h"
#include "./WorkerGroup//WorkerGroup.h"

