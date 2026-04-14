#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("HrmaAdHocSimulation");

/* ---------------- HRMA SLOT LOGIC ---------------- */

void AdvanceToNextSlot(int currentSlot, int totalSlots) {
    if (currentSlot == 0) {
        NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: [HRMA] Sync Slot (f0)");
    } else {
        NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: [HRMA] Slot " << currentSlot << " (f" << currentSlot << ")");
        
        // Fake HRMA phases (for demo)
        NS_LOG_UNCOND("   SYN → HR → RTS → CTS → DATA");
    }

    int nextSlot = (currentSlot + 1) % (totalSlots + 1);
    Simulator::Schedule(MilliSeconds(10), &AdvanceToNextSlot, nextSlot, totalSlots);
}

/* ---------------- MAIN ---------------- */

int main (int argc, char *argv[])
{
  uint32_t numNodes = 4;
  double simulationTime = 5.0;

  CommandLine cmd;
  cmd.AddValue ("numNodes", "Number of nodes", numNodes);
  cmd.AddValue ("simulationTime", "Simulation time", simulationTime);
  cmd.Parse (argc, argv);

  NS_LOG_UNCOND("Starting HRMA Concept Simulation...");

  /* 1. Nodes */
  NodeContainer nodes;
  nodes.Create (numNodes);

  /* 2. WiFi Setup (Base layer) */
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("DsssRate1Mbps"),
                                "ControlMode", StringValue ("DsssRate1Mbps"));

  YansWifiPhyHelper phy;
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  /* 3. Mobility */
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  /* 4. Internet */
  InternetStackHelper internet;
  internet.Install (nodes);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

  /* 5. Traffic */
  uint16_t port = 9;

  UdpEchoServerHelper server (port);
  ApplicationContainer serverApp = server.Install (nodes.Get (numNodes - 1));
  serverApp.Start (Seconds (1.0));
  serverApp.Stop (Seconds (simulationTime));

  UdpEchoClientHelper client (interfaces.GetAddress (numNodes - 1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (20));
  client.SetAttribute ("Interval", TimeValue (Seconds (0.2)));
  client.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApp = client.Install (nodes.Get (0));
  clientApp.Start (Seconds (1.0));
  clientApp.Stop (Seconds (simulationTime));

  /* 6. HRMA Scheduler */
  int M = 5;
  Simulator::Schedule(Seconds(0.0), &AdvanceToNextSlot, 0, M);

  /* 7. Flow Monitor */
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  /* 8. NetAnim */
  AnimationInterface anim ("hrma-animation.xml");
  anim.SetMaxPktsPerTraceFile(999999);

  /* 9. Run */
  Simulator::Stop (Seconds (simulationTime));
  Simulator::Run ();

  /* 10. Results */
  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier =
    DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());

  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

  for (auto &flow : stats) {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (flow.first);

      NS_LOG_UNCOND("\nFlow " << flow.first);
      NS_LOG_UNCOND("Src → Dst: " << t.sourceAddress << " → " << t.destinationAddress);

      double throughput = flow.second.rxBytes * 8.0 /
        (flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024;

      NS_LOG_UNCOND("Throughput: " << throughput << " Mbps");

      if (flow.second.rxPackets > 0) {
          NS_LOG_UNCOND("Delay: " <<
            flow.second.delaySum.GetSeconds() / flow.second.rxPackets << " s");
      }
  }

  monitor->SerializeToXmlFile ("hrma-results.xml", true, true);

  Simulator::Destroy ();

  NS_LOG_UNCOND("Simulation Finished!");

  return 0;
}