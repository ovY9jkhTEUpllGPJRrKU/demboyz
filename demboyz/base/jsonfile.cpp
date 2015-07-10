
#include "jsonfile.h"
#include <cstdlib>

#define CBASE64_IMPLEMENTATION
#include "cbase64/cbase64.h"

namespace base
{
    JsonWriterFile::JsonWriterFile(FILE* fp, char* buffer, std::size_t length):
        m_fileStream(fp, buffer, length),
        m_writer(m_fileStream),
        m_fp(fp)
    {
    }

    JsonWriterFile::~JsonWriterFile()
    {
        Flush();
    }

    FILE* JsonWriterFile::GetFp() const
    {
        return m_fp;
    }

    void JsonWriterFile::Flush()
    {
        m_fileStream.Flush();
        fflush(m_fp);
    }

    void JsonWriterFile::Reset()
    {
        m_writer.Reset(m_fileStream);
    }

    bool JsonWriterFile::IsComplete() const
    {
        return m_writer.IsComplete();
    }

    void JsonWriterFile::StartObject(const char* name /*= nullptr*/)
    {
        auto& writer = m_writer;
        if (name)
        {
            writer.String(name);
        }
        writer.StartObject();
    }

    void JsonWriterFile::EndObject()
    {
        m_writer.EndObject();
    }

    void JsonWriterFile::StartArray(const char* name /*= nullptr*/)
    {
        auto& writer = m_writer;
        if (name)
        {
            writer.String(name);
        }
        writer.StartArray();
    }

    void JsonWriterFile::EndArray()
    {
        m_writer.EndArray();
    }

    void JsonWriterFile::WriteNull(const char* name)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Null();
    }

    void JsonWriterFile::WriteBool(const char* name, bool value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Bool(value);
    }

    void JsonWriterFile::WriteInt32(const char* name, std::int32_t value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Int(value);
    }

    void JsonWriterFile::WriteInt32(const char* name, std::int32_t value, bool writeCondition)
    {
        auto& writer = m_writer;
        writer.String(name);
        if (writeCondition)
        {
            writer.Int(value);
        }
        else
        {
            writer.Null();
        }
    }

    void JsonWriterFile::WriteInt64(const char* name, std::int64_t value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Int64(value);
    }

    void JsonWriterFile::WriteUInt32(const char* name, std::uint32_t value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Uint(value);
    }

    void JsonWriterFile::WriteUInt32(const char* name, std::uint32_t value, bool writeCondition)
    {
        auto& writer = m_writer;
        writer.String(name);
        if (writeCondition)
        {
            writer.Uint(value);
        }
        else
        {
            writer.Null();
        }
    }

    void JsonWriterFile::WriteUint64(const char* name, std::uint64_t value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Uint64(value);
    }

    void JsonWriterFile::WriteString(const char* name, const char* value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.String(value);
    }

    void JsonWriterFile::WriteString(const char* name, const char* value, std::int32_t length)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.String(value, length);
    }

    void JsonWriterFile::WriteFloat(const char* name, const double value)
    {
        auto& writer = m_writer;
        writer.String(name);
        writer.Double(value);
    }

    void JsonWriterFile::WriteBits(const char* name, const unsigned char* data, std::size_t numBits)
    {
        auto& writer = m_writer;
        writer.String(name);

        cbase64_encodestate state;
        cbase64_init_encodestate(&state);

        const std::size_t numBytes = ((numBits + 7) >> 3);
        const int encodedLength = cbase64_calc_encoded_length(numBytes);

        char* const encoded = (char*)malloc(encodedLength);
        char* encodedCurOut = encoded;

        const std::size_t numTrailingBits = (numBits & 7);
        if (numTrailingBits > 0)
        {
            const std::size_t numBytesWithoutBits = (numBits >> 3);
            encodedCurOut += cbase64_encode_block(data, numBytesWithoutBits, encodedCurOut, &state);
            const unsigned char lastByteClean = data[numBytesWithoutBits] & ~(0xFF >> numTrailingBits);
            encodedCurOut += cbase64_encode_block(&lastByteClean, 1, encodedCurOut, &state);
        }
        else
        {
            encodedCurOut += cbase64_encode_block(data, numBytes, encodedCurOut, &state);
        }
        encodedCurOut += cbase64_encode_blockend(encodedCurOut, &state);

        writer.String(encoded, encodedCurOut - encoded);
        free(encoded);
        return;
    }

    void JsonWriterFile::WriteBytes(const char* name, const unsigned char* data, std::size_t numBytes)
    {
        JsonWriterFile::WriteBits(name, data, numBytes * 8);
    }
}
