#include "bittorrent.h"
#include "global.h"
#include "fsm.h"
#include <algorithm>

Tracker* Tracker::m_tracker = nullptr;

Tracker* Tracker::getTracker()
{
	if (m_tracker == nullptr)
	{
		m_tracker = new Tracker();
	}
	return m_tracker;
}


 int Tracker::receive(Msg* msg)
{
	 Msg to_msg(this, msg->m_src, MsgType::NOTIFY_QUERY_NEIGHBOR_FINISH, getDelay(m_cid), msg->m_src->m_cid);
	 to_msg.m_hash = msg->m_hash;
	 MsgScheduler::getScheduler()->addMsg(to_msg);
	return 0;
}

Sender::Sender(Peer * src, Peer * dst, int blockIdx, double fileSize, float rate, int stepLen)
{
	m_src = src;
	m_dst = dst;
	m_blockIdx = blockIdx;
	m_remainSize = fileSize;
	m_rate = rate;
	m_interval = stepLen;
	m_confirm = false;
	if (m_src->m_upRemain >= m_rate)
	{
		m_src->m_upRemain -= m_rate;
	}
}

int Sender::step()
{
	m_remainSize -= m_rate / 1000 / 8 * m_interval;
	if (m_remainSize <= 0)
	{
		m_src->m_upRemain += m_rate;
		return -1;
	}
	return 0;
}

Recver::Recver(Peer* src, Peer* dst, int blockIdx, double fileSize, float rate, int stepLen)
{
	m_src = src;
	m_dst = dst;
	m_blockIdx = blockIdx;
	m_remainSize = fileSize;
	m_rate = rate;
	m_interval = stepLen;
	m_confirm = false;
	m_revcedSize = 0;
	dst->m_blockTable[m_blockIdx] = BLOCKStatus::BLOCK_RECVING;
	if ( m_dst->m_downRemain >= m_rate )
	{
		m_dst->m_downRemain -= m_rate;
	}
}

int Recver::step() 
{
	float plus = (m_rate / 8)*(m_interval / 1000);		//将文件大小转换成传输时间
	if (m_revcedSize + plus > m_blockSize)
	{
		plus = m_blockSize - m_revcedSize;
	}
	m_dst->m_recvedSize += plus;
	m_revcedSize += plus;
	if (m_revcedSize>=m_blockSize)
	{
		m_dst->m_blockTable[m_blockIdx] = BLOCKStatus::BLOCK_RECVED;
		m_dst->m_downRemain += m_rate;
		return -1;
	}
	return 0;
}

Peer::Peer(int idx, float upRemain, float downRemain, float perRate)
{
	m_cid = idx;
	m_upRemain = upRemain;
	m_downRemain = downRemain;
	m_perRate = perRate;
	m_blockTable = std::vector<BLOCKStatus>(GlobalConfig::m_blockNum);
	m_hash = rand();
	m_recvedSize = 0.0;
	m_timeCounter = 0;
	m_lastRefreshTimer = m_timeCounter;
	m_isSeed = false;
	for (size_t i = 0; i < GlobalConfig::m_blockNum; i++)
	{
		m_blockTable[i] = BLOCKStatus::BLOCK_EMPTY;
	}
	m_refreshNeighborFSM = FSMStatus::FSM_EMPTY;
	m_shakeFSM = FSMStatus::FSM_EMPTY;

	m_actionMap[MsgType::NOTIFY_QUERY_NEIGHBOR_LIST] = &Peer::query_neighbor_callback;
	m_actionMap[MsgType::NOTIFY_UNCHOKE] = &Peer::query_unchoke_callback;
	m_actionMap[MsgType::NOTIFY_CHOKE] = &Peer::query_choke_callback;
	m_actionMap[MsgType::NOTIFY_QUERY_NEIGHBOR_FINISH] = &Peer::query_neighbor_finish_callback;
	m_actionMap[MsgType::NOTIFY_DOWNLOAD_REQUEST] = &Peer::query_download_request_callback;
	m_actionMap[MsgType::NOTIFY_HANDSHAKE] = &Peer::query_handshake_callback;
	m_actionMap[MsgType::NOTIFY_HANDSHAKE_FINISH] = &Peer::query_handshake_finish_callback;
	//
}

int Peer::receive(Msg * msg)
{
	PeerCallBackFunc pmf = m_actionMap[msg->m_msgType];
	(this->*pmf)(msg);
	return 0;
}

int Peer::get_sender_size()
{
	return m_senderList.size();
}

int Peer::get_recver_size()
{
	return m_recverList.size();
}

bool Peer::check_is_seed()
{
	if (m_isSeed)
	{
		return true;
	}
	for (size_t i =0; i < GlobalConfig::m_blockNum; i++)
	{
		if (m_blockTable[i] != BLOCKStatus::BLOCK_RECVED)
		{
			return false;
		}
	}
	return true;
}

void Peer::add_peer_to_upload_list(Peer* peer)
{
	for (size_t i = 0; i < m_uploadList.size(); i++)
	{
		if (m_uploadList[i] == peer)
		{
			//如果是已有的节点那么就将其提供的速度增加1
			m_uploadRate[i] += 1;
			return;
		}
	}

	m_uploadList.push_back(peer);
	m_uploadRate.push_back(1);
}

void Peer::add_peer_to_download_list(Peer* peer)
{
	for (size_t i = 0; i < m_downloadList.size(); i++)
	{
		if (m_downloadList[i] == peer)
		{
			//如果是已有的节点那么就将其提供的速度增加1
			m_downloadRate[i] += 1;
			return;
		}
	}

	m_downloadList.push_back(peer);
	m_downloadRate.push_back(1);
}

void Peer::minus_from_upload_list(Peer* peer)
{
	for (size_t i = 0; i < m_uploadList.size(); i++)
	{
		if (m_uploadList[i] == peer)
		{
			if (m_uploadRate[i] > 1)
			{
				m_uploadRate[i] -= 1;
			}
			else
			{
				m_uploadList.erase(m_uploadList.begin() + i);
				m_uploadRate.erase(m_uploadRate.begin() + i);
			}
			return;
		}
	}
}

void Peer::minus_from_download_list(Peer* peer)
{
	for (size_t i = 0; i < m_downloadList.size(); i++)
	{
		if (m_downloadList[i] == peer)
		{
			if (m_downloadRate[i] > 1)
			{
				m_downloadRate[i] -= 1;
			}
			else
			{
				m_downloadList.erase(m_uploadList.begin() + i);
				m_downloadRate.erase(m_uploadRate.begin() + i);
			}
			return;
		}
	}
}


void Peer::clear_neighbor_list()
{
	std::vector<Peer*>::iterator it;
	int m = 0;
	for (it = m_neighborList.begin(); it != m_neighborList.end();)
	{
		//如果对方没有需要的block，则将其从neighborlist中删除
		bool sameFlag = true;
		for (size_t i = 0; i < GlobalConfig::m_blockNum; i++)
		{
			if (m_blockTable[i] == BLOCKStatus::BLOCK_EMPTY &&
				(*m_neighborTableList[m])[i] == BLOCKStatus::BLOCK_RECVED)
			{
				//只要有一个block是我所需要的就不清理
				sameFlag = false;
				break;
			}
		}

		if (sameFlag)
		{
			m_interestFSMList.erase(m_interestFSMList.begin() + m);
			m_downloadFSMList.erase(m_downloadFSMList.begin() + m);

			delete m_neighborTableList[m];

			m_neighborTableList.erase(m_neighborTableList.begin() + m);

			it = m_neighborList.erase(it);
		}
		else
		{
			it++;
			m++;
		}
	}
}