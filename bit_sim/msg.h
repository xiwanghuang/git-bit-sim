#pragma once

#include <iostream>
#include <map>
#include <vector>
#include "fsm.h"
#include "bittorrent.h"

class BasePeer;

enum class MsgType
{
	NOTIFY_QUERY_NEIGHBOR_LIST = 1,
	NOTIFY_QUERY_NEIGHBOR_FINISH,
	NOTIFY_INTEREST,
	NOTIFY_INTEREST_FINISH,
	NOTIFY_CHOKE,
	NOTIFY_UNCHOKE,
	NOTIFY_HANDSHAKE,
	NOTIFY_HANDSHAKE_FINISH,
	NOTIFY_DOWNLOAD_REQUEST,
	NOTIFY_DOWNLOAD_REQUEST_CONFIRM,
	NOTIFY_DOWNLOAD_REFUSE,
	NOTIFY_DATA_REFUSE,
	DATA
};



class Msg
{
public:
	BasePeer* m_src;
	BasePeer* m_dst;
	MsgType m_msgType;
	int m_timeCost;
	int m_hash;
	int block_idx;
	Msg(){;}
	Msg(BasePeer* src, BasePeer* dst, MsgType msgType, int cost);
	Msg(BasePeer* src, BasePeer* dst, MsgType msgType);
	Msg(const Msg& msg);
};

class MsgScheduler
{
public:
	std::vector<Msg> m_msgList;
	std::vector<Msg> m_tempMsgList;

	static MsgScheduler* getScheduler();
	std::vector<int> m_msgTypeNums;
	int step();
	int m_interval;
	~MsgScheduler() { ; }
	int addMsg(Msg msg);
	void printNums();
private:
	static MsgScheduler* m_scheduler;
	MsgScheduler(int interval) :m_interval(interval) {}
};


