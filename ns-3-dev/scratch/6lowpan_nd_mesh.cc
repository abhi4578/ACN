// Mesh topology with classic ICMpv6 ND
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ExampleSixlowpan");

int main (int argc, char** argv)
{
  bool verbose = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "turn on some relevant log components", verbose);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("SixLowPanNetDevice", LOG_LEVEL_ALL);
      LogComponentEnable ("RadvdApplication", LOG_LEVEL_ALL);
    }

  Packet::EnablePrinting ();
  Packet::EnableChecking ();

  NS_LOG_INFO ("Create nodes.");
  NodeContainer ln;
  ln.Create(4);
  NodeContainer br;
  br.Create(1);

  

  NodeContainer all;
  all.Add(ln.Get(0));
  all.Add(ln.Get(1));
  all.Add(ln.Get(2));
  all.Add(ln.Get(3));
all.Add(br.Get(0));

 MobilityHelper mobility;

mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
  "MinX", DoubleValue (0.0),
  "MinY", DoubleValue (0.0),
  "DeltaX", DoubleValue (5.0),
  "DeltaY", DoubleValue (10.0),
  "GridWidth", UintegerValue (3),
  "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
  "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

mobility.Install (ln);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (br);



LrWpanHelper lrWpanHelper;
InternetStackHelper internetv6;
internetv6.Install(all);

NetDeviceContainer lrwpandevices = lrWpanHelper.Install (all);
lrWpanHelper.AssociateToPan(lrwpandevices,1);
SixLowPanHelper sixlowpan;
 NetDeviceContainer sixlowpandevices = sixlowpan.Install (lrwpandevices); 
  NS_LOG_INFO ("Create IPv6 Internet Stack");
   for (uint32_t i = 0; i < sixlowpandevices.GetN (); i++)
    {
      Ptr<NetDevice> dev = sixlowpandevices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true)); // defining the toplogy
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }

  Ipv6AddressHelper ipv6;
  
  
  NetDeviceContainer tmp4;
  tmp4.Add (sixlowpandevices.Get (0)); 
  tmp4.Add (sixlowpandevices.Get (1)); 
  tmp4.Add (sixlowpandevices.Get (2)); 
  tmp4.Add (sixlowpandevices.Get (3)); 
  Ipv6InterfaceContainer net = ipv6.AssignWithoutAddress (tmp4); 
  
  
   
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64)); 
  Ipv6InterfaceContainer iicr = ipv6.Assign(sixlowpandevices.Get(4)); 
  iicr.SetForwarding (0, true);
  net.Add (iicr);

 
  
  /* radvd configuration */
  RadvdHelper radvdHelper;

   radvdHelper.AddAnnouncedPrefix (net.GetInterfaceIndex (4), Ipv6Address("2001:1::0"), 64);

  
  ApplicationContainer radvdApps = radvdHelper.Install (br.Get(0));
  radvdApps.Start (Seconds (1.0)); 
  radvdApps.Stop (Seconds (3600.0));
  lrWpanHelper.EnablePcapAll (std::string ("mesh-topology-frames"), true);
  internetv6.EnablePcapIpv6All(std::string ("mesh-topology-ip"));
  
   NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (3600.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");



}
