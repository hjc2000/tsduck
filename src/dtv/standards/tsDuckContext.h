//-------------------       ---------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  TSDuck execution context containing current preferences.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsByteBlock.h"
#include "tsCharset.h"
#include "tsPSI.h"
#include "tsStandards.h"
#include "tsUString.h"

namespace ts
{

	class HFBand;
	class Report;
	class Args;

	//!
	//! TSDuck execution context containing current preferences.
	//! @ingroup app
	//!
	//! An instance of this class contains specific contextual information
	//! for the execution of TSDuck. This context contains either user's
	//! preferences and accumulated contextual information.
	//!
	//! Context information include:
	//! - Report for log and error messages.
	//! - Text output stream.
	//! - Default character sets (input and output).
	//! - Default CAS id.
	//! - Default Private Data Specifier (PDS) for DVB private descriptors.
	//! - Accumulated standards from the signalization (MPEG, DVB, ATSC, etc.)
	//! - Default region for UHF and VHF frequency layout.
	//!
	//! Support is included to define and analyze command line options which
	//! define values for the environment.
	//!
	//! Unlike DuckConfigFile, this class is not a singleton. More than
	//! one context is allowed in the same process as long as the various
	//! instances of classes which use DuckContext use only one context at
	//! a time. For instance, inside a @e tsp or @e tsswitch process, each
	//! plugin can use its own context, using different preferences.
	//!
	//! The class DuckContext is not thread-safe. It shall be used from one
	//! single thread or explicit synchronization is required.
	//!
	class TSDUCKDLL DuckContext
	{
		TS_NOCOPY(DuckContext);

	public:
		//!
		//! Constructor.
		//! @param [in] report Address of the report for log and error messages. If null, use the standard error.
		//! @param [in] output The output stream to use, @c std::cout on null pointer.
		//!
		DuckContext(Report *report = nullptr, std::ostream *output = nullptr);

		//!
		//! Reset the TSDuck context to initial configuration.
		//!
		void reset();

		//!
		//! Get the current report for log and error messages.
		//! @return A reference to the current output report.
		//!
		Report &report() const
		{
			return *_report;
		}

		//!
		//! Set a new report for log and error messages.
		//! @param [in] report Address of the report for log and error messages. If null, use the standard error.
		//!
		void setReport(Report *report);

		//!
		//! Get the current output stream to issue long text output.
		//! @return A reference to the output stream.
		//!
		std::ostream &out() const
		{
			return *_out;
		}

		//!
		//! Redirect the output stream to a file.
		//! @param [in] fileName The file name to create. If empty or equal to "-", reset to @c std::cout.
		//! @param [in] override It true, the previous file is closed. If false and the
		//! output is already redirected outside @c std::cout, do nothing.
		//! @return True on success, false on error.
		//!
		bool setOutput(fs::path const &fileName, bool override = true);

		//!
		//! Redirect the output stream to a stream.
		//! @param [in] output The output stream to use, @c std::cout on null pointer.
		//! @param [in] override It true, the previous file is closed. If false and the
		//! output is already redirected outside @c std::cout, do nothing.
		//!
		void setOutput(std::ostream *output, bool override = true);

		//!
		//! Check if output was redirected.
		//! @return True if the current output is neither the initial one nor the standard output.
		//!
		bool redirectedOutput() const
		{
			return _out != &std::cout && _out != _initial_out;
		}

		//!
		//! Flush the text output.
		//!
		void flush();

		//!
		//! Get the default input character set for strings from tables and descriptors.
		//! The default is the DVB superset of ISO/IEC 6937 as defined in ETSI EN 300 468.
		//! Other defaults can be used in non-DVB contexts or when a DVB operator uses an incorrect
		//! signalization, assuming another default character set (usually from its own country).
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The default input character set (never null).
		//!
		Charset const *charsetIn(Charset const *charset = nullptr) const
		{
			return charset != nullptr ? charset : _charsetIn;
		}

		//!
		//! Get the preferred output character set for strings to insert in tables and descriptors.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The preferred output character set (never null).
		//!
		Charset const *charsetOut(Charset const *charset = nullptr) const
		{
			return charset != nullptr ? charset : _charsetOut;
		}

		//!
		//! Convert a signalization string into UTF-16 using the default input character set.
		//! @param [out] str Returned decoded string.
		//! @param [in] data Address of an encoded string.
		//! @param [in] size Size in bytes of the encoded string.
		//! @return True on success, false on error (truncated, unsupported format, etc.)
		//! @see ETSI EN 300 468, Annex A.
		//!
		bool decode(UString &str, uint8_t const *data, size_t size) const
		{
			return _charsetIn->decode(str, data, size);
		}

		//!
		//! Convert a signalization string into UTF-16 using the default input character set.
		//! @param [in] data Address of a string in in binary representation (DVB or similar).
		//! @param [in] size Size in bytes of the string.
		//! @return The equivalent UTF-16 string. Stop on untranslatable character, if any.
		//! @see ETSI EN 300 468, Annex A.
		//!
		UString decoded(uint8_t const *data, size_t size) const
		{
			return _charsetIn->decoded(data, size);
		}

		//!
		//! Convert a signalization string (preceded by its one-byte length) into UTF-16 using the default input character set.
		//! @param [out] str Returned decoded string.
		//! @param [in,out] data Address of an encoded string. The address is updated to point after the decoded value.
		//! @param [in,out] size Size of the buffer. Updated to remaining size.
		//! @return True on success, false on error (truncated, unsupported format, etc.)
		//! @see ETSI EN 300 468, Annex A.
		//!
		bool decodeWithByteLength(UString &str, uint8_t const *&data, size_t &size) const
		{
			return _charsetIn->decodeWithByteLength(str, data, size);
		}

		//!
		//! Convert a signalization string (preceded by its one-byte length) into UTF-16 using the default input character set.
		//! @param [in,out] data Address of a buffer containing a string to read.
		//! The first byte in the buffer is the length in bytes of the string.
		//! Upon return, @a buffer is updated to point after the end of the string.
		//! @param [in,out] size Size in bytes of the buffer, which may be larger than
		//! the DVB string. Upon return, @a size is updated, decremented by the same amount
		//! @a buffer was incremented.
		//! @return The equivalent UTF-16 string. Stop on untranslatable character, if any.
		//! @see ETSI EN 300 468, Annex A.
		//!
		UString decodedWithByteLength(uint8_t const *&data, size_t &size) const
		{
			return _charsetIn->decodedWithByteLength(data, size);
		}

		//!
		//! Encode an UTF-16 string into a signalization string using the preferred output character set.
		//! Stop either when this string is serialized or when the buffer is full, whichever comes first.
		//! @param [in,out] buffer Address of the buffer where the signalization string is written.
		//! The address is updated to point after the encoded value.
		//! @param [in,out] size Size of the buffer. Updated to remaining size.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @return The number of serialized characters (which is usually not the same as the number of written bytes).
		//!
		size_t encode(uint8_t *&buffer, size_t &size, UString const &str, size_t start = 0, size_t count = NPOS) const
		{
			return _charsetOut->encode(buffer, size, str, start, count);
		}

		//!
		//! Encode an UTF-16 string into a signalization string using the preferred output character set.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @return The DVB string.
		//!
		ByteBlock encoded(UString const &str, size_t start = 0, size_t count = NPOS) const
		{
			return _charsetOut->encoded(str, start, count);
		}

		//!
		//! Encode an UTF-16 string into a signalization string (preceded by its one-byte length) using the preferred output character set.
		//! Stop either when this string is serialized or when the buffer is full or when 255 bytes are written, whichever comes first.
		//! @param [in,out] buffer Address of the buffer where the DVB string is written.
		//! The first byte will receive the size in bytes of the DVB string.
		//! The address is updated to point after the encoded value.
		//! @param [in,out] size Size of the buffer. Updated to remaining size.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @return The number of serialized characters (which is usually not the same as the number of written bytes).
		//!
		size_t encodeWithByteLength(uint8_t *&buffer, size_t &size, UString const &str, size_t start = 0, size_t count = NPOS) const
		{
			return _charsetOut->encodeWithByteLength(buffer, size, str, start, count);
		}

		//!
		//! Encode an UTF-16 string into a signalization string (preceded by its one-byte length) using the preferred output character set.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @return The DVB string with the initial length byte.
		//!
		ByteBlock encodedWithByteLength(UString const &str, size_t start = 0, size_t count = NPOS) const
		{
			return _charsetOut->encodedWithByteLength(str, start, count);
		}

		//!
		//! Set the default input character set for strings.
		//! The default should be the DVB superset of ISO/IEC 6937 as defined in ETSI EN 300 468.
		//! Use another default in the context of an operator using an incorrect signalization,
		//! assuming another default character set (usually from its own country).
		//! @param [in] charset The new default input character set or a null pointer to revert
		//! to the default.
		//!
		void setDefaultCharsetIn(Charset const *charset);

		//!
		//! Set the preferred output character set for strings.
		//! @param [in] charset The new preferred output character set or a null pointer to revert
		//! to the default.
		//!
		void setDefaultCharsetOut(Charset const *charset);

		//!
		//! Set the default CAS id to use.
		//! @param [in] cas Default CAS id to be used when the CAS is unknown.
		//!
		void setDefaultCASId(uint16_t cas);

		//!
		//! The actual CAS id to use.
		//! @param [in] cas Proposed CAS id. If equal to CASID_NULL, then another value can be returned.
		//! @return The actual CAS id to use.
		//!
		uint16_t casId(uint16_t cas = CASID_NULL) const;

		//!
		//! Set the default private data specifier to use in the absence of explicit private_data_specifier_descriptor.
		//! @param [in] pds Default PDS. Use zero to revert to no default.
		//!
		void setDefaultPDS(PDS pds);

		//!
		//! The actual private data specifier to use.
		//! @param [in] pds Current PDS, typically from a private_data_specifier_descriptor.
		//! @return The actual PDS to use.
		//!
		PDS actualPDS(PDS pds) const;

		//!
		//! Add a registration id, as found in an MPEG-defined registration descriptor.
		//! @param [in] regid A new registration id.
		//!
		void addRegistrationId(uint32_t regid);

		//!
		//! Get the set of all registration ids, as found in MPEG-defined registration descriptors.
		//! @return A constant reference to the set of all registration ids.
		//!
		std::set<uint32_t> const &registrationIds() const
		{
			return _registrationIds;
		}

		//!
		//! Get the last registration id, as found in MPEG-defined registration descriptors.
		//! @return The last registration id or REGID_NULL if none was found.
		//!
		uint32_t lastRegistrationId() const
		{
			return _lastRegistrationId;
		}

		//!
		//! Clear all accumulated registration ids, as found in MPEG-defined registration descriptors.
		//!
		void resetRegistrationIds();

		//!
		//! Get the list of standards which are present in the transport stream or context.
		//! @return A bit mask of standards.
		//!
		Standards standards() const
		{
			return _accStandards;
		}

		//!
		//! Add a list of standards which are present in the transport stream or context.
		//! @param [in] mask A bit mask of standards.
		//!
		void addStandards(Standards mask);

		//!
		//! Reset the list of standards which are present in the transport stream or context.
		//! @param [in] mask A bit mask of standards.
		//!
		void resetStandards(Standards mask = Standards::NONE);

		//!
		//! Set the name of the default region for UVH and VHF band frequency layout.
		//! @param [in] region Name of the region. Use an empty string to revert to the default.
		//!
		void setDefaultHFRegion(UString const &region);

		//!
		//! Set the explicit inclusion of leap seconds where it is needed.
		//! Currently, this applies to SCTE 35 splice_schedule() commands only.
		//! @param [in] on True if leap seconds shall be explicitly included (the default), false to ignore leap seconds.
		//!
		void setUseLeapSeconds(bool on)
		{
			_useLeapSeconds = on;
		}

		//!
		//! Check the explicit inclusion of leap seconds where it is needed.
		//! @return True if leap seconds shall be explicitly included, false to ignore leap seconds.
		//!
		bool useLeapSeconds() const
		{
			return _useLeapSeconds;
		}

		//!
		//! An opaque class to save all command line options, as loaded by loadArgs().
		//!
		class TSDUCKDLL SavedArgs
		{
		public:
			//! Default constructor.
			SavedArgs();

		private:
			friend class DuckContext;
			int _definedCmdOptions;     // Defined command line options, indicate which fields are valid.
			Standards _cmdStandards;    // Forced standards from the command line.
			UString _charsetInName;     // Character set to interpret strings without prefix code.
			UString _charsetOutName;    // Preferred character set to generate strings.
			uint16_t _casId;            // Preferred CAS id.
			PDS _defaultPDS;            // Default PDS value if undefined.
			UString _hfDefaultRegion;   // Default region for UHF/VHF band.
			MilliSecond _timeReference; // Time reference in milli-seconds from UTC (used in ISDB variants).
		};

		//!
		//! Save all command line options, as loaded by loadArgs().
		//! @param [out] args Saved arguments.
		//!
		void saveArgs(SavedArgs &args) const;

		//!
		//! Restore all command line options, as loaded by loadArgs() in another DuckContext.
		//! @param [in] args Saved arguments to restore.
		//!
		void restoreArgs(SavedArgs const &args);

	private:
		Report *_report;                                           // Pointer to a report for error messages. Never null.
		std::ostream *_initial_out;                                // Initial text output stream. Never null.
		std::ostream *_out;                                        // Pointer to text output stream. Never null.
		std::ofstream _outFile{};                                  // Open stream when redirected to a file by name.
		Charset const *_charsetIn;                                 // DVB character set to interpret strings without prefix code.
		Charset const *_charsetOut;                                // Preferred DVB character set to generate strings.
		uint16_t _casId{CASID_NULL};                               // Preferred CAS id.
		PDS _defaultPDS = 0;                                       // Default PDS value if undefined.
		bool _useLeapSeconds = true;                               // Explicit use of leap seconds.
		Standards _cmdStandards{Standards::NONE};                  // Forced standards from the command line.
		Standards _accStandards{Standards::NONE};                  // Accumulated list of standards in the context.
		UString _hfDefaultRegion{};                                // Default region for UHF/VHF band.
		MilliSecond _timeReference = 0;                            // Time reference in milli-seconds from UTC (used in ISDB variants).
		UString _timeRefConfig{};                                  // Time reference name from TSDuck configuration file.
		int _definedCmdOptions = 0;                                // Mask of defined command line options (CMD_xxx below).
		uint32_t _lastRegistrationId = REGID_NULL;                 // Last encountered registration id in a registration descriptor.
		std::set<uint32_t> _registrationIds{};                     // Set of all registration ids.
		std::map<uint16_t, UChar const *> const _predefined_cas{}; // Predefined CAS names, index by CAS id (first in range).

		// List of command line options to define and analyze.
		enum CmdOptions
		{
			CMD_CHARSET = 0x0001,
			CMD_HF_REGION = 0x0002,
			CMD_STANDARDS = 0x0004,
			CMD_PDS = 0x0008,
			CMD_CAS = 0x0010,
			CMD_TIMEREF = 0x0020,
		};
	};
} // namespace ts
