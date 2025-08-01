#include "tsduck/corrector/CCCorrector.h"

void video::CCCorrector::CorrectCC(ts::TSPacket &packet)
{
	uint16_t pid = packet.getPID();
	if (_counter_map[pid] == nullptr)
	{
		// 此 PID 第一次送入包
		_counter_map[pid] = shared_ptr<base::Counter<uint8_t>>{new base::Counter<uint8_t>{0, 15}};
		_counter_map[pid]->SetCurrentValue(packet.getCC());
		return;
	}

	// 不是第一次送入包
	base::Counter<uint8_t> &counter = *_counter_map[pid];
	if (packet.getDiscontinuityIndicator())
	{
		// 存在不连续指示
		counter.SetCurrentValue(packet.getCC());
	}
	else
	{
		// 不存在不连续指示
		packet.setCC((uint8_t)++counter);
	}
}

void video::CCCorrector::SendPacket(ts::TSPacket *packet)
{
	CorrectCC(*packet);
	SendPacketToEachConsumer(packet);
}
