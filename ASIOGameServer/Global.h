//전역 헤더
#pragma once

#pragma warning( disable : 4267 )		//  size_t <-> other type 변환 경고

#define BUFSIZE	1024
#define _WIN32_WINNT  0x0603

typedef unsigned int uint;
typedef unsigned char 		uint8;


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

#include "./Packet.h"

#include "Flag.h"
#include "Structs.h"

#include "Asio/Asio.h"


#include "./Component/Component.h"
#include "./WorkerGruop/WorkerGruop.h"
