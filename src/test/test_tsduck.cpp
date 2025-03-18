#include "test_tsduck.h"
#include <base/file/IFileStream.h>
#include <base/task/CancellationTokenSource.h>
#include <tsCerrReport.h>
#include <tsduck/io/TSPacketStreamReader.h>
#include <tsduck/mux/JoinedTsStream.h>
#include <tsduck/TestProgramMux.h>

void test_tsduck()
{
	// 输入文件
	base::Queue<std::string> file_queue;
	file_queue.Enqueue("不老梦.ts");
	file_queue.Enqueue("水龙吟.ts");
	file_queue.Enqueue("idol.ts");

	video::JoinedTsStream joined_ts_stream;
	joined_ts_stream._on_ts_packet_source_list_exhausted = [&]()
	{
		std::string file_name;
		if (!file_queue.TryDequeue(file_name))
		{
			return;
		}

		shared_ptr<base::IFileStream> input_file_stream = base::file::OpenExisting(file_name.c_str());
		shared_ptr<video::TSPacketStreamReader> ts_packet_reader{new video::TSPacketStreamReader{input_file_stream}};
		joined_ts_stream.AddSource(ts_packet_reader);
	};

	shared_ptr<video::TestProgramMux> test_program_mux{new video::TestProgramMux{}};
	base::CancellationTokenSource cancel_pump_source;
	video::ITSPacketSource::ReadPacketResult pump_result = joined_ts_stream.PumpTo(test_program_mux, cancel_pump_source.Token());
	switch (pump_result)
	{
	case video::ITSPacketSource::ReadPacketResult::NoMorePacket:
		{
			std::cout << "读取完成，没有更多包了" << std::endl;
			break;
		}
	default:
		{
			std::cout << "其他错误" << std::endl;
			break;
		}
	}
}
