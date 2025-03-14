//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAbstractDescriptorsTable.h"
#include "tsBinaryTable.h"
#include "tsPSIBuffer.h"
#include "tsSection.h"

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::AbstractDescriptorsTable::AbstractDescriptorsTable(TID tid_, UChar const *xml_name, Standards standards, uint16_t tid_ext_, uint8_t version_, bool is_current_)
	: AbstractLongTable(tid_, xml_name, standards, version_, is_current_),
	  descs(this),
	  _tid_ext(tid_ext_)
{
}

ts::AbstractDescriptorsTable::AbstractDescriptorsTable(ts::AbstractDescriptorsTable const &other)
	: AbstractLongTable(other),
	  descs(this, other.descs),
	  _tid_ext(other._tid_ext)
{
}

ts::AbstractDescriptorsTable::AbstractDescriptorsTable(DuckContext &duck, TID tid, UChar const *xml_name, Standards standards, BinaryTable const &table)
	: AbstractLongTable(tid, xml_name, standards, 0, true),
	  descs(this)
{
	deserialize(duck, table);
}

//----------------------------------------------------------------------------
// Get the table id extension.
//----------------------------------------------------------------------------

uint16_t ts::AbstractDescriptorsTable::tableIdExtension() const
{
	return _tid_ext;
}

//----------------------------------------------------------------------------
// Clear the content of the table.
//----------------------------------------------------------------------------

void ts::AbstractDescriptorsTable::clearContent()
{
	descs.clear();
	_tid_ext = 0xFFFF;
}

//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::AbstractDescriptorsTable::deserializePayload(PSIBuffer &buf, Section const &section)
{
	_tid_ext = section.tableIdExtension();
	buf.getDescriptorList(descs);
}

//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::AbstractDescriptorsTable::serializePayload(BinaryTable &table, PSIBuffer &buf) const
{
	size_t start = 0;
	while (!buf.error() && start < descs.size())
	{
		start = buf.putPartialDescriptorList(descs, start);
		addOneSection(table, buf);
	}
}
