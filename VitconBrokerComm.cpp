/**
 * Created by Park, Nil on 2017-04-26.
 * Last Revision: 2017-04-26
 *
 * VitconBrokerComm.h의 Description 참조
 */

#include "VitconBrokerComm.h"

using namespace vitcon;

static const uint8_t CORRECTION = 80;
static const uint8_t SIGNATURE[] = { 'V' + CORRECTION, 'I' + CORRECTION, 'T' + CORRECTION, 'C' + CORRECTION, 'O' + CORRECTION, 'N' + CORRECTION };
static const uint16_t SIGNATURE_LEN = 6;

BrokerComm::BrokerComm(Stream *stream, const char *device_id, IOTItem **items, uint8_t item_count)
{
  mStream = stream;
  mDeviceId = device_id;
  mItems = items;
  mItemCount = item_count;
  mLastSent = 0;
  mLastRecv = 0;
  mInterval = 1000;
  mTimeout = 1000;
  mStatus = STATUS_ADVERTISE;
}

void BrokerComm::SetInterval(uint32_t ms)
{
  if (ms < 200)
    mInterval = 200;
  else
    mInterval = ms;
}

void BrokerComm::SetTimeout(uint32_t ms)
{
  if (ms < 200)
    mTimeout = 200;
  else
    mTimeout = ms;
}

uint32_t BrokerComm::SendHeader(uint16_t len)
{
  CRC32 crc;
  uint16_t lenInv = ~len;
  mStream->write(SIGNATURE, SIGNATURE_LEN);
  mStream->write((uint8_t *)(&len), sizeof(uint16_t));
  crc.Update((const uint8_t *)(&len), sizeof(uint16_t));
  mStream->write((uint8_t *)(&lenInv), sizeof(uint16_t));
  crc.Update((const uint8_t *)(&lenInv), sizeof(uint16_t));
  return crc;
}

void BrokerComm::SendCRC(uint32_t crc)
{
  mStream->write((uint8_t *)(&crc), sizeof(uint32_t));
  mStream->write((uint8_t)0);
}

void BrokerComm::Run()
{
  // 특정 시간마다 한번씩 서버로 상태를 전송
  int32_t curr = millis();
  switch (mStatus)
  {
    case STATUS_ADVERTISE:
      if (curr - mLastRecv >= mInterval)
      {
        Advertise();
        mStatus = STATUS_PACKETSENT;
        mLastSent = millis();
      }
      break;
    case STATUS_PACKETSENT:
      // 정상적으로 패킷을 교환하지 못했다면 Advertise 패킷을 전송
      if (curr - mLastSent >= mTimeout)
      {
        mStatus = STATUS_ADVERTISE;
        mLastRecv = millis();
      }
      break;
    case STATUS_PACKETRECV:
      // 정상적으로 패킷을 교환했다면 현재 상태를 전송
      if (curr - mLastRecv >= mInterval)
      {
        SendData();
        mStatus = STATUS_PACKETSENT;
        mLastSent = millis();
      }
      break;        
  }
  int c;
  while ((c = mStream->read()) != -1)// Recv(c) && mLen > 0)
  {
    if (Recv(c) && mLen > 0)
    {
      if (mData[0] == 2)
        CheckWrite();
      mStatus = STATUS_PACKETRECV;
      mLastRecv = millis();
    }
  }
}

void BrokerComm::Advertise()
{
  // cmd number, device id, item count, (item_type, item_length)
  CRC32 crc(SendHeader(mItemCount * 2 + 34));
  
  // Send cmd number
  uint8_t cmd = 0;
  mStream->write(cmd);
  crc.Update(cmd);
  
  // Send device id
  mStream->write(mDeviceId, DEVICEID_LEN);
  crc.Update((const uint8_t *)mDeviceId, DEVICEID_LEN);
  
  // Send item count
  mStream->write(mItemCount);
  crc.Update(mItemCount);
  
  // Send type data
  for (uint8_t i = 0; i < mItemCount; i++)
  {
    uint16_t typeData = mItems[i]->GetItemInfo();
    mStream->write((uint8_t *)(&typeData), sizeof(uint16_t));
    crc.Update((const uint8_t *)(&typeData), sizeof(uint16_t));
  }
  
  SendCRC(crc);
}

void BrokerComm::SendData()
{
  // Calculate total data length
  uint16_t len = 1;
  for (uint8_t i = 0; i < mItemCount; i++)
  {
    if ((mItems[i]->GetItemInfo() & IOTItem::TYPE_W) == 0)
      len += mItems[i]->GetLength() + 2; // index(1byte) + length(1byte) + data(len)
  }

  CRC32 crc(SendHeader(len));
  
  // Send cmd number
  uint8_t cmd = 1;
  mStream->write(cmd);
  crc.Update(cmd);

  // Send data
  for (uint8_t i = 0; i < mItemCount; i++)
  {
    if ((mItems[i]->GetItemInfo() & IOTItem::TYPE_W) == 0)
    {
      uint8_t l = mItems[i]->GetLength();
      const uint8_t *data = mItems[i]->GetData();
      mStream->write(i);
      crc.Update(i);
      mStream->write(l);
      crc.Update(l);
      mStream->write(data, l);
      crc.Update(data, l);
    }
  }
  
  SendCRC(crc);
}

bool BrokerComm::Recv(uint8_t c)
{  
  // 타임아웃시 패킷 초기화
  int32_t curr_tick = millis();
  if (curr_tick - mLastTick >= CHAR_TIMEOUT)
    mIdxSig = 0;
  mLastTick = curr_tick;
  
  /* Signature 일치 여부 확인 */
  if (mIdxSig < SIGNATURE_LEN)
  {
    if (c == SIGNATURE[mIdxSig++])
    {
      if (mIdxSig >= SIGNATURE_LEN)
      {
        mIdxLen = 0;
        mIdxLenInv = 0;
        mIdxCrc = 0;
        mIdxData = 0;
        mCrcComp.Reset();
      }
    }
    else
    {
      mIdxSig = 0;
    }
  }
  
  /* 길이 정보 읽어옴 */
  else if (mIdxLen < sizeof(uint16_t))
  {
    ((char *)(&mLen))[mIdxLen++] = c;
    if (mIdxLen >= sizeof(uint16_t))
      mCrcComp.Update((const uint8_t *)&mLen, sizeof(uint16_t));
  }
  /* 반전 길이 정보 읽어옴 */
  else if (mIdxLenInv < sizeof(uint16_t))
  {
    ((char *)(&mLenInv))[mIdxLenInv++] = c;
    if (mIdxLenInv >= sizeof(uint16_t))
    {
      if ((mLen ^ mLenInv) == 0xFFFF
        && mLen <= RECV_BUFFER)
        mCrcComp.Update((const uint8_t *)&mLenInv, sizeof(uint16_t));
      else
      {
        mIdxSig = 0;
      }
    }
  }
  /* 패킷 데이터 읽어옴 */
  else if (mIdxData < mLen)
  {
    mData[mIdxData++] = c;
    if (mIdxData >= mLen)
      mCrcComp.Update(mData, mLen);
  }
  /* CRC 읽어옴 */
  else if (mIdxCrc < sizeof(uint32_t))
  {
    ((uint8_t *)(&mCrc32))[mIdxCrc++] = c;
    if (mIdxCrc >= sizeof(uint32_t))
    {
      mIdxSig = 0;
      
      if (mCrc32 == mCrcComp)
      {
        return true; // 정상 적인 패킷을 성공적으로 받음. 더블 버퍼링 없음.
      }
    }
  }
  /* 절대 있어서는 안되는 조건 (진입 불가) */
  else
  {
    mIdxSig = 0;
  }
  return false;
}

void BrokerComm::CheckWrite()
{
  if (mLen >= 2)
  {
    uint8_t cmd = mData[0];
    uint8_t idx = mData[1];
    if (idx < mItemCount
      && mItems[idx]->GetLength() == mLen - 2)
    {
      mItems[idx]->SetData(&mData[2]);
      mItems[idx]->Written();
    }
  }
}
