#include "tsduck/TsDuckToString.h"

std::string ToString(ts::TSPacket const &packet)
{
	base::Json json{
		{"PID", packet.getPID()},
		{"continuity_counter", packet.getCC()},
	};

#pragma region 时间信息

	class TimeInfos : base::IJsonSerializable
	{
	public:
		TimeInfos(ts::TSPacket const &packet)
			: _packet(packet)
		{
		}

	private:
		ts::TSPacket const &_packet;

	public:
		base::Json ToJson() const override
		{
			base::Json json;

			if (_packet.hasPCR())
			{
				json["PCR"] = _packet.getPCR();
				json["PCR to millisecond"] = ts::PCRToMilliSecond(_packet.getPCR());
			}

			if (_packet.hasPTS())
			{
				json["pts"] = _packet.getPTS();
			}

			if (_packet.hasDTS())
			{
				json["dts"] = _packet.getDTS();
			}

			return json;
		}
	};

	base::Json time_infos = TimeInfos{packet}.ToJson();
	if (!time_infos.is_null())
	{
		json["time infos"] = time_infos;
	}
#pragma endregion

	if (packet.getPUSI())
	{
		json["payload_unit_start_indicator"] = true;
	}

	return json.dump(4);
}

std::string ToString(ts::PAT const &pat)
{
	base::Json json{
		{"table_name", "PAT"},
		{"ts_id", pat.ts_id},
		{"nit_pid", pat.nit_pid},
		{"version", pat.version},
	};

	for (auto const &pmt : pat.pmts)
	{
		base::Json pmt_json{
			{"service_id", pmt.first},
			{"pmt_pid", pmt.second},
		};
		json["pmts"].push_back(pmt_json);
	}

	return json.dump(4);
}

std::string ToString(ts::PMT const &pmt)
{
	base::Json json{
		{"table_name", "PMT"},
		{"service_id", pmt.service_id},
		{"pcr_pid", pmt.pcr_pid},
		{"version", pmt.version},
	};
	for (auto &stream : pmt.streams)
	{
		base::Json stream_json{
			{"pid", stream.first}};
		json["streams"].push_back(stream_json);
	}

	return json.dump(4);
}

std::string ToString(ts::SDT const &sdt, ts::DuckContext &duck)
{
	base::Json json{
		{"table_name", "SDT"},
		{"service_count", sdt.services.size()},
		{"version", sdt.version},
	};

	for (auto &service : sdt.services)
	{
		base::Json services_json{
			{"service_name", service.second.serviceName(duck).c_str()},
			{"provider_name", service.second.providerName(duck).c_str()},
		};
		json["services"].push_back(services_json);
	}

	return json.dump(4);
}
