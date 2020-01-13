#include "msg.h"
#include "global.h"
#include "bittorrent.h"

Msg::Msg(BasePeer* src, BasePeer* dst, MsgType msgType, int cost)
{
	m_src = src;
	m_dst = dst;
	m_msgType = msgType;
	m_timeCost = cost;
	m_hash = src->m_hash;
}

Msg::Msg(BasePeer * src, BasePeer * dst, MsgType msgType)
{
	m_src = src;
	m_dst = dst;
	m_msgType = msgType;
	m_hash = src->m_hash;
	m_timeCost = GlobalConfig::getDelay(m_src->m_cid, m_dst->m_cid);
}

Msg::Msg(const Msg & msg)
{
	m_src = msg.m_src;
	m_dst = msg.m_dst;
	m_msgType = msg.m_msgType;
	m_timeCost = msg.m_timeCost;
	m_hash = msg.m_hash;
}

MsgScheduler * MsgScheduler::getScheduler()
{
	if (m_scheduler == nullptr)
	{
		m_scheduler = new MsgScheduler(GlobalConfig::m_simStep);
		m_scheduler->m_msgTypeNums = std::vector<int>(int(MsgType::DATA));
		for (size_t i=0; i < int(MsgType::DATA); i++)
		{
			m_scheduler->m_msgTypeNums[i] = 0;
		}
		return m_scheduler;
	}
	else
	{
		return m_scheduler;
	}
}

int MsgScheduler::addMsg(Msg msg)
{
	m_msgTypeNums[int(msg.m_msgType)-1] += 1;
	m_tempMsgList.push_back(msg);
	return 0;
}

//遍历所有message，如果时间到了就将其移出事件列表
int MsgScheduler::step()
{
	std::vector<Msg>::iterator iter;
	for (iter = m_msgList.begin(); iter != m_msgList.end();)
	{
		(*iter).m_timeCost -= m_interval;
		if ((*iter).m_timeCost <= 0)
		{
			(*iter).m_dst->receive(&(*iter));
			iter = m_msgList.erase(iter);	//此处使用erase方法可以不用让iter++
		}
		else
		{
			iter++;
		}
	}
	for (iter = m_tempMsgList.begin(); iter != m_tempMsgList.end(); iter++)
	{
		m_msgList.push_back(*iter);			//添加事件至事件调度器中
	}
	m_tempMsgList.clear();
	m_tempMsgList.shrink_to_fit();
	return 0;
}


// 打印消息接收的数量
void MsgScheduler::printNums()
{

	for (size_t i =0; i <m_msgTypeNums.size(); i++)
	{
		std::cout << i << ":" << m_msgTypeNums[i] << " ";
	}
	std::cout << std::endl;
}

int MsgScheduler::step() 
{
	std::vector<Msg>::iterator iter;
	for ( iter = m_msgList.begin(); iter!=m_msgList.end() ;)
	{

	}
}
