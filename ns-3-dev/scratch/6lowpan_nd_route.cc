// route over topology with classic ICMpv6 ND
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
  ln.Create(4); //  6LoWPAN nodes
  NodeContainer lr;
  lr.Create(2);  //  6LoWPAN routers
  NodeContainer br; 
  br.Create(1); // Border router

  
//   Topology
  NodeContainer net1;
  net1.Add(ln.Get(0));
  net1.Add(ln.Get(1));
  net1.Add(lr.Get(0));
  NodeContainer net2 ;
  net2.Add(ln.Get(2));
  net2.Add(ln.Get(3));
  net2.Add(lr.Get(1));
  NodeContainer net3;
  net3.Add(lr.Get(0));
  net3.Add(lr.Get(1));
  net3.Add(br.Get(0));
  NodeContainer all;
  all.Add(ln.Get(0));
  all.Add(ln.Get(1));
  all.Add(ln.Get(2));
  all.Add(ln.Get(3));
  all.Add(lr.Get(0));
  all.Add(lr.Get(1));
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
// mobility.Install();

mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (br);
mobility.Install(lr);



  LrWpanHelper lrWpanHelper;

InternetStackHelper internetv6;
internetv6.Install(all);

NetDeviceContainer d11 = lrWpanHelper.Install (net1);
  NetDeviceContainer d22 = lrWpanHelper.Install(net2);
  NetDeviceContainer d33=lrWpanHelper.Install(net3);
lrWpanHelper.AssociateToPan(d11,1);
lrWpanHelper.AssociateToPan(d22,2);
lrWpanHelper.AssociateToPan(d33,3);

SixLowPanHelper sixlowpan;
 NetDeviceContainer d1 = sixlowpan.Install (d11); 
 NetDeviceContainer d2 = sixlowpan.Install (d22); 
 NetDeviceContainer d3 = sixlowpan.Install (d33); 
  NS_LOG_INFO ("Create IPv6 Internet Stack");
  

  Ipv6AddressHelper ipv6;
  
  
  
  
  Ipv6InterfaceContainer iicr2 = ipv6.AssignWithoutAddress (d1.Get(2));
  iicr2.SetForwarding (0, true);

  NetDeviceContainer tmp4;
  tmp4.Add (d1.Get (0)); 
  tmp4.Add (d1.Get (1)); 
  Ipv6InterfaceContainer subnet1 = ipv6.AssignWithoutAddress (tmp4); 
  subnet1.Add (iicr2);
  Ipv6InterfaceContainer iicr22 = ipv6.AssignWithoutAddress (d2.Get(2)); 
  iicr22.SetForwarding (0, true);

  NetDeviceContainer tmp6;
  tmp6.Add (d2.Get (0)); 
  tmp6.Add (d2.Get (1)); 
  Ipv6InterfaceContainer subnet2 = ipv6.AssignWithoutAddress (tmp6); 
  subnet2.Add (iicr22);
  
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64)); 
  Ipv6InterfaceContainer iicr23 = ipv6.Assign(d3.Get(2)); 
  iicr23.SetForwarding (0, true);

  NetDeviceContainer tmp7;
  tmp7.Add (d3.Get (0)); 
  tmp7.Add (d3.Get (1)); 
  Ipv6InterfaceContainer subnet3 = ipv6.AssignWithoutAddress (tmp7); 
  subnet3.Add (iicr23);
  
  
  /* radvd configuration */
  RadvdHelper radvdHelper;

  radvdHelper.AddAnnouncedPrefix (subnet1.GetInterfaceIndex (2), Ipv6Address("2001:1::0"), 64);

 
    RadvdHelper radvdHelper2;
  radvdHelper2.AddAnnouncedPrefix(subnet1.GetInterfaceIndex (2), Ipv6Address("2001:1::0"), 64);
  RadvdHelper radvdHelper3;
  radvdHelper3.AddAnnouncedPrefix(subnet3.GetInterfaceIndex (2), Ipv6Address("2001:1::0"), 64);
  ApplicationContainer radvdApps = radvdHelper.Install (lr.Get(0));
  radvdApps.Add(radvdHelper2.Install(lr.Get(1)));
  radvdApps.Add(radvdHelper3.Install(br.Get(0)));
  radvdApps.Start (Seconds (1.0)); 
  radvdApps.Stop (Seconds (3600.0));
  lrWpanHelper.EnablePcapAll (std::string ("route-over-topology"), true);
internetv6.EnablePcapIpv6All(std::string ("route-over-topology"));
  
   NS_LOG_INFO ("Run Simulation.");
Simulator::Stop (Seconds (3600.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");



}
