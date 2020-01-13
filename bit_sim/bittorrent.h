#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "fsm.h"
#include "msg.h"

using namespace std;

class Peer;
class Tracker;
class Statistics;
enum class MsgType;

class BasePeer {
public:
	virtual int receive(Msg* msg) = 0;
	int m_cid;
	int m_hash;
};

class Tracker :public BasePeer {
public:
	int receive(Msg* msg);
	static Tracker* getTracker();
private:
	static Tracker* m_tracker;
	Tracker()
	{
		m_hash = 0;
	}
};

class Sender {
public:
	Peer* m_src;
	Peer* m_dst;
	float m_remainSize;
	float m_blockSize;
	int m_blockIdx;
	float m_rate;
	int m_interval;
	bool m_confirm;
	Sender() {}
	Sender(Peer* src, Peer* dst, int blockIdx, double fileSize, float rate, int stepLen);
	int step();
};

class Recver {
	Peer* m_src;
	Peer* m_dst;
	float m_remainSize;
	float m_blockSize;
	int m_blockIdx;
	float m_rate;
	int m_interval;
	bool m_confirm;
	float m_revcedSize;
	Recver() {}
	Recver(Peer* src, Peer* dst, int blockIdx, double fileSize, float rate, int stepLen);
	int step();
};

typedef void (Peer::*PeerCallBackFunc)(Msg* msg);

class Peer :public BasePeer 
{
public:
	std::vector<Peer*> m_neighborList;
	std::vector<Peer*> m_interestList;
	std::vector<Peer*> m_downloadList;
	std::vector<Peer*> m_uploadList;
	std::vector<Peer*> m_unchokeList;

	std::vector<BLOCKStatus> m_blockTable;

	std::map<MsgType, PeerCallBackFunc> m_actionMap;

	std::vector<int> m_downloadRate;
	std::vector<int> m_uploadRate;

	std::vector<std::vector<BLOCKStatus>*> m_neighborTableList;
	std::vector<int> m_rareBlockList;

	std::vector<Sender*> m_senderList;
	std::vector<Recver*> m_recverList;

	float m_upRemain;
	float m_downRemain;
	float m_perRate;

	double m_recvedSize;
	int m_timeCounter;
	
	bool m_isSeed;
	
	long int m_lastRefreshTimer;

private:
	std::vector<FSMStatus> m_interestFSMList;
	std::vector<FSMStatus> m_downloadFSMList;

	FSMStatus m_refreshNeighborFSM;
	FSMStatus m_shakeFSM;

	int m_shakeCounter;

public:
	~Peer() { ; }
	Peer() { ; }
	Peer(int idx, float upRemain, float downRemain, float perRate);
	int receive(Msg* msg);
	int step();

	void query_interest_callback(Msg* msg);
	void query_neighbor_callback(Msg* msg);
	void query_neighbor_finish_callback(Msg* msg);
	void query_choke_callback(Msg* msg);
	void query_unchoke_callback(Msg* msg);
	void query_handshake_callback(Msg* msg);
	void query_handshake_finish_callback(Msg* msg);
	void query_download_request_callback(Msg* msg);

	void as_seed();
	bool check_is_seed();
	int get_recver_size();
	int get_sender_size();

private:
	void refresh_neighbor_list();
	int get_recver_num_by_peer(Peer* peer);
	bool is_download_5th(Peer* peer);
	Recver* get_recver_num_by_msg(Msg* msg);
	Sender* get_sneder_num_by_msg(Msg* msg);

	std::vector<int> get_local_rarest_list();
	int get_i_need_block_num(int neighbotIdx);
	
	void refresh_local_rarest_list();
	void add_peer_to_download_list(Peer* peer);
	void minus_from_download_list(Peer* peer);
	void add_peer_to_upload_list(Peer* peer);
	void minus_from_upload_list(Peer* peer);
	void clear_neighbor_list();
};
