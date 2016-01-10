#ifndef A_NDN_TAG_H_
#define A_NDN_TAG_H_

#include "ns3/tag.h"
#include "ns3/packet.h"

using namespace ns3;

//tags
class StaTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetSta (uint32_t flag);
  uint32_t GetSta (void) const;

  void Print (std::ostream &os) const;

private:
  uint32_t m_sta;

  // end class
};


//tags
class AllTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetACK (uint32_t flag);
  uint32_t GetACK (void) const;

  void Print (std::ostream &os) const;

private:
  uint32_t m_ack;

  // end class
};



//Producer or Consumer or Pigger :{1,2,0}
class PCTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetFlag (uint32_t flag);
  uint32_t GetFlag (void) const;

  void Print (std::ostream &os) const;

private:
  uint32_t m_flag;

  // end class
};

class LastHopTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetMobility (double mobility);
  double GetMobility (void) const;

  void Print (std::ostream &os) const;

private:
  double m_mobility;

  // end class 
};


class SourceDestTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetSource (double mobility);
  double GetSource (void) const;
  void SetDest (double dest);
  double GetDest (void) const;

  void Print (std::ostream &os) const;

private:
  double m_source;
  double m_dest;

  // end class
};

//
class VelocityTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetVelocity (double velocity);
  double GetVelocity (void) const;

  void Print (std::ostream &os) const;

private:
  double m_velocity;

  // end class 
};

class ObjectSizeTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetSize (double size);
  double GetSize (void) const;

  void Print (std::ostream &os) const;

private:
  double m_objectSize;
};


class HopTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void UpdateHop ();
  void SetHop (uint32_t hop);
  uint32_t GetHop (void) const;

  void UpdateOverHead ();
  void SetOverHead (uint32_t hop);
  uint32_t GetOverHead (void) const;

  void Print (std::ostream &os) const;

private:
  uint32_t m_hop;
  uint32_t m_overHead;
};


//------------------------------------------------

class IndexTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetIndex (double mobility);
  double GetIndex (void) const;

  void Print (std::ostream &os) const;

private:
  double m_index;

  // end class
};

///////////////////////////////////////////////////////////
TypeId 
LastHopTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("LastHopTag")
    .SetParent<Tag> ()
    .AddConstructor<LastHopTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

//--------------------------------------------------

//--------------------------------------------------
TypeId 
LastHopTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
LastHopTag::GetSerializedSize (void) const
{
  return 8;
}
void 
LastHopTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(m_mobility);
}
void 
LastHopTag::Deserialize (TagBuffer i)
{
  m_mobility = i.ReadDouble();
}

void
LastHopTag::SetMobility (double mobility)
{
  m_mobility = mobility;
}
double
LastHopTag::GetMobility (void) const
{
  return m_mobility;
}

void 
LastHopTag::Print (std::ostream &os) const
{
  os << "v=" << m_mobility;
}

//------------------------------------------------

///////////////////////////////////////////////////////////
TypeId
AllTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("AllTag")
    .SetParent<Tag> ()
    .AddConstructor<LastHopTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

TypeId
AllTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
AllTag::GetSerializedSize (void) const
{
  return sizeof(int);
}
void
AllTag::Serialize (TagBuffer i) const
{
  i.WriteU32(m_ack);
}
void
AllTag::Deserialize (TagBuffer i)
{
  m_ack = i.ReadU32();
}

void
AllTag::SetACK (uint32_t  flag)
{
  m_ack = flag;
}
uint32_t
AllTag::GetACK (void) const
{
  return m_ack;
}

void
AllTag::Print (std::ostream &os) const
{
  os << "v=" << m_ack;
}



///////////////////////////////////////////////////////////
TypeId
StaTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("StaTag")
    .SetParent<Tag> ()
    .AddConstructor<LastHopTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

TypeId
StaTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
StaTag::GetSerializedSize (void) const
{
  return sizeof(int);
}
void
StaTag::Serialize (TagBuffer i) const
{
  i.WriteU32(m_sta);
}
void
StaTag::Deserialize (TagBuffer i)
{
  m_sta = i.ReadU32();
}

void
StaTag::SetSta (uint32_t  flag)
{
  m_sta = flag;
}
uint32_t
StaTag::GetSta (void) const
{
  return m_sta;
}

void
StaTag::Print (std::ostream &os) const
{
  os << "v=" << m_sta;
}




//------------------------------------------------

///////////////////////////////////////////////////////////
TypeId
PCTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("PCTag")
    .SetParent<Tag> ()
    .AddConstructor<LastHopTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

TypeId
PCTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
PCTag::GetSerializedSize (void) const
{
  return sizeof(int);
}
void
PCTag::Serialize (TagBuffer i) const
{
  i.WriteU32(m_flag);
}
void
PCTag::Deserialize (TagBuffer i)
{
  m_flag = i.ReadU32();
}

void
PCTag::SetFlag (uint32_t  flag)
{
  m_flag = flag;
}
uint32_t
PCTag::GetFlag (void) const
{
  return m_flag;
}

void
PCTag::Print (std::ostream &os) const
{
  os << "v=" << m_flag;
}

//------------------------------------------------


TypeId
SourceDestTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("SourceDestTag")
    .SetParent<Tag> ()
    .AddConstructor<SourceDestTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

TypeId
SourceDestTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
SourceDestTag::GetSerializedSize (void) const
{
  return 2*sizeof(double);
}
void
SourceDestTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(m_source);
  i.WriteDouble(m_dest);
}
void
SourceDestTag::Deserialize (TagBuffer i)
{
  m_source = i.ReadDouble();
  m_dest = i.ReadDouble();
}

void
SourceDestTag::SetSource (double mobility)
{
  m_source = mobility;
}
double
SourceDestTag::GetSource (void) const
{
  return m_source;
}
void
SourceDestTag::SetDest (double mobility)
{
  m_dest = mobility;
}
double
SourceDestTag::GetDest (void) const
{
  return m_dest;
}
void
SourceDestTag::Print (std::ostream &os) const
{
  os << "source--" << m_source<<" destination--"<<m_dest;
}

////////////////////////////////////////////////////////////////------------------------------------------------



TypeId 
VelocityTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("VelocityTag")
    .SetParent<Tag> ()
    .AddConstructor<VelocityTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}
TypeId 
VelocityTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
VelocityTag::GetSerializedSize (void) const
{
  return 8;
}
void 
VelocityTag::Serialize (TagBuffer i) const
{
  double t = m_velocity;
  i.Write ((const uint8_t *)&t, 8);
}
void 
VelocityTag::Deserialize (TagBuffer i)
{
  double t;
  i.Read ((uint8_t *)&t, 8);
  m_velocity = t;
}

void
VelocityTag::SetVelocity (double velocity)
{
  m_velocity = velocity;
}
double
VelocityTag::GetVelocity (void) const
{
  return m_velocity;
}

void 
VelocityTag::Print (std::ostream &os) const
{
  os << "v=" << m_velocity;
}


//--------------------------------------------------
TypeId 
ObjectSizeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ObjectSizeTag")
    .SetParent<Tag> ()
    .AddConstructor<VelocityTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}
TypeId 
ObjectSizeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
ObjectSizeTag::GetSerializedSize (void) const
{
  return 8;
}
void 
ObjectSizeTag::Serialize (TagBuffer i) const
{
  double t = m_objectSize;
  i.Write ((const uint8_t *)&t, 8);
}
void 
ObjectSizeTag::Deserialize (TagBuffer i)
{
  double t;
  i.Read ((uint8_t *)&t, 8);
  m_objectSize = t;
}

void
ObjectSizeTag::SetSize (double size)
{
  m_objectSize = size;
}
double
ObjectSizeTag::GetSize (void) const
{
  return m_objectSize;
}

void 
ObjectSizeTag::Print (std::ostream &os) const
{
  os << "v=" << m_objectSize;
}

//----------------------------------------------------------
TypeId 
HopTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("HopTag")
    .SetParent<Tag> ()
    .AddConstructor<HopTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}

//--------------------------------------------------

//--------------------------------------------------
TypeId 
HopTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
HopTag::GetSerializedSize (void) const
{
  return 2*sizeof(uint32_t);
}
void 
HopTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_hop);
  i.WriteU32(m_overHead);
}
void 
HopTag::Deserialize (TagBuffer i)
{
  m_hop = i.ReadU32 ();
  m_overHead = i.ReadU32 ();
}

void
HopTag::UpdateHop ()
{
  m_hop++;
}
void
HopTag::UpdateOverHead()
{
  m_overHead++;
}
void
HopTag::SetHop(uint32_t hop)
{
  m_hop = hop;
}
void
HopTag::SetOverHead (uint32_t hop)
{
  m_overHead = hop;
}
uint32_t
HopTag::GetHop (void) const
{
  return m_hop;
}
uint32_t
HopTag::GetOverHead (void) const
{
  return m_overHead;
}

void 
HopTag::Print (std::ostream &os) const
{
  os <<m_hop;
}


//--------------------------------------------------
TypeId
IndexTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("IndexTag")
    .SetParent<Tag> ()
    .AddConstructor<IndexTag> ()
    // .AddAttribute ("Mobility",
    //                "The Mobility",
    //                EmptyAttributeValue (),
    //                MakeTimeAccessor (&TimestampTag::GetTimestamp),
    //                MakeTimeChecker ())
  ;
  return tid;
}
TypeId
IndexTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
IndexTag::GetSerializedSize (void) const
{
  return 8;
}
void
IndexTag::Serialize (TagBuffer i) const
{
  double t = m_index;
  i.Write ((const uint8_t *)&t, 8);
}
void
IndexTag::Deserialize (TagBuffer i)
{
  double t;
  i.Read ((uint8_t *)&t, 8);
  m_index = t;
}

void
IndexTag::SetIndex (double mobility)
{
  m_index = mobility;
}
double
IndexTag::GetIndex (void) const
{
  return m_index;
}

void
IndexTag::Print (std::ostream &os) const
{
  os << "v=" << m_index;
}

//------------------------------------------------



#endif
