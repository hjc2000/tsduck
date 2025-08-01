#pragma once
#include <base/container/List.h>
#include <base/string/define.h>
#include <memory>
#include <tsduck/interface/ITSPacketConsumer.h>
#include <tsTSPacket.h>

using std::shared_ptr;

namespace video
{
	class IPipeTsPacketSource
	{
	public:
		virtual ~IPipeTsPacketSource() = default;

		/// <summary>
		///		添加一个包消费者
		/// </summary>
		/// <param name="packet_comsumer"></param>
		virtual void AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) = 0;

		/// <summary>
		///		移除指定的包消费者。
		/// </summary>
		/// <param name="packet_comsumer"></param>
		/// <returns>容器中存在该消费者且移除成功则返回 true，否则返回 false。</returns>
		virtual bool RemovePacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) = 0;

		/// <summary>
		///		清空所有消费者。
		/// </summary>
		virtual void ClearConsumers() = 0;
	};

	class PipeTsPacketSource : public IPipeTsPacketSource
	{
	protected:
		base::List<shared_ptr<ITSPacketConsumer>> _consumer_list;

		void SendPacketToEachConsumer(ts::TSPacket *packet);
		void SendPacketToEachConsumer(std::vector<ts::TSPacket> packets);
		void SendPacketToEachConsumer(std::vector<std::vector<ts::TSPacket>> packet_vectors);

	public:
		virtual void AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		virtual bool RemovePacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		virtual void ClearConsumers() override;

	public:
		void AddTsPacketConsumerFromAnother(PipeTsPacketSource &another);
	};
} // namespace video
