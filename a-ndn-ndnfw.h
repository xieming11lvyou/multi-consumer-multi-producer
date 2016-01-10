#include "ns3/ndn-forwarding-strategy.h"
#include "ns3/ndn-name.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndnSIM/model/pit/ndn-pit-impl.h"
#include <iomanip>
#include <boost/foreach.hpp>
// #include "a-ndn-simple-test.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"

using namespace std;

namespace ns3 {
namespace ndn {
namespace fw {


class ndnFw : public ForwardingStrategy
{
private:
  struct Item
    {
      Item (const Ptr<Name> & name, const int64_t & time);
      Ptr<Name> m_name;
      int64_t m_time;
    };
    typedef std::list<Item> ItemQueue;

    ItemQueue m_requestQueue;
    ItemQueue m_numCarQueue;
    list<int> m_reQueue;
public:
	static TypeId GetTypeId ();
	ndnFw();
 void OnInterest (Ptr<Face> inFace,
                                Ptr<Interest> interest);
 void DoOnInterest (Ptr<Face> inFace,
                                Ptr<Interest> interest);
	void OnData (Ptr<Face> inFace,Ptr<Data> data);
	// void DoOnData (Ptr<Face> inFace,Ptr<Data> data);

  bool DoPropagateInterest (Ptr<Face> inFace,
                       		  Ptr<const Interest> interest,
                            Ptr<pit::Entry> pitEntry);

  bool TrySendOutInterest (Ptr<Face> inFace,
                          Ptr<Face> outFace,
                          Ptr<const Interest> interest,
                          Ptr<pit::Entry> pitEntry);
  void DidReceiveSolicitedData (Ptr<Face> inFace,
                                               Ptr<const Data> data,
                                               bool didCreateCacheEntry);
  void DidReceiveUnsolicitedData (Ptr<Face> inFace,
                                               Ptr<const Data> data,
                                               bool didCreateCacheEntry);
  bool CanSendOutInterest (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const Interest> interest,
                                        Ptr<pit::Entry> pitEntry);
  void InsertUpdateQueue(ItemQueue & itemqueue, Item & item, uint64_t gap);
  int GetNum(ItemQueue & itemqueue, Item & item);

private:

	static const int m_size = 10000;
	const int m_request_gap = 10000;
	const int m_car_gap = 10000;
	const int m_reQueue_max = 100;

	double m_pop;
	double m_ni;
	//Ptr<Interest> m_interest;

	uint32_t dataNum;
	uint32_t interestNum;
	uint32_t consumerID;


	//Name perReceived[m_size];

};

NS_OBJECT_ENSURE_REGISTERED (ndnFw);
//-------------------------------------------------
TypeId
ndnFw::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::fw::ndnFw")
    .SetParent<ForwardingStrategy> ()
    .SetGroupName ("Ndn")
    .AddConstructor<ndnFw> ()
    .AddAttribute ("ID",
                   "ID",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ndnFw::consumerID),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

ndnFw::ndnFw():dataNum(0), interestNum(0), m_pop(0.0), m_ni(0.0)
{	
}
ndnFw::Item::Item (const Ptr<Name> & name, const int64_t & time)
{
        m_name = name;
        m_time = time;
}
void
ndnFw::DoOnInterest (Ptr<Face> inFace,
                                Ptr<Interest> interest)
{

  //for statist
  IndexTag indexTag;
  interest->GetPayload ()->PeekPacketTag(indexTag);

  int index = indexTag.GetIndex();

  StaTag sta;
  if (interest->GetPayload()->PeekPacketTag(sta))
    {
          if (sta.GetSta() == 1)
            {
//            cout<<indexTag.GetIndex()<<endl;
            if (m_reQueue.size() >= m_reQueue_max)
              {
                      m_reQueue.pop_front();
              }
            m_reQueue.push_back(index);
            return;
            }
    }
    if (m_reQueue.size()>0)
      m_pop = (double)count(m_reQueue.begin() , m_reQueue.end() , index)/(double)m_reQueue.size();

  // Ptr<Node> node = inFace->GetNode();
  // uint32_t i = node->GetId();

  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*interest);
  bool similarInterest = true;
  if (pitEntry == 0)
    {
      similarInterest = false;

      pitEntry = m_pit->Create (interest);
      if (pitEntry != 0)
        {
          DidCreatePitEntry (inFace, interest, pitEntry);
        }
      else
        { 

          FailedToCreatePitEntry (inFace, interest);
          return;
        }
    }

  AllTag ackTag;
  bool isAck = interest->GetPayload ()->PeekPacketTag(ackTag);
  if (isAck)
    {
    PropagateInterest (inFace, interest, pitEntry);
    return;
    }

  bool isDuplicated = true;
  if (!pitEntry->IsNonceSeen (interest->GetNonce ()))
    {
      pitEntry->AddSeenNonce (interest->GetNonce ());
      isDuplicated = false;
    }

//  if (isDuplicated)
//    {
//      DidReceiveDuplicateInterest (inFace, interest, pitEntry);
//      return;
//    }

  Ptr<Data> contentObject;
  //cout<<"!!!"<<endl;
  contentObject = m_contentStore->Lookup (interest);
  if (contentObject != 0)
    { 
      //cout<<"NODE "<<i<<" NO CONTENT"<<endl;
      FwHopCountTag hopCountTag;
      if (interest->GetPayload ()->PeekPacketTag (hopCountTag))
        {
          contentObject->GetPayload ()->AddPacketTag (hopCountTag);
        }

      pitEntry->AddIncoming (inFace/*, Seconds (1.0)*/);

      // Do data plane performance measurements
      WillSatisfyPendingInterest (0, pitEntry);

      // Actually satisfy pending interest
      SatisfyPendingInterest (0, contentObject, pitEntry);
      return;
    }
     //else 
     //cout<<"NODE "<<i<<" CONTENT"<<endl;

  if (similarInterest && ShouldSuppressIncomingInterest (inFace, interest, pitEntry))
    {
      pitEntry->AddIncoming (inFace/*, interest->GetInterestLifetime ()*/);
      // update PIT entry lifetime
      pitEntry->UpdateLifetime (interest->GetInterestLifetime ());

      // Suppress this interest if we're still expecting data from some other face
      //NS_LOG_DEBUG ("Suppress interests");
      m_dropInterests (interest, inFace);

      DidSuppressSimilarInterest (inFace, interest, pitEntry);
      return;
    }

  if (similarInterest)
    {
      DidForwardSimilarInterest (inFace, interest, pitEntry);
    }

  PropagateInterest (inFace, interest, pitEntry);
}







void
ndnFw::OnInterest (Ptr<Face> inFace,
                                Ptr<Interest> interest)
{
 
  //cout<<"NODE "<<i<<" SEND"<<endl;

  DoOnInterest(inFace, interest);
}

void
ndnFw::DidReceiveUnsolicitedData (Ptr<Face> inFace,
                                               Ptr<const Data> data,
                                               bool didCreateCacheEntry)
{
  Ptr<Node> node = inFace->GetNode();
  uint32_t i = node->GetId();
  inFace->SendData (data);
}

void
ndnFw::DidReceiveSolicitedData (Ptr<Face> inFace,
                                               Ptr<const Data> data,
                                               bool didCreateCacheEntry)
{
//  Ptr<Node> node = inFace->GetNode();
//  uint32_t i = node->GetId();
//  if (i!=consumerID)
//  {
//  inFace->SendData (data);
//  }

}



void
ndnFw::OnData (Ptr<Face> inFace, Ptr<Data> data)
{	


	Ptr<Node> node = inFace->GetNode();
	uint32_t i = node->GetId();
	  StaTag sta;
	  if (data->GetPayload()->PeekPacketTag(sta))
	    {
	          if (sta.GetSta() == 1)
	            {
                    IndexTag indexTag;
                    data->GetPayload ()->PeekPacketTag(indexTag);
//                    cout<<indexTag.GetIndex()<<endl;
                     int64_t now = Simulator::Now().GetMilliSeconds();

	            }
	            return;
	    }

	  if (inFace->GetNode()->GetId() == 18)
	    cout<<"NODE seeeeeeeek "<<inFace->GetNode()->GetId()<<" receive DATA\n";
//	ForwardingStrategy::OnData (inFace, data);
	//DoOnData(inFace, data);
	  m_inData (data, inFace);

	  // Lookup PIT entry
	  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*data);
	  if (pitEntry == 0)
	    {
	      bool cached = false;

	      if (m_cacheUnsolicitedData || (m_cacheUnsolicitedDataFromApps && (inFace->GetFlags () & Face::APPLICATION)))
	        {
	          // Optimistically add or update entry in the content store
	          cached = m_contentStore->Add (data);
	        }
	      else
	        {
	          // Drop data packet if PIT entry is not found
	          // (unsolicited data packets should not "poison" content store)

	          //drop dulicated or not requested data packet
	          m_dropData (data, inFace);
	        }

	      DidReceiveUnsolicitedData (inFace, data, cached);
	      return;
	    }
	  else
	    {
	      bool cached = m_contentStore->Add (data);
	      DidReceiveSolicitedData (inFace, data, cached);
	    }

	  while (pitEntry != 0)
	    {

	    cout<<"PIT seeeeeeeek "<<inFace->GetNode()->GetId()<<" receive DATA\n";
	      // Do data plane performance measurements
	      WillSatisfyPendingInterest (inFace, pitEntry);

	      // Actually satisfy pending interest
	      SatisfyPendingInterest (inFace, data, pitEntry);

	      // Lookup another PIT entry
	      pitEntry = m_pit->Lookup (*data);
	    }
}

// void
// MyFw::DoOnData (Ptr<Face> inFace, Ptr<Data> data)
// {

// }
bool
ndnFw::TrySendOutInterest (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const Interest> interest,
                                        Ptr<pit::Entry> pitEntry)
{
  if (!CanSendOutInterest (inFace, outFace, interest, pitEntry))
    { 
      cout<<"false!!";
      return false;
    }
  // Ptr<Node> node = inFace->GetNode();
  // uint32_t i = node->GetId();
  // cout<<"NODE "<<i<<" TRY SEND"<<endl;

  pitEntry->AddOutgoing (outFace);

  //transmission--------------------------

  //transmission---------------------------
  bool successSend = outFace->SendInterest (interest);
  interestNum++;
  if (!successSend)
    {
      m_dropInterests (interest, outFace);
    }

  DidSendOutInterest (inFace, outFace, interest, pitEntry);

  return true;
}

bool
ndnFw::CanSendOutInterest (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const Interest> interest,
                                        Ptr<pit::Entry> pitEntry)
{
  // if (outFace == inFace)
  //   {
  //     // NS_LOG_DEBUG ("Same as incoming");
  //     //cout<<"SANME"<<endl;
  //     return false; // same face as incoming, don't forward
  //   }

  pit::Entry::out_iterator outgoing =
    pitEntry->GetOutgoing ().find (outFace);

  if (outgoing != pitEntry->GetOutgoing ().end ())
    {
      if (!m_detectRetransmissions)
       {cout<<"same"<<endl;
        return false; // suppress
      }
      else if (outgoing->m_retxCount >= pitEntry->GetMaxRetxCount ())
        {
  //        cout<<"same2"<<endl;
          // NS_LOG_DEBUG ("Already forwarded before during this retransmission cycle (" <<outgoing->m_retxCount << " >= " << pitEntry->GetMaxRetxCount () << ")");
  //        return false; // already forwarded before during this retransmission cycle
        }
   }

  return true;
}


bool
ndnFw::DoPropagateInterest (Ptr<Face> inFace,
                       Ptr<const Interest> interest,
                       Ptr<pit::Entry> pitEntry)
{
  //NS_LOG_FUNCTION (this);
 //cout<<endl;
//cout<<"do!"<<endl;
 int propagatedCount = 0;

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {	

    	// interest->Print(cout);
     //  cout<<endl;
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED)
      // ||
        //  metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_YELLOW)
        break; //propagate only to green faces
      // interest->Print(cout);
      // cout<<endl;
 		
      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          continue;
        }

      propagatedCount++;
      break; // propagate only one interest
    }

  return propagatedCount > 0;
}



}
}
}
