/**
 * Created by Park, Nil on 2017-04-26.
 * Last Revision: 2017-04-26
 *
 * 본 모듈은 Vitcon IOT 서버와의 통신에 사용되는 IOT 아이템을 정의한 것이다.
 *
 * Park, Nil (2017-04-26): 소스코드 관리 시작
 */

#ifndef VITCON_IOTITEM_H_
#define VITCON_IOTITEM_H_

#include <stdint.h>

namespace vitcon
{
  class IOTItem
  {
  public:
public:
    static const uint8_t TYPE_BIN = 0x00;
    static const uint8_t TYPE_INT = 0x01;
    static const uint8_t TYPE_FLO = 0x02;
    static const uint8_t TYPE_STR = 0x03;
    static const uint8_t TYPE_R = 0x00;
    static const uint8_t TYPE_W = 0x80;
    IOTItem();
    virtual const uint8_t * GetData() = 0;
    virtual void SetData(const uint8_t *data) = 0;
    virtual void Written() = 0; // To be called when the data is written by remote server
    uint8_t GetLength();
    uint16_t GetItemInfo();
  protected:
    uint8_t mType;
    uint8_t mLen;
  };
  
  class IOTItemBin: public IOTItem
  {
  public:
    IOTItemBin();
    IOTItemBin(void (* func)(bool));
    // Type specific interface functions
    bool Get();
    void Set(bool val);
    // Common interface functions
    const uint8_t * GetData();
    void SetData(const uint8_t *data);
    void Written();
  private:
    bool mData;
    void (* mFunc)(bool);
  };
  
  class IOTItemInt: public IOTItem
  {
  public:
    IOTItemInt();
    IOTItemInt(void (* func)(int32_t));
    // Type specific interface functions
    int32_t Get();
    void Set(int32_t val);
    // Common interface functions
    const uint8_t * GetData();
    void SetData(const uint8_t *data);
    void Written();
  private:
    int32_t mData;
    void (* mFunc)(int32_t);
  };
  
  class IOTItemFlo: public IOTItem
  {
  public:
    IOTItemFlo();
    IOTItemFlo(void (* func)(float));
    // Type specific interface functions
    float Get();
    void Set(float val);
    // Common interface functions
    const uint8_t * GetData();
    void SetData(const uint8_t *data);
    void Written();
  private:
    float mData;
    void (* mFunc)(float);
  };

  class IOTItemStr: public IOTItem
  {
  public:
    static const uint8_t LEN = 32;
    IOTItemStr();
    IOTItemStr(void (* func)(const char *));
    // Type specific interface functions
    const char * Get();
    void Set(const char * val);
    // Common interface functions
    const uint8_t * GetData();
    void SetData(const uint8_t *data);
    void Written();
  private:
    char mData[LEN];
    void (* mFunc)(const char *);
  };
}

#endif /* VITCON_IOTITEM_H_ */
