#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("HrmaAdHocSimulation");

/**
 * HRMA Protocol Concept Logic
 * 
 * HRMA Frame format:
 * [ Synchronizing Slot | Slot 1 | Slot 2 | ... | Slot M ]
 * 
 * Each Slot i is divided into:
 * [ SYN | HR | RTS | CTS ] (for reservation) -> then Data transmission.
 * 
 * In a real ns-3 implementation, you would subclass WifiMac or create a new 
 * NetDevice that changes the channel frequency at the start of each slot.
 */

void AdvanceToNextSlot(int currentSlot, int totalSlots) {
    if (currentSlot == 0) {
        NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: [HRMA] Active on Synchronizing Slot (f0)");
    } else {
        NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: [HRMA] Active on Frequency Pair Slot " << currentSlot << " (f" << currentSlot << ")");
        // During this slot, HR, RTS, CTS, and Data happen.
    }

    int nextSlot = (currentSlot + 1) % (totalSlots + 1); // +1 because slot 0 is the sync slot
    
    // Schedule the next frequency hop / slot change (e.g., every 10ms)
    Time slotDuration = MilliSeconds(10); 
    Simulator::Schedule(slotDuration, &AdvanceToNextSlot, nextSlot, totalSlots);
}


int main (int argc, char *argv[])
{
  uint32_t numNodes = 4;
  double simulationTime = 2.0; // seconds

  CommandLine cmd;
  cmd.AddValue ("numNodes", "Number of nodes", numNodes);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.Parse (argc, argv);

  NS_LOG_UNCOND("Setting up HRMA conceptual Ad-Hoc network with " << numNodes << " nodes.");

  // 1. Create Nodes
  NodeContainer nodes;
  nodes.Create (numNodes);

  // 2. Set up Physical Layer and MAC (Using standard AdHoc WiFi as placeholder for HRMA layer)
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b); // Simple standard for baseline
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("DsssRate1Mbps"),
                                "ControlMode", StringValue ("DsssRate1Mbps"));

  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());

  // In a true HRMA implementation, this MacHelper would be replaced by your custom HrmaMacHelper
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // 3. Set up Mobility (Nodes need to be in range of each other)
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (5.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // 4. Install Internet Stack and IP Addresses
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

  // 5. Install a basic UDP application to simulate traffic that HRMA would carry
  uint16_t port = 9;
  UdpEchoServerHelper echoServer (port);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (numNodes - 1));
  serverApps.Start (Seconds (0.5));
  serverApps.Stop (Seconds (simulationTime));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (numNodes - 1), port);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.2))); // Send a packet every 0.2s
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (0.5));
  clientApps.Stop (Seconds (simulationTime));

  // 6. Start the conceptual HRMA frequency hopping scheduler
  // Assuming L = 11 frequencies, M = (11-1)/2 = 5 frequency pairs.
  int M = 5; 
  Simulator::Schedule(Seconds(0.0), &AdvanceToNextSlot, 0, M);

  // 7. Set up Performance Graphing (Gnuplot)
  GnuplotHelper plotHelper;
  plotHelper.ConfigurePlot ("hrma-throughput",
                            "HRMA Throughput vs Time",
                            "Time (Seconds)",
                            "Throughput (bytes)",
                            "png");

  // Plot throughput of node 0 sending to last node
  plotHelper.PlotProbe ("ns3::Ipv4PacketProbe",
                        "/NodeList/0/$ns3::Ipv4L3Protocol/Tx",
                        "OutputBytes",
                        "Throughput",
                        GnuplotAggregator::KEY_BELOW);

  // 8. Set up FlowMonitor to collect statistics
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // 9. Set up NetAnim Animation
  AnimationInterface anim ("hrma-animation.xml");
  anim.SetMaxPktsPerTraceFile(999999); // Ensure all packets are captured

  // 10. Run Simulation
  Simulator::Stop (Seconds (simulationTime));
  NS_LOG_UNCOND("Starting Simulation...");
  Simulator::Run ();

  // 10. Process FlowMonitor results
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetIdStats ();

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      NS_LOG_UNCOND ("Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
      NS_LOG_UNCOND ("  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024  << " Mbps");
      NS_LOG_UNCOND ("  Mean Delay: " << i->second.delaySum.GetSeconds () / i->second.rxPackets << " s");
    }

  monitor->SerializeToXmlFile ("hrma-results.xml", true, true);

  Simulator::Destroy ();
  NS_LOG_UNCOND("Simulation Finished.");

  return 0;
}
