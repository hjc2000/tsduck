#pragma once
#include "base/math/Counter.h"
#include <map>
#include <memory>
#include <tsduck/interface/ITSPacketConsumer.h>
#include <tsduck/PipeTsPacketSource.h>
#include <tsTSPacket.h>

using std::shared_ptr;

namespace video
{
	/// <summary>
	///		连续性计数校正器。输入的包的连续性计数会被修改，让它变成连续的。
	///
	///		* 如果某个包有非连续指示，则会将本对象内部的计数器的值设置为该包的计数值。
	/// </summary>
	class CCCorrector : public ITSPacketConsumer, public PipeTsPacketSource
	{
	private:
		/// <summary>
		///		键=PID，值=Counter。因为每个 PID 各自维护连续性计数，毫不相干。
		/// </summary>
		std::map<uint16_t, shared_ptr<base::Counter<uint8_t>>> _counter_map;

		/// <summary>
		///		更正连续性计数。
		/// </summary>
		/// <param name="packet"></param>
		void CorrectCC(ts::TSPacket &packet);

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;
	};
} // namespace video
