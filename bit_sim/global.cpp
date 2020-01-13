#include "global.h"
#include "bittorrent.h"
#include "msg.h"
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>

// 全局变量配置
int GlobalConfig::m_peerNum = 2000;
double GlobalConfig::m_fileSize = 200.0 * 1024;	//KB
float GlobalConfig::m_blockSize = 256;
double GlobalConfig::m_blockNum = 0; //(int)(GlobalConfig::m_filesize/GlobalConfig::m_blocksize);

// 带宽
float GlobalConfig::m_maxUpload = 1000;
float GlobalConfig::m_maxDownload = 3000;

float GlobalConfig::m_minUpload = 384;
float GlobalConfig::m_minDownload = 1500;

int GlobalConfig::m_simStep = 500; //ms
int GlobalConfig::m_neighborNum = 60;
int GlobalConfig::m_neighborFreshPeriod = 30;
int GlobalConfig::m_interestCheckPeriod = 10;
int GlobalConfig::unchoke_num = 5;

std::vector< std::vector<int> > GlobalConfig::m_allDelay = std::vector< std::vector<int> >((int)GlobalConfig::m_peerNum);

// 全局对象
Tracker* GlobalConfig::m_tracker = nullptr;
Peer* GlobalConfig::m_seed = nullptr;

std::vector<Peer*> GlobalConfig::m_allPeers = std::vector<Peer*>((int)GlobalConfig::m_peerNum);

void GlobalConfig::initGlobalData(double filesize, int peerNum)
{
	GlobalConfig::m_peerNum = peerNum;
	GlobalConfig::m_fileSize = filesize * 1024;		//KB
	GlobalConfig::m_blockSize = 256;	//KB
	GlobalConfig::m_perRate
}