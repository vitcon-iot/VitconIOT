/**
 * Created by Park, Nil on 2017-04-26.
 * Last Revision: 2017-04-26
 *
 * 본 모듈은 Vitcon IOT 서버와의 통신을 담당하는 클래스이다.
 *
 * Park, Nil (2017-04-26): 소스코드 관리 시작
 * Park, Nil (2017-04-27): SetInterval, SetTimeout 함수를 public으로 변경
 */

#ifndef VITCON_BROKERCOMM_H_
#define VITCON_BROKERCOMM_H_

#include "VitconIOTItem.h"
#include <VitconCRC32.h>

#include <stdint.h>
#include <Arduino.h>

namespace vitcon
{
  class BrokerComm
  {
  public:
    static const int DEVICEID_LEN = 32;
    static const int RECV_BUFFER = 50;
    static const int CHAR_TIMEOUT = 50; // 수신 문자 간격에 대한 타임아웃
    static const uint8_t STATUS_ADVERTISE = 0; // 응답 패킷 수신 실패 (Interval 대기 상태)
    static const uint8_t STATUS_PACKETSENT = 1; // 명령 패킷 전송 완료
    static const uint8_t STATUS_PACKETRECV = 2; // 응답 패킷 수신 성공 (Interval 대기 상태)
    BrokerComm(Stream *stream, const char *device_id, IOTItem **items, uint8_t item_count);
    void Run();
    // Timing management function
    void SetInterval(uint32_t ms);
    void SetTimeout(uint32_t ms);
  private:
    // Communication Media
    Stream *mStream;

    // Device definitions
    const char *mDeviceId;
    IOTItem **mItems;
    uint8_t mItemCount;

    // Timing variables
    uint32_t mInterval; // 전송 인터벌
    uint32_t mLastSent;
    uint32_t mTimeout;
    uint32_t mLastRecv;

    // Packet: Device to Server
    uint32_t SendHeader(uint16_t len);
    void SendCRC(uint32_t crc);
    void Advertise();
    void SendData();

    // Packet: Server to Device
    uint8_t mStatus;
    
    // Variables for incoming data
    uint16_t mLen;
    uint16_t mLenInv;
    uint8_t mData[RECV_BUFFER];
    uint32_t mCrc32;
    
    // Variables for protocol
    int mIdxSig;
    int mIdxLen;
    int mIdxLenInv;
    int mIdxData;
    int mIdxCrc;
    CRC32 mCrcComp; // 통신 데이터에서 계산된 CRC값
    int32_t mLastTick; // 타임 아웃 관리
        
    // Receiver
    bool Recv(uint8_t c);
    void CheckWrite();
  };
}

#endif /* VITCON_BROKERCOMM_H_ */
