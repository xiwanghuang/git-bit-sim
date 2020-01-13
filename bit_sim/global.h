#pragma once

#include <vector>
#include "bittorrent.h"
#include "fsm.h"

class Tracker;
class Peer;

class GlobalConfig {
public:
	//ȫ�ֱ���
	static int m_peerNum;		//Peer�ڵ������
	static double m_fileSize;		//�ļ���С
	static float m_blockSize;	//ÿ��block��С
	static int m_perRate;		//ƽ����������
	static double m_blockNum;		//block����
	static int m_simStep;		//ģ�ⲽ��
	static int m_neighborNum;	//�ھӸ���
	static int m_neighborFreshPeriod;	//ˢ���ھ��б�����
	static int m_interestCheckPeriod;	//ˢ��unchoke�б�����
	static float m_maxUpload;			//���д���
	static float m_maxDownload;		//���д���
	
	static float m_minUpload;			//���д���
	static float m_minDownload;		//���д���

	static std::vector<std::vector<int>> m_allDelay;

	static Tracker* m_tracker;
	static Peer*	 m_seed;

	static std::vector<Peer* > m_allPeers;
	static int unchoke_num;
	static std::vector<float> getUpBW();
	static std::vector<float> getDownBW();
	
	void initGlobalData(double fileSize, int peerNum);
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
	static int timeCounter;	//��ʱ��
	static std::vector<std::vector<float>> m_peerSizeArr;
	static std::vector<int> m_msgNum;
	static std::vector<int> m_bandwidth;
	static std::vector<float> m_throughPut;

	void initData();
	void statData();
	void printData();
	void saveData(double fileSize, int PeerNum);
};

