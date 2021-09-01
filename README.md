# qkdcns
Quantum key distribution constellation network simulation module based on network simulator ns-3.
This project is to simulate qkds of satellite-to-ground and inter-satellite.
For now, the routing protocol of qkdcns is still not be deployed.
How to use it
1.Install the ns3, please reference https://www.nsnam.org/docs/release/3.32/tutorial/html/index.html.
2.We provided the module of qkdcns instead of whole ns3.
To build the qkdcns module with ns3, please move the content we provided to /ns-3.32/contrib/ and then build it.
3.To build the set of default libraries and the example programs included in this package, you need to use the tool 'waf'. Detailed information on how to use waf is included in the file doc/build.txt
However, the real quick and dirty way to get started is to type the command ./waf configure --enable-examples followed by ./waf in the directory which contains this README file. The files built will be copied in the build/ directory.
4.On recent Linux systems, once you have built ns-3 (with examples enabled), it should be easy to run the sample programs with the following command, such as:
./waf --run qkdcns-test-example
That program should generate a S2G.log text file.
The program source can be found in the contrib/examples/ directory.

For the adi module used in it, please reference https://github.com/StuWjy/Adi.git.
