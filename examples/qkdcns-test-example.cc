#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/vector.h"
#include "ns3/qkdcns-module.h"
#include "ns3/adi-helper.h"
#include "ns3/data-rate.h"
#include "ns3/internet-module.h"
#include "ns3/coordinate-geodetic.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("QKDCNSExample");

#define Canberra    CoordGeodetic (-35.3009,  149.1234,  0.5587,  true)
#define Pretoria    CoordGeodetic (-25.7445,   28.2287,  1.3801,  true)
#define Brasilia    CoordGeodetic (-15.8366,  -47.9065,  1.0300,  true)
#define Singapore   CoordGeodetic (  1.3034,  103.8208,  0.0288,  true)
#define KualaLumpur CoordGeodetic (  3.1596,  101.7278,  0.0471,  true)
#define Bangkok     CoordGeodetic ( 13.7627,  100.5671,  0.0032,  true)
#define Manila      CoordGeodetic ( 14.5422,  121.0297,  0.0225,  true)
#define Yangon      CoordGeodetic ( 16.7888,   96.1346,  0.0242,  true)
#define Sanya       CoordGeodetic ( 18.3130,  109.3110,  0.0180,  true)
#define HaNoi       CoordGeodetic ( 21.0324,  105.8380,  0.0184,  true)
#define MexicoCity  CoordGeodetic ( 19.3370,  -99.1978,  2.3109,  true)
#define Guangzhou   CoordGeodetic ( 23.1167,  113.2500,  0.0434,  true)
#define Riyadh      CoordGeodetic ( 24.6735,   46.6263,  0.6595,  true)
#define AbuDhabi    CoordGeodetic ( 24.4409,   54.3712,  0.0068,  true)
#define NewDelhi    CoordGeodetic ( 28.5986,   77.1897,  0.2264,  true)
#define Cairo       CoordGeodetic ( 30.0682,   31.2188,  0.0315,  true)
#define Haifa       CoordGeodetic ( 32.0917,   34.7747,  0.0171,  true)
#define Chengdu     CoordGeodetic ( 30.6667,  104.0667,  0.4850,  true)
#define Shanghai    CoordGeodetic ( 31.1094,  121.3681,  0.0040,  true)
#define Baghdad     CoordGeodetic ( 33.2976,   44.3103,  0.0402,  true)
#define Islamabad   CoordGeodetic ( 33.7344,   73.1358,  0.5610,  true)
#define Teheran     CoordGeodetic ( 35.8045,   51.4775,  1.5842,  true)
#define Seoul       CoordGeodetic ( 37.5822,  126.9710,  0.0462,  true)
#define Tokyo       CoordGeodetic ( 35.6563,  139.7272,  0.0307,  true)
#define Ankara      CoordGeodetic ( 39.8992,   32.8727,  0.9860,  true)
#define Beijing     CoordGeodetic ( 40.1172,  116.2280,  0.0380,  true)
#define Pyongyang   CoordGeodetic ( 39.0544,  125.7526,  0.3397,  true)
#define Washington  CoordGeodetic ( 38.9428,  -77.0664,  0.0982,  true)
#define Rome        CoordGeodetic ( 41.9229,   12.4996,  0.0592,  true)
#define Madrid      CoordGeodetic ( 40.4483,   -3.6417,  0.6922,  true)
#define Ottawa      CoordGeodetic ( 45.4364,  -75.6847,  0.0587,  true)
#define Berne       CoordGeodetic ( 46.9395,    7.4654,  0.5541,  true)
#define Budapest    CoordGeodetic ( 47.5093,   19.0757,  0.1061,  true)
#define Paris       CoordGeodetic ( 48.8502,    2.3158,  0.0362,  true)
#define Kyiv        CoordGeodetic ( 50.4442,   30.5407,  0.1962,  true)
#define Berlin      CoordGeodetic ( 52.5131,   13.4172,  0.0387,  true)
#define Warsaw      CoordGeodetic ( 52.2507,   21.0024,  0.1057,  true)
#define London      CoordGeodetic ( 51.5211,   -0.1457,  0.0292,  true)
#define Moscow      CoordGeodetic ( 55.7109,   37.5164,  0.1789,  true)
#define Reykjavik   CoordGeodetic ( 64.1500,  -21.9500,  0.0390,  true)

int
main (int argc, char** argv)
{
  // LogComponentEnable ("FsoChannel", LOG_LEVEL_INFO);
  // LogComponentEnable ("Turntable", LOG_LEVEL_INFO);
  LogComponentEnable ("Q3pTxCache", LOG_LEVEL_INFO);
  LogComponentEnable ("Q3pRxCache", LOG_LEVEL_INFO);
  // LogComponentEnable ("AccessManager", LOG_LEVEL_LOGIC);
  Time EndTime = Years (1.0);
  double sma = K_RE + 500.0;
  double ecc = 0.0;
  double inc = 97.406;
  double raan = 85.0;
  double aop = 0.0;
  double ma = 0.0;
  size_t N_PLANE = 3;
  size_t N_SAT = 40;
  double draan = 15;
  QkdSatelliteContainer satContainer[N_PLANE];
  QkdStationContainer staContainer;

  for (size_t i = 0;i < N_PLANE;++i)
  {
    satContainer[i].Create (N_SAT);
    for (size_t j = 0;j < N_SAT;++j)
    {
      satContainer[i][j]->SetElement (sma, ecc, inc, raan, aop, ma);
      ma += 360.0 / N_SAT;
      // _ma_ += 360.0 / 40;
    }
    raan += draan;
  }
  staContainer.Create (Canberra, "Canberra");
  staContainer.Create (Pretoria, "Pretoria");
  staContainer.Create (Brasilia, "Brasilia");
  staContainer.Create (Singapore, "Singapore");
  staContainer.Create (KualaLumpur, "KualaLumpur");
  staContainer.Create (Bangkok, "Bangkok");
  staContainer.Create (Manila, "Manila");
  staContainer.Create (Yangon, "Yangon");
  staContainer.Create (Sanya, "Sanya");
  staContainer.Create (HaNoi, "HaNoi");
  staContainer.Create (MexicoCity, "MexicoCity");
  staContainer.Create (Guangzhou, "Guangzhou");
  staContainer.Create (Riyadh, "Riyadh");
  staContainer.Create (AbuDhabi, "AbuDhabi");
  staContainer.Create (NewDelhi, "NewDelhi");
  staContainer.Create (Cairo, "Cairo");
  staContainer.Create (Haifa, "Haifa");
  staContainer.Create (Chengdu, "Chengdu");
  staContainer.Create (Shanghai, "Shanghai");
  staContainer.Create (Baghdad, "Baghdad");
  staContainer.Create (Islamabad, "Islamabad");
  staContainer.Create (Teheran, "Teheran");
  staContainer.Create (Seoul, "Seoul");
  staContainer.Create (Tokyo, "Tokyo");
  staContainer.Create (Ankara, "Ankara");
  staContainer.Create (Beijing, "Beijing");
  staContainer.Create (Pyongyang, "Pyongyang");
  staContainer.Create (Washington, "Washington");
  staContainer.Create (Rome, "Rome");
  staContainer.Create (Madrid, "Madrid");
  staContainer.Create (Ottawa, "Ottawa");
  staContainer.Create (Berne, "Berne");
  staContainer.Create (Budapest, "Budapest");
  staContainer.Create (Paris, "Paris");
  staContainer.Create (Kyiv, "Kyiv");
  staContainer.Create (Berlin, "Berlin");
  staContainer.Create (Warsaw, "Warsaw");
  staContainer.Create (London, "London");
  staContainer.Create (Moscow, "Moscow");
  staContainer.Create (Reykjavik, "Reykjavik");

  QkdDeviceHelper qkdDeviceHelper;
  QkdFsoDeviceHelper qkdFsoDeviceHelper;
  QkdNetDeviceHelper qkdNetDeviceHelper;
  qkdNetDeviceHelper.SetNetDeviceAttribute ("DataRate", DataRateValue (DataRate ("20Mb/s")));
  qkdNetDeviceHelper.SetQueueAttribute ("MaxSize", QueueSizeValue (QueueSize ("100p")));
  Ipv4AddressHelper address;
  QkdNetStackHelper netHelper;
  netHelper.Initialize ();
  // QkdAodvHelper routing;
  // routing.Set ("NodeTraversalTime", TimeValue (MilliSeconds (200)));
  // routing.Set ("NextHopWait", TimeValue (MilliSeconds (210)));
  // routing.Set ("HelloInterval", TimeValue (Seconds (5)));
  // routing.Set ("RerrRateLimit", UintegerValue (1));
  // routing.Set ("EnableHello", BooleanValue (false));
  // routing.Set ("EnableBroadcast", BooleanValue (false));   
  // netHelper.SetRoutingHelper (routing);
  netHelper.Install (satContainer[0]);
  netHelper.Install (satContainer[1]);
  netHelper.Install (satContainer[2]);
  netHelper.Install (staContainer);
  Ipv4InterfaceContainer interfaces;
  if (EnableS2G)
  {
    QkdDeviceContainer qkdDeviceContainerToStation[N_PLANE];
    qkdDeviceHelper.SetDeviceType (Tx);
    qkdDeviceHelper.SetDeviceFace (adi::Bottom);
    for (size_t i = 0;i < N_PLANE;++i)
    {
      qkdDeviceContainerToStation[i].Add (qkdDeviceHelper.Install (satContainer[i]));
    }
    QkdDeviceContainer qkdDeviceContainerFromStations;
    qkdDeviceHelper.SetDeviceType (Rx);
    qkdDeviceHelper.SetDeviceFace (adi::Top);
    qkdDeviceContainerFromStations.Add (qkdDeviceHelper.Install (staContainer));
    QkdDeviceContainer qkdDeviceContainerToStations;
    qkdDeviceContainerToStations.Add (qkdDeviceContainerToStation[0]);
    qkdDeviceContainerToStations.Add (qkdDeviceContainerToStation[1]);
    qkdDeviceContainerToStations.Add (qkdDeviceContainerToStation[2]);
    FsoDeviceContainer fsoDeviceContainerToStations = qkdFsoDeviceHelper.Install (qkdDeviceContainerToStations);
    NetDeviceContainer netDeviceContainerToStations = qkdNetDeviceHelper.Install (qkdDeviceContainerToStations);
    FsoDeviceContainer fsoDeviceContainerFromStation = qkdFsoDeviceHelper.Install (qkdDeviceContainerFromStations);
    NetDeviceContainer netDeviceContainerFromStation = qkdNetDeviceHelper.Install (qkdDeviceContainerFromStations);
    address.SetBase ("10.2.0.0", "255.255.0.0");
    interfaces.Add (address.Assign (netDeviceContainerToStations));
    address.SetBase ("10.3.0.0", "255.255.0.0");
    interfaces.Add (address.Assign (netDeviceContainerFromStation));
    AdiHelper::RegisterS2G (qkdDeviceContainerToStations, qkdDeviceContainerFromStations);
  }
  if (EnableISL)
  {
    QkdDeviceContainer qkdDeviceContainerInterPlanarLeft[N_PLANE];
    QkdDeviceContainer qkdDeviceContainerInterPlanarRight[N_PLANE];
    QkdDeviceContainer qkdDeviceContainerIntraPlanarFront[N_PLANE];
    QkdDeviceContainer qkdDeviceContainerIntraPlanarBack[N_PLANE];
    qkdDeviceHelper.SetDeviceType (Tx);
    qkdDeviceHelper.SetDeviceFace (adi::Right);
    qkdDeviceContainerInterPlanarRight[0].Add (qkdDeviceHelper.Install (satContainer[0]));
    qkdDeviceContainerInterPlanarRight[1].Add (qkdDeviceHelper.Install (satContainer[1]));
    qkdDeviceHelper.SetDeviceFace (adi::Front);
    qkdDeviceContainerIntraPlanarFront[0].Add (qkdDeviceHelper.Install (satContainer[0]));
    qkdDeviceContainerIntraPlanarFront[1].Add (qkdDeviceHelper.Install (satContainer[1]));
    qkdDeviceContainerIntraPlanarFront[2].Add (qkdDeviceHelper.Install (satContainer[2]));
    qkdDeviceHelper.SetDeviceType (Rx);
    qkdDeviceHelper.SetDeviceFace (adi::Left);
    qkdDeviceContainerInterPlanarLeft[1].Add (qkdDeviceHelper.Install (satContainer[1]));
    qkdDeviceContainerInterPlanarLeft[2].Add (qkdDeviceHelper.Install (satContainer[2]));
    qkdDeviceHelper.SetDeviceFace (adi::Back);
    qkdDeviceContainerIntraPlanarBack[0].Add (qkdDeviceHelper.Install (satContainer[0])).Shift (1);
    qkdDeviceContainerIntraPlanarBack[1].Add (qkdDeviceHelper.Install (satContainer[1])).Shift (1);
    qkdDeviceContainerIntraPlanarBack[2].Add (qkdDeviceHelper.Install (satContainer[2])).Shift (1);
    QkdDeviceContainer qkdDeviceContainerInterPlanar;
    qkdDeviceContainerInterPlanar.Add (qkdDeviceContainerInterPlanarLeft[1]);
    qkdDeviceContainerInterPlanar.Add (qkdDeviceContainerInterPlanarLeft[2]);
    qkdDeviceContainerInterPlanar.Add (qkdDeviceContainerInterPlanarRight[0]);
    qkdDeviceContainerInterPlanar.Add (qkdDeviceContainerInterPlanarRight[1]);
    QkdDeviceContainer qkdDeviceContainerIntraPlanar;
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarFront[0]);
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarFront[1]);
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarFront[2]);
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarBack[0]);
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarBack[1]);
    qkdDeviceContainerIntraPlanar.Add (qkdDeviceContainerIntraPlanarBack[2]);
    FsoDeviceContainer fsoDeviceContainerInterPlanar = qkdFsoDeviceHelper.Install (qkdDeviceContainerInterPlanar);
    FsoDeviceContainer fsoDeviceContainerIntraPlanar = qkdFsoDeviceHelper.Install (qkdDeviceContainerIntraPlanar);
    NetDeviceContainer netDeviceContainerInterPlanar = qkdNetDeviceHelper.Install (qkdDeviceContainerInterPlanar);
    NetDeviceContainer netDeviceContainerIntraPlanar = qkdNetDeviceHelper.Install (qkdDeviceContainerIntraPlanar);
    address.SetBase ("10.1.0.0", "255.255.0.0");
    interfaces.Add (address.Assign (netDeviceContainerInterPlanar));
    interfaces.Add (address.Assign (netDeviceContainerIntraPlanar));
    AdiHelper::RegisterISL (qkdDeviceContainerInterPlanarRight[0], qkdDeviceContainerInterPlanarLeft[1]);
    AdiHelper::RegisterISL (qkdDeviceContainerInterPlanarRight[1], qkdDeviceContainerInterPlanarLeft[2]);
    AdiHelper::RegisterISL (qkdDeviceContainerIntraPlanarFront[0], qkdDeviceContainerIntraPlanarBack[0]);
    AdiHelper::RegisterISL (qkdDeviceContainerIntraPlanarFront[1], qkdDeviceContainerIntraPlanarBack[1]);
    AdiHelper::RegisterISL (qkdDeviceContainerIntraPlanarFront[2], qkdDeviceContainerIntraPlanarBack[2]);
  }
  AdiHelper::Update ();
 
  Simulator::Stop (EndTime);
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}