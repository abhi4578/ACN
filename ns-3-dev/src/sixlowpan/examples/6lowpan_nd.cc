#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/sixlowpan-module.h"

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
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
    }

  Packet::EnablePrinting ();
  Packet::EnableChecking ();

  NS_LOG_INFO ("Create nodes.");
  NodeContainer ln;
  ln.Create(4);
  NodeContainer lr;
  lr.Create(2);
  NodeContainer br;
  br.Create(1);

  
  #Topology
  NodeContainer net1;
  net1.Add(ln.Get(0));
  net1.Add(ln.Get(1));
  net1.Add(lr.Get(0));
  NodeContainer net2 ;
  net2.Add(ln.Get(2));
  net2.Add(ln.Get(3));
  net2.Add(lr.Get(1));
  NodeContainer net3;
  net3.Add(lr.Get(1));
  net3.Add(lr.Get(1));
  net3.Add(br.Get(0));


  NS_LOG_INFO ("Create IPv6 Internet Stack");
  InternetStackHelper internetv6;
  internetv6.Install (net1);
  internetv6.Install (net2);
  internetv6.Install (net3);

  LrWpanHelper lrWpanHelper;
   Simulator::Run ();
  Simulator::Destroy ();

}