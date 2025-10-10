#include "tsduck/container/TSPacketQueue.h"
#include "base/Placement.h"

using namespace video;
using namespace std;

void video::TSPacketQueue::SendPacket(ts::TSPacket *packet)
{
	if (_flushed)
	{
		throw std::runtime_error("已经冲洗了，禁止再送入包");
	}

	if (packet == nullptr)
	{
		_flushed = true;
		return;
	}

	_packet_queue.Enqueue(*packet);
}

ITSPacketSource::ReadPacketResult video::TSPacketQueue::ReadPacket(ts::TSPacket &packet)
{
	base::Placement<ts::TSPacket> packet_placement;
	_packet_queue.TryDequeue(packet_placement);
	if (packet_placement.Available())
	{
		packet = packet_placement.Object();
		return ITSPacketSource::ReadPacketResult::Success;
	}

	// 退队失败
	if (_flushed)
	{
		return ITSPacketSource::ReadPacketResult::NoMorePacket;
	}

	return ITSPacketSource::ReadPacketResult::NeedMoreInput;
}
