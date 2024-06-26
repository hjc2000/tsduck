#pragma once
#include<tsduck/interface/ITSPacketConsumer.h>
#include<tsduck/PipeTsPacketSource.h>
#include<tsduck/TableOperator.h>
#include<tsduck/handler/TableHandler.h>

namespace video
{
	/// <summary>
	///		将多节目的 ts 变成单节目的 ts。只保留其中一个节目，其它节目都将被过滤掉。
	/// </summary>
	class MptsToSpts :
		public ITSPacketConsumer,
		public PipeTsPacketSource,
		public TableHandler
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="service_id">想要保留的节目的 service_id。其他节目都将被过滤掉。</param>
		MptsToSpts(uint16_t service_id);

	private:
		uint16_t _service_id = 0;
		uint16_t _pmt_pid = 0;

		/// <summary>
		///		原始 PAT 指示的所有 PMT PID 都记录进来。
		/// </summary>
		ts::PIDSet _pmt_pid_set;

		// 通过 TableHandler 继承
		void HandlePAT(ts::BinaryTable const &table) override;
		void HandleSDT(ts::BinaryTable const &table) override;

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;
	};
}