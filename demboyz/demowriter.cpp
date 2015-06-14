
#include "idemowriter.h"
#include "demotypes.h"
#include "demofile.h"
#include "netmessages/nethandlers.h"
#include "netmessages/netcontants.h"
#include "sourcesdk/bitbuf.h"
#include <cstdio>
#include <memory>

class DemoWriter: public IDemoWriter
{
public:
    DemoWriter(FILE* outputFp);

    virtual void StartWriting(demoheader_t& header) override final;
    virtual void EndWriting() override final;

    virtual void StartCommandPacket(CommandPacket& packet) override final;
    virtual void EndCommandPacket() override final;

    virtual void WriteNetPacket(NetPacket& packet, SourceGameContext& context) override final;

private:
    DemoFileWriter m_writer;
    bf_write m_cmdPacketBuf;
    std::unique_ptr<uint8_t[]> m_packetBuffer;
};

IDemoWriter* IDemoWriter::CreateDemoWriter(void* outputFp)
{
    return new DemoWriter(reinterpret_cast<FILE*>(outputFp));
}

DemoWriter::DemoWriter(FILE* outputFp):
    m_writer(outputFp),
    m_packetBuffer(new uint8_t[NET_MAX_PAYLOAD])
{
    m_cmdPacketBuf.StartWriting(m_packetBuffer.get(), NET_MAX_PAYLOAD);
}

void DemoWriter::StartWriting(demoheader_t& header)
{
    m_writer.WriteDemoHeader(header);
}

void DemoWriter::EndWriting()
{
}

void DemoWriter::StartCommandPacket(CommandPacket& packet)
{
    m_writer.WriteCmdHeader(packet.cmd, packet.tick);
    if (packet.cmd == dem_packet || packet.cmd == dem_signon)
    {
        m_writer.WriteCmdInfo(*packet.cmdInfo);
        m_writer.WriteSequenceInfo(packet.sequenceInfo1, packet.sequenceInfo2);
    }
    m_cmdPacketBuf.Reset();
}

void DemoWriter::EndCommandPacket()
{
    m_writer.WriteRawData(m_cmdPacketBuf.GetBasePointer(), m_cmdPacketBuf.GetNumBytesWritten());
}

void DemoWriter::WriteNetPacket(NetPacket& packet, SourceGameContext& context)
{
    NetHandlers::NetMsg_BitWrite(packet.type, m_cmdPacketBuf, context, packet.data);
}
