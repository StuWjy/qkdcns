#include "ns3/qkdcns-module.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/vector.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("QkdcnsExample");

int
main (int argc, char** argv)
{
  LogComponentEnable ("SpacePointToPointChannel", LOG_LEVEL_LOGIC);
  Ptr<SpacePointToPointNetDevice> devA = CreateObject<SpacePointToPointNetDevice> ();
  Ptr<SpacePointToPointNetDevice> devB = CreateObject<SpacePointToPointNetDevice> ();
  {
    Ptr<SpacePointToPointChannel> channel = CreateObject<SpacePointToPointChannel> ();
    NS_LOG_UNCOND (channel->GetId ());
    // channel->DoAttach (devA, devB);
  }
  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}