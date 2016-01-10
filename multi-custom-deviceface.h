#ifndef MULTI_CUSTOM_DEVICEFACE_H
#define MULTI_CUSTOM_DEVICEFACE_H

#include "math.h"
#include "ns3/ndn-net-device-face.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"
#include "a-ndn-tag.h"

using namespace std;
namespace ns3{
namespace ndn{
class MultiCustomDeviceFace : public NetDeviceFace{
public:
  static TypeId
  GetTypeId ();
  double  range;
  static int totalnterestNum;
  static int duplicte;
  double m_dec;
//  static int totalInterestNum;

  MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice);
  MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice,double i);
  MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice,uint32_t array[],uint32_t numP,double p,double decs);
  virtual ~MultiCustomDeviceFace();
  virtual void RegisterProtocolHandlers (const InterestHandler &interestHandler, const DataHandler &dataHandler);
  bool Receive (Ptr<const Packet> p);
  bool ToSend ();
  void FromSendQueue();
  void SetRange(double rangex);

protected:
  virtual bool Send (Ptr<Packet> p);
private:
  void ReceiveFromNetDevice (Ptr<NetDevice> device,
                           Ptr<const Packet> p,
                           uint16_t protocol,
                           const Address &from,
                           const Address &to,
                           NetDevice::PacketType packetType);
  struct Item
   {
     Item (const Time &_gap, const Ptr<Packet> &_packet);
     Item (const Item &item);

     Item &
     operator ++ ();

     Item &
     Gap (const Time &time);

     Time m_gap;
     Ptr<Packet> m_packet;
     HeaderHelper::Type m_type;
     Ptr<const Name> m_name;
     uint32_t m_retxCount;
   };
  typedef std::list<Item> ItemQueue;
  ItemQueue m_sendQueue;
  ItemQueue m_receiveQueue;
  ItemQueue m_rexQueue;
  UniformVariable m_randomGap;//0-2ms
  EventId m_sendSchedule;//for send event
  EventId m_toSendSchedule;//for TO send() event

  double GetDistance (double sourcePosition);
  bool GetProbability (double distance);
  double interestNum;
  double m_number;
  uint32_t m_currentIndex;
  uint32_t temp;
  double m_probility;
  double s_number;
  bool receiveInterestFlag;
  bool receiveDataFlag;
  bool reSendInterestFlag;
  bool reSendDataFlag;
  bool needToCancel;
  EventId sendInterestEvent;
  EventId sendDataEvent;
  EventId reSendInterestEvent;
  EventId reSendDataEvent;
  double waitTime;
  uint32_t reSendTimes_I;
  uint32_t reSendTimes_D;
  uint32_t tempTimes;
  bool print;
  uint32_t consumerId;
  uint32_t producerId;
  uint32_t numProducer;
 // uint32_t producerArray[8];//1~numProducer
  bool isProducer;
private:
  Ptr<NetDevice> m_netDevice;
};

NS_OBJECT_ENSURE_REGISTERED (MultiCustomDeviceFace);

int MultiCustomDeviceFace::totalnterestNum = 0;
int MultiCustomDeviceFace::duplicte = 0;
TypeId
MultiCustomDeviceFace::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::MultiCustomDeviceFace")
    .SetParent<NetDeviceFace> ();
  return tid;
}

MultiCustomDeviceFace::MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice,double i)
  : NetDeviceFace (node, netDevice),interestNum(0),m_number(0),s_number(0)
  ,receiveInterestFlag(false),receiveDataFlag(false),reSendInterestFlag(true),reSendDataFlag(true),waitTime(0.002)
  ,reSendTimes_I(0),tempTimes(0),print(0),m_netDevice(netDevice),reSendTimes_D(0)
  {
    m_randomGap = UniformVariable(0,0.002);
    consumerId = i-2;
    producerId = i-1;
    range = 300;
    m_currentIndex = -1;
    needToCancel = false;
  }
MultiCustomDeviceFace::MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice,uint32_t array[],uint32_t numP,double p,double decs)
  : NetDeviceFace (node, netDevice),interestNum(0),m_number(0),s_number(0)
  ,receiveInterestFlag(false),receiveDataFlag(false),reSendInterestFlag(true),reSendDataFlag(true),waitTime(0.002)
  ,reSendTimes_I(0),tempTimes(0),print(0),m_netDevice(netDevice),reSendTimes_D(0)
  {
    m_randomGap = UniformVariable(0,0.002);
    consumerId = array[0];
    m_probility = p;
    numProducer = numP;
    temp = 0;
    m_dec = decs;
 //   cout<<m_node->GetId()<<endl;
    for(int i=1;i<numProducer+1;i++)
      {
//      producerArray[i] = array[i];
//      cout<<"xiao"<<producerArray[i]<<endl;
      if(m_node->GetId()==array[i])
        {
        isProducer = true;
//      cout<<"true"<<m_node->GetId()<<endl;
        }
      else
        {
        isProducer = false;
 //       cout<<"false"<<endl;
        }
      }
    range = 300;
    m_currentIndex = -1;
    needToCancel = false;
  }

MultiCustomDeviceFace::MultiCustomDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice)
  : NetDeviceFace (node, netDevice),interestNum(0),m_number(0),s_number(0)
  ,receiveInterestFlag(false),receiveDataFlag(false),reSendInterestFlag(true),reSendDataFlag(true),waitTime(0.002)
  ,reSendTimes_I(0),tempTimes(0),print(0),m_netDevice(netDevice),reSendTimes_D(0)
  {
   consumerId = 8;
   producerId = 9;
   range = 300;
  }
MultiCustomDeviceFace::Item::Item (const Time &gap, const Ptr<Packet> &packet)
  : m_gap (gap), m_packet (packet), m_retxCount (0)
{
  // NS_LOG_FUNCTION (this << _gap << _packet);

  m_name = HeaderHelper::GetName (packet); // not efficient (extra deserialization), but it is the only way for now
  NS_ASSERT (m_name != 0);

  HeaderHelper::Type guessedType = HeaderHelper::GetNdnHeaderType (packet);
  if (guessedType == HeaderHelper::INTEREST_CCNB ||
      guessedType == HeaderHelper::INTEREST_NDNSIM)
    {
      m_type = HeaderHelper::INTEREST_NDNSIM;
      NS_LOG_DEBUG ("Schedule low-priority Interest");
    }
  else if (guessedType == HeaderHelper::CONTENT_OBJECT_CCNB ||
           guessedType == HeaderHelper::CONTENT_OBJECT_NDNSIM)
    {
      m_type = HeaderHelper::CONTENT_OBJECT_NDNSIM;
      NS_LOG_DEBUG ("Schedule low-priority ContentObject");
    }
  else
    {
      NS_FATAL_ERROR ("Unknown NDN header type");
    }
}

MultiCustomDeviceFace::Item::Item (const Item &item)
  : m_gap (item.m_gap), m_packet (item.m_packet), m_type (item.m_type), m_name (item.m_name), m_retxCount (item.m_retxCount)
{
}

MultiCustomDeviceFace::Item &
MultiCustomDeviceFace::Item::operator ++ ()
{
  // remote GeoTag when packet is scheduled for retransmission

  m_retxCount ++;
  return *this;
}

MultiCustomDeviceFace::Item &
MultiCustomDeviceFace::Item::Gap (const Time &time)
{
  m_gap = time;
  return *this;
}

void
MultiCustomDeviceFace::SetRange(double rangex)
{
range = rangex;
}
MultiCustomDeviceFace::~MultiCustomDeviceFace ()
{
  //NS_LOG_FUNCTION_NOARGS ();
}

bool
MultiCustomDeviceFace::Send (Ptr<Packet> packet)
{

  //add hop tag
  if (!Face::Send (packet))
    {
      return false;
    }

 //enqueue sendQueue
  AllTag ackTag;
  Time gap = Seconds( m_randomGap.GetValue() );
  bool isAck= packet->PeekPacketTag(ackTag) && ackTag.GetACK() == 1;

  if (isAck)
    {
 //   cout<<m_node->GetId()<<" ACK TRANSE\n";
    Ptr<const Name> name = HeaderHelper::GetName (packet); // not efficient (extra deserialization), but it is the only way for now
    NS_ASSERT (name != 0);
    // cancel delay event
    AllTag ackTag;
    bool cancel = false;

    ItemQueue::iterator it = m_receiveQueue.begin();
    while (it != m_receiveQueue.end() )
      {

        if (*it->m_name == *name)
        {
          cancel = true;
          ItemQueue::iterator tmp = it;
          tmp++;
          m_receiveQueue.erase(it);
           it = tmp;
        }else
          {
            it++;
          }
      }

    it = m_rexQueue.begin();
    while (it != m_rexQueue.end() )
      {
      cancel = true;
      if (*it->m_name == *name)
        {
            ItemQueue::iterator tmp = it;
            tmp++;
            m_rexQueue.erase(it);
           it = tmp;
        }else
          {
            it++;
          }
      }

    it = m_sendQueue.begin();
    while (it != m_sendQueue.end() )
      {
      cancel = true;
      if ( *it->m_name == *name)
        {
            ItemQueue::iterator tmp = it;
            tmp++;
            m_sendQueue.erase(it);
           it = tmp;
        }else
          {
            it++;
          }
      }
        m_sendQueue.push_front(Item(Seconds(0.0) , packet));
    }else
    {
      m_sendQueue.push_back(Item(gap , packet));
    }


// cout<<m_node->GetId()<<" onNetDevice-Send plan on--"<<Simulator::Now().GetSeconds()+gap.GetSeconds()<<endl;
 if (!m_sendSchedule.IsRunning() && m_sendQueue.size() > 0)
 {
   m_sendSchedule = Simulator::Schedule(m_sendQueue.front().m_gap , &MultiCustomDeviceFace::FromSendQueue , this);
 }else
   cout<<"Busy and m_sendQueue.size() "<<m_sendQueue.size()<<endl;

  return true;
}

bool
MultiCustomDeviceFace::ToSend ()
{
 // cout<<Simulator::Now().GetSeconds()<<"\t"<<m_node->GetId()<<" onNetDevice-Tosend"<<endl;
  if (m_receiveQueue.size() > 0)
    {
    Item item = m_receiveQueue.front();
    m_receiveQueue.pop_front();
      //enqueue m_rex_queue
 //     if (item.m_retxCount < 100)
        {
          m_rexQueue.push_back(++item.Gap(Seconds(0.05)));
        }
      Receive(item.m_packet);
    }else if (m_rexQueue.size() > 0)
      {
      Item item = m_rexQueue.front();
      m_rexQueue.pop_front();
      cout<<Simulator::Now().GetSeconds()<<"\t"<<m_node->GetId()<<" Retrans "<<item.m_retxCount
          <<"\t"<<m_rexQueue.size()<<endl;
        //enqueue m_rex_queue
//        if (item.m_retxCount < 100)
          {
            m_rexQueue.push_back(++item.Gap(Seconds(0.05)));
          }
        Receive(item.m_packet);
      }

      if (!m_toSendSchedule.IsRunning() && m_receiveQueue.size()>0)
      {
        m_toSendSchedule = Simulator::Schedule(m_receiveQueue.front().m_gap , &MultiCustomDeviceFace::ToSend , this);
      }else if (!m_toSendSchedule.IsRunning() && m_rexQueue.size() > 0)
        {
        m_toSendSchedule = Simulator::Schedule(m_rexQueue.front().m_gap , &MultiCustomDeviceFace::ToSend , this);
        }

  return true;
}

void
MultiCustomDeviceFace::FromSendQueue ()
{
 if (m_sendQueue.size() > 0)
   {
     Item item = m_sendQueue.front();
     m_sendQueue.pop_front();
     double pos = m_node->GetObject<MobilityModel> ()->GetPosition().x;
     cout<<Simulator::Now().GetSeconds()<<"\t"<<m_node->GetId()<<" onNetDevice-FromSendQueue pos--"<<pos<<endl;
     //tag ++
     Ptr<Packet> packet = item.m_packet;
     HopTag hopTag;
     packet->RemovePacketTag(hopTag);
     hopTag.UpdateHop();
     packet->AddPacketTag (hopTag);

     // update last hop position
     LastHopTag LastHopTag;
     packet->RemovePacketTag(LastHopTag);
     Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
     LastHopTag.SetMobility((mobility->GetPosition()).x);
     packet->AddPacketTag (LastHopTag);

    m_netDevice->Send (packet, m_netDevice->GetBroadcast (),
                              L3Protocol::ETHERNET_FRAME_TYPE);
    if (!m_sendSchedule.IsRunning() && m_sendQueue.size()>0)
      {
             m_sendSchedule = Simulator::Schedule(m_sendQueue.front().m_gap , &MultiCustomDeviceFace::FromSendQueue , this);
      }
   }
}

void
MultiCustomDeviceFace::RegisterProtocolHandlers (const InterestHandler &interestHandler, const DataHandler &dataHandler)
{
   //NS_LOG_FUNCTION (this);

  Face::RegisterProtocolHandlers (interestHandler, dataHandler);

  m_node->RegisterProtocolHandler (MakeCallback (&MultiCustomDeviceFace::ReceiveFromNetDevice, this),
                                   L3Protocol::ETHERNET_FRAME_TYPE, GetNetDevice (), true/*promiscuous mode*/);
  //cout<<"REGISTER";
}


void
MultiCustomDeviceFace::ReceiveFromNetDevice (Ptr<NetDevice> device,
                           Ptr<const Packet> p,
                           uint16_t protocol,
                           const Address &from,
                           const Address &to,
                           NetDevice::PacketType packetType)
{
  //for statist
  Ptr<Packet> packet = p->Copy();
  StaTag sta;
  sta.SetSta(1);
  packet->AddPacketTag(sta);
  Receive(packet);

  //get packet type
  HeaderHelper::Type receiveType = HeaderHelper::GetNdnHeaderType (p);
  if (receiveType == HeaderHelper::INTEREST_CCNB ||
      receiveType == HeaderHelper::INTEREST_NDNSIM)
    {
    receiveType = HeaderHelper::INTEREST_NDNSIM;
    }
  else if (receiveType == HeaderHelper::CONTENT_OBJECT_CCNB ||
      receiveType == HeaderHelper::CONTENT_OBJECT_NDNSIM)
    {
    receiveType = HeaderHelper::CONTENT_OBJECT_NDNSIM;
    cout<<"NODE "<<m_node->GetId()<<" receive DATA\n";
    }
  else
    {
      NS_FATAL_ERROR ("Unknown NDN header type");
    }



//get last hop position
  LastHopTag LastHopTag;
  p->PeekPacketTag(LastHopTag);

  //get source position
  SourceDestTag DestTag;
  p->PeekPacketTag(DestTag);



  double a = DestTag.GetDest() , b = m_node->GetObject<MobilityModel>()->GetPosition().x, c = LastHopTag.GetMobility();
  cout<<"Node--"<<m_node->GetId()<<" dest--"<<a<<" self--"<<b<<" last--"<<c<<endl;
   if(!(abs(m_node->GetObject<MobilityModel>()->GetPosition().x-DestTag.GetDest()) <=
       abs(LastHopTag.GetMobility()-DestTag.GetDest()) &&
       abs(m_node->GetObject<MobilityModel>()->GetPosition().x-LastHopTag.GetMobility())<=
       abs(LastHopTag.GetMobility()-DestTag.GetDest())))
     {
       needToCancel = true;
     }
   else
     {
     //add tag to stop trans in fw layer
       needToCancel = false;
     }

   Ptr<const Name> name = HeaderHelper::GetName (p); // not efficient (extra deserialization), but it is the only way for now
   NS_ASSERT (name != 0);
   // cancel delay event
   AllTag ackTag;

   bool cancel = false;
   bool isAck= p->PeekPacketTag(ackTag) && ackTag.GetACK() == 1;
   ItemQueue::iterator it = m_receiveQueue.begin();

   while (it != m_receiveQueue.end() )
     {
       if (/* (it->m_type == receiveType || receiveType == HeaderHelper::CONTENT_OBJECT_NDNSIM || isAck)
           && */*it->m_name == *name)
       {
         cancel = true;
         ItemQueue::iterator tmp = it;
         tmp++;
         m_receiveQueue.erase(it);
          it = tmp;
       }else
         {
           it++;
         }
     }

   it = m_rexQueue.begin();
   while (it != m_rexQueue.end() )
     {
     cancel = true;
     if (/* (it->m_type == receiveType || receiveType == HeaderHelper::CONTENT_OBJECT_NDNSIM || isAck)
         && */*it->m_name == *name)
       {
           ItemQueue::iterator tmp = it;
           tmp++;
           m_rexQueue.erase(it);
          it = tmp;
       }else
         {
           it++;
         }
     }

   it = m_sendQueue.begin();
   while (it != m_sendQueue.end() )
     {
     cancel = true;
     if (/* (it->m_type == receiveType || receiveType == HeaderHelper::CONTENT_OBJECT_NDNSIM || isAck)
         && */*it->m_name == *name)
       {
           ItemQueue::iterator tmp = it;
           tmp++;
           m_sendQueue.erase(it);
          it = tmp;
       }else
         {
           it++;
         }
     }
 //test
   if (cancel)
   cout<<Simulator::Now().GetSeconds()<<"\t"<<m_node->GetId()<<" onNetDevice-ReceiveFromNetDevice-cancel"<<endl;



   if (m_node->GetId() == 18 && receiveType == HeaderHelper::CONTENT_OBJECT_NDNSIM)
        cout<<"ReceiveFrom seeeeeeeek "<<m_node->GetId()<<" receive DATA\n";
   //test
   //enqueue
   if (!cancel && !needToCancel && !isAck)
     {
       double distance = GetDistance(LastHopTag.GetMobility());
       double delayTime = (range-min(fabs(distance),range))/range*0.05;
       cout<<Simulator::Now().GetSeconds()<<"\t"<<m_node->GetId()<<" delay--"<<delayTime<<endl;

       Ptr<Packet> packet = p->Copy();
       Item item = Item(Seconds(delayTime) , packet);
       m_receiveQueue.push_back(item);
       if (!m_toSendSchedule.IsRunning() && m_receiveQueue.size() > 0)
       {
         m_toSendSchedule = Simulator::Schedule(m_receiveQueue.front().m_gap , &MultiCustomDeviceFace::ToSend , this);
       }
     }

}

bool
MultiCustomDeviceFace::Receive (Ptr<const Packet> p)
{

  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  Ptr<Packet> packet = p->Copy (); // give upper layers a rw copy of the packet
  try
    {
      HeaderHelper::Type type = HeaderHelper::GetNdnHeaderType (packet);
      switch (type)
        {
        case HeaderHelper::INTEREST_NDNSIM:
          return ReceiveInterest (Wire::ToInterest (packet, Wire::WIRE_FORMAT_NDNSIM));
        case HeaderHelper::INTEREST_CCNB:
          return ReceiveInterest (Wire::ToInterest (packet, Wire::WIRE_FORMAT_CCNB));
        case HeaderHelper::CONTENT_OBJECT_NDNSIM:
          {
          return ReceiveData (Wire::ToData (packet, Wire::WIRE_FORMAT_NDNSIM));
          }
        case HeaderHelper::CONTENT_OBJECT_CCNB:
          return ReceiveData (Wire::ToData (packet, Wire::WIRE_FORMAT_CCNB));
        default:
          //NS_FATAL_ERROR ("Not supported NDN header");
          return false;
        }

    }
  catch (UnknownHeaderException)
    {

      return false;
    }

  return false;
}



double
MultiCustomDeviceFace::GetDistance(double sourcePosition)
{
  Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
  double destPosition = mobility->GetPosition().x;
  return fabs(destPosition-sourcePosition);
}
bool
MultiCustomDeviceFace::GetProbability (double distance)
{
  UniformVariable probability;
  double i = probability.GetValue (0.0,1.0);
//  double p = abs(distance)/range*0.8+100/((producerId+1)*10)*0.5;
  double p = abs(distance)/range*m_probility*m_dec+abs(m_node->GetObject<MobilityModel> ()->GetVelocity().x)/60*(1-m_probility)*m_dec;
//  double p = abs(distance)/range;
  //cout<<i<<"\t"<<p<<endl;
  if(abs(distance)>range*0.9)
    return true;
  if (i<=p)
    return true;
  else
    return false;
}
}
}
#endif

