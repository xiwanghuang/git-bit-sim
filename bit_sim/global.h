#pragma once

#include <vector>
#include "bittorrent.h"
#include "fsm.h"

class Tracker;
class Peer;

class GlobalConfig {
public:
	//全局变量
	static int m_peerNum;		//Peer节点的数量
	static double m_fileSize;		//文件大小
	static int m_blockSize;	//每个block大小
	static int m_perRate;		//平均传输速率
	static double m_blockNum;		//block数量
	static int m_simStep;		//模拟步长
	static int m_neighborNum;	//邻居个数
	static int m_neighborFreshPeriod;	//刷新邻居列表周期
	static int m_interestCheckPeriod;	//刷新unchoke列表周期
	static float m_maxUpload;			//上行带宽
	static float m_maxDownload;		//下行带宽
	
	static std::vector<std::vector<int>> m_allDelay;

	Tracker* m_tracker;
	Peer*	 m_seed;

	static std::vector<Peer* > m_allPeers;
	static int unchoke_num;
	static std::vector<float> getUpBW();
	static std::vector<float> getDownBW();
	
	void initGlobalData(int peerNum, double fileSize);
	void initAllPeers();
	void freeAllPeers();
	void initDelay();
	int getDelay(Peer* src, Peer* dst);
	int getDelay(int i, int j);
	void debug();

};

class Statistics
{
public:
	static std::vector<double> m_peerRecvedSize;
	static std::vector<std::vector<BLOCKStatus>> m_peerBlockTable;
	static std::vector<double> m_recvedNum;
	static int recver_num;
	static int timeCounter;	//计时器
	static std::vector<std::vector<float>> m_peerSizeArr;
	static std::vector<int> m_msgNum;
	static std::vector<int> m_bandwidth;
	static std::vector<float> m_throughPut;

	void initDate();
	void statData();
	void printData();
	void saveData(double fileSize, int PeerNum);
};

