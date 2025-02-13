#pragma once
#include <base/container/Queue.h>
#include <tsduck/interface/ITSPacketConsumer.h>
#include <tsduck/interface/ITSPacketSource.h>

namespace video
{
	class TSPacketQueue :
		public ITSPacketConsumer,
		public ITSPacketSource
	{
	private:
		base::Queue<ts::TSPacket> _packet_queue;
		bool _flushed = false;

	public:
		using ITSPacketConsumer::SendPacket;
		/// <summary>
		///		送入包。
		///		送入空指针冲洗内部队列。冲洗后继续送入包会抛出 InvalidOperationException。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(ts::TSPacket *packet) override;

		/// <summary>
		///		读取包。
		/// </summary>
		/// <param name="packet"></param>
		/// <returns>
		///		内部队列为空时，如果已经冲洗过了，会返回 ITSPacketSource::ReadPacketResult::NoMorePacket，
		///		否则返回 ITSPacketSource::ReadPacketResult::NeedMoreInput。
		/// </returns>
		ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) override;
	};
} // namespace video
