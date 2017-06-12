/**
 * Created by Park, Nil on 2017-04-26.
 * Last Revision: 2017-04-26
 *
 * VitconIOTItem.h의 Description 참조
 */

#include "VitconIOTItem.h"

using namespace vitcon;

static const uint8_t TYPE_LENGTHS[] = { 1, 4, 4, 32 };

/* Super Class Functions */

IOTItem::IOTItem()
{
  
}

uint8_t IOTItem::GetLength()
{
  return mLen;
}

uint16_t IOTItem::GetItemInfo()
{
  uint16_t res = mType;
  res |= ((uint16_t)mLen << 8);
  return res;
}

/* Constructors */

IOTItemBin::IOTItemBin()
{
  mFunc = 0;
  mType = TYPE_BIN;
  mLen = TYPE_LENGTHS[mType];
}
IOTItemBin::IOTItemBin(void (* func)(bool)): IOTItemBin()
{
  mFunc = func;
  if (func != 0)
    mType |= TYPE_W;
}

IOTItemInt::IOTItemInt()
{
  mFunc = 0;
  mType = TYPE_INT;
  mLen = TYPE_LENGTHS[mType];
}
IOTItemInt::IOTItemInt(void (* func)(int32_t)): IOTItemInt()
{
  mFunc = func;
  if (func != 0)
    mType |= TYPE_W;
}

IOTItemFlo::IOTItemFlo()
{
  mFunc = 0;
  mType = TYPE_FLO;
  mLen = TYPE_LENGTHS[mType];
}
IOTItemFlo::IOTItemFlo(void (* func)(float)): IOTItemFlo()
{
  mFunc = func;
  if (func != 0)
    mType |= TYPE_W;
}

IOTItemStr::IOTItemStr()
{
  mFunc = 0;
  mType = TYPE_STR;
  mLen = TYPE_LENGTHS[mType];
}
IOTItemStr::IOTItemStr(void (* func)(const char *)): IOTItemStr()
{
  mFunc = func;
  if (func != 0)
    mType |= TYPE_W;
}

/* Type Specific Get Functions */

bool IOTItemBin::Get()
{
  return mData;
}

int32_t IOTItemInt::Get()
{
  return mData;
}

float IOTItemFlo::Get()
{
  return mData;
}

const char *IOTItemStr::Get()
{
  return mData;
}

/* Type Specific Set Functions */

void IOTItemBin::Set(bool val)
{
  mData = val;
}

void IOTItemInt::Set(int32_t val)
{
  mData = val;
}

void IOTItemFlo::Set(float val)
{
  mData = val;
}

void IOTItemStr::Set(const char *val)
{
  for (int i = 0; i < LEN; i++)
    mData[i] = val[i];
}

/* Common GetData Functions */

const uint8_t *IOTItemBin::GetData()
{
  return (uint8_t *)&mData;
}

const uint8_t *IOTItemInt::GetData()
{
  return (uint8_t *)&mData;
}

const uint8_t *IOTItemFlo::GetData()
{
  return (uint8_t *)&mData;
}

const uint8_t *IOTItemStr::GetData()
{
  return (uint8_t *)mData;
}

/* Common SetData Functions */

void IOTItemBin::SetData(const uint8_t *data)
{
  for (int i = 0; i < mLen; i++)
    ((uint8_t *)&mData)[i] = data[i];
}

void IOTItemInt::SetData(const uint8_t *data)
{
  for (int i = 0; i < mLen; i++)
    ((uint8_t *)&mData)[i] = data[i];
}

void IOTItemFlo::SetData(const uint8_t *data)
{
  for (int i = 0; i < mLen; i++)
    ((uint8_t *)&mData)[i] = data[i];
}

void IOTItemStr::SetData(const uint8_t *data)
{
  for (int i = 0; i < mLen; i++)
    mData[i] = (char)data[i];
}

/* Callback Functions */

void IOTItemBin::Written()
{
  if (mFunc != 0)
    mFunc(mData);
}

void IOTItemInt::Written()
{
  if (mFunc != 0)
    mFunc(mData);
}

void IOTItemFlo::Written()
{
  if (mFunc != 0)
    mFunc(mData);
}

void IOTItemStr::Written()
{
  if (mFunc != 0)
    mFunc(mData);
}
