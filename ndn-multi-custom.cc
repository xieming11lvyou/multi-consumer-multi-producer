/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2012-2013 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/ndnSIM-module.h"

#include "a-ndn-app.h"
//#include "a-ndn-consumer.h"
#include "a-ndn-fw.h" 
#include "a-ndn-simple-test.h"
#include "multi-custom-deviceface.h"
#include "ndn-netdeviceface.h"
#include "a-ndn-consumerbatches.h"
#include "ns3/ns2-mobility-helper.h"
#include "a-ndn-ndnfw.h"
#include "a-ndn-consumer.h"
#include "a-ndn-producer.h"
using namespace std;
using namespace ns3;

double Dec = 1;
uint32_t nodeNumber = 20;
uint32_t numProducer = 1;
uint32_t array[100] ;
double p = 2;
double origin = 0;

const double RANGE = 300;
Ptr<ndn::NetDeviceFace>
NdnNetDeviceFaceCallback (Ptr<Node> node, Ptr<ndn::L3Protocol> ndn, Ptr<NetDevice> device)
{
 Ptr<ndn::NetDeviceFace> face = CreateObject<MultiCustomDeviceFace> (node, device,array,numProducer,p/10.0,Dec);
 ndn->AddFace (face);
  return face;
}
void RandSeq (uint32_t n,uint32_t arr[])
{
   uint32_t i;
   uint32_t p;
   uint32_t tmp;
   for (i=0;i<=n;i++)
   {
    arr[i] = i;
   }

   for (i=n;i>0;i--)
   {
      UniformVariable m_rand1;
      p = m_rand1.GetInteger(0,i);
      tmp=arr[p];
      arr[p]=arr[i];
      arr[i]=tmp;
   }
}

int
main (int argc, char *argv[])
{
  double T=2;
  int seed = 1;
  numProducer = 1;
  nodeNumber = 20;
  Dec =0.2;

  CommandLine cmd;
  cmd.AddValue("seed","seed",seed);
  cmd.AddValue("numProducer","numProducer",numProducer);
  cmd.AddValue ("T", "T", T);
  cmd.AddValue ("p", "p", p);
  cmd.AddValue ("consumer", "consumer", array[0]);
  cmd.AddValue ("num", "num",nodeNumber );
  cmd.AddValue ("dec", "dec", Dec);
  cmd.Parse (argc,argv);

  RngSeedManager::SetSeed (seed);
  RandSeq((nodeNumber-3),array);
  array[0] = nodeNumber-2;
  array[1] =nodeNumber-1;
  array[2] = 8;
  array[3] = 10;
  stringstream ss,ss2;
  ss<<nodeNumber;
  ss2<<array[0];
  string b = ss.str();
  string a = ss2.str();
  string url = string("scratch/")+b+string(".tcl");

  //create node
  NodeContainer nodes;
  nodes.Create (nodeNumber);

  // 1. Install Wifi
  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  YansWifiChannelHelper wifiChannel;// = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel","MaxRange", DoubleValue(RANGE));
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel (wifiChannel.Create ());
  NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();
  wifiMacHelper.SetType("ns3::AdhocWifiMac");
  NetDeviceContainer wifiNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, nodes);

  // 2. Install Mobility model
//       MobilityHelper mobility;
//       double distance =20.0;
//       Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
//       int j = -1;
//       for (uint32_t i = 0; i<nodeNumber; i++)
//      {
//         j = j*(-1);
//         positionAlloc->Add (Vector (40+120*i, 80.0+10*j, 0.0));
//       }
//       mobility.SetPositionAllocator (positionAlloc);
//       mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//       mobility.Install (nodes);

  Ns2MobilityHelper ns2 = Ns2MobilityHelper (url);//5km_50node.tcl");//
  ns2.Install ();

  // 3. Install NDN stack
  ndn::StackHelper ndnHelper;
  ndnHelper.AddNetDeviceFaceCreateCallback (WifiNetDevice::GetTypeId (), MakeCallback (NdnNetDeviceFaceCallback));
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::ndnFw","ID",a);
  ndnHelper.SetContentStore ("ns3::ndn::cs::Lru", "MaxSize", "50");
  ndnHelper.SetDefaultRoutes (true);
  ndnHelper.Install (nodes);

  // 4. Set up applications
  ndn::AppHelper consumerHelper ("ns3::ndn::MyConsumer");
  consumerHelper.SetPrefix ("/test/prefix");
  consumerHelper.SetAttribute("MaxSeq", IntegerValue(0));
  consumerHelper.SetAttribute ("LifeTime", StringValue("4000s"));
  consumerHelper.SetAttribute ("StartTime", TimeValue (Seconds (T)));
  consumerHelper.Install (nodes.Get (array[0]));

  consumerHelper.SetPrefix ("/test1/prefix");
//  consumerHelper.Install (nodes.Get (array[2]));
  consumerHelper.SetPrefix ("/test2/prefix");
 // consumerHelper.Install (nodes.Get (array[3]));


  ndn::AppHelper producerHelper ("ns3::ndn::V2VProducer");
  producerHelper.SetPrefix ("/");
  producerHelper.SetAttribute ("PayloadSize", StringValue("0"));
  for(uint32_t i =1;i<numProducer+1;i++)
  {
    producerHelper.Install (nodes.Get (array[i]));
   }

  Simulator::Stop (Seconds (2000));

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
