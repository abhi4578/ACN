# ACN

The implemented code is present at ns-3-dev/scratch/ directory. Following files correspond to following simulations:

1) 6lowpan_route_optimed.cc - route over topology with partially optimized ND
2)  6lowpan_mesh_optimed.cc -  mesh  topology with partially optimized ND
3) 6lowpan_nd_mesh.cc -  mesh over topology with classic ICMPv6 ND
4) 6lowpan_nd_route.cc - mesh topology with classic ICMPv6 ND

To run the : 
1. enter ns-3-dev folder
2. enter "./waf --run scratch/6lowpan_nd_mesh"

frame and IP packet pcap captures present in ns-3-dev/ of mesh toplogy with partially optimized ND  
