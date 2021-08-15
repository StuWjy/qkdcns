## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
#import sys

def configure(conf):
    conf.env['ENABLE_ADI'] = conf.check(
        mandatory = True,
        libpath = [
            '/home/repos/ns-3-allinone/ns-3.32/contrib/qkdcns/lib/',
            '/usr/local/cuda/lib64'],
        includes = [
            '/home/repos/ns-3-allinone/ns-3.32/contrib/qkdcns/lib'],
        lib=['adi', 'cudart_static', 'm', 'stdc++', 'dl', 'rt'],
        uselib_store='LIB_ADI'
    )

def build(bld):
    if bld.env['ENABLE_MPI']:
        module = bld.create_ns3_module('qkdcns', ['core', 'applications', 'propagation', 'point-to-point', 'wifi', 'mpi'])
    else:
        module = bld.create_ns3_module('qkdcns', ['core', 'applications', 'propagation', 'point-to-point', 'wifi'])
    module.source = [
        'model/util.cc',
        'model/constant.cc',
        #turntable
        'model/coordinate-turntable.cc',
        'model/coordinate-geodetic.cc',
        'model/turntable.cc',
        'model/bang-bang-controller.cc',
        #free-space-optics
        'model/fso-device.cc',
        'model/fso-device-container.cc',
        'model/fso-tx-device.cc',
        'model/fso-rx-device.cc',
        'model/fso-channel.cc',
        'model/fso-channel-list.cc',
        'model/fso-propagation-loss-model.cc',
        'model/fso-propagation-delay-model.cc',
        #p2p
        'model/space-point-to-point-channel.cc',
        'model/space-point-to-point-net-device.cc',
        #qkd
        'model/qkd-key.cc',
        'model/qkd-key-pool.cc',
        'model/qkd-device.cc',
        'model/qkd-device-container.cc',
        'model/qkd-node.cc',
        'model/qkd-satellite.cc',
        'model/qkd-satellite-list.cc',
        'model/qkd-satellite-container.cc',
        'model/qkd-station.cc',
        'model/qkd-station-list.cc',
        'model/qkd-station-container.cc',
        #qkd-protocol
        'model/qkd-header.cc',
        'model/qkd-tag.cc',
        'model/qnet-ipv4-l3-protocol.cc',
        'model/qkd-util.cc',
        'model/q3p-calc.cc',
        'model/q3p-cache.cc',
        'model/q3p-tx-cache.cc',
        'model/q3p-rx-cache.cc',
        'model/q3p-header.cc',
        'model/q3p-tag.cc',
        'model/q3p-l3-protocol.cc',
        'model/aodv/qkd-aodv-dpd.cc',
        'model/aodv/qkd-aodv-id-cache.cc',
        'model/aodv/qkd-aodv-neighbor.cc',
        'model/aodv/qkd-aodv-packet.cc',
        'model/aodv/qkd-aodv-routing-protocol.cc',
        'model/aodv/qkd-aodv-rqueue.cc',
        'model/aodv/qkd-aodv-rtable.cc',
        #helper
        'helper/adi-helper.cc',
        'helper/constellation-helper.cc',
        'helper/qkd-device-helper.cc',
        'helper/qkd-fso-device-helper.cc',
        'helper/qkd-net-device-helper.cc',
        'helper/qkd-net-stack-helper.cc',
        'helper/qkd-aodv-helper.cc',
        'helper/access-manager.cc',
        ]

    module_test = bld.create_ns3_module_test_library('qkdcns')
    module_test.source = [
        ]
    # Tests encapsulating example programs should be listed here
    if (bld.env['ENABLE_EXAMPLES']):
        module_test.source.extend([
           #'test/model-j2-test.cc',
            ])
            
    module.use.append("LIB_ADI")
    headers = bld(features='ns3header')
    headers.module = 'qkdcns'
    headers.source = [
        'model/util.h',
        'model/constant.h',
        #turntable
        'model/coordinate-turntable.h',
        'model/coordinate-geodetic.h',
        'model/turntable.h',
        'model/bang-bang-controller.h',
        #free-space-optics
        'model/fso-device.h',
        'model/fso-device-container.h',
        'model/fso-tx-device.h',
        'model/fso-rx-device.h',
        'model/fso-channel.h',
        'model/fso-channel-list.h',
        'model/fso-propagation-loss-model.h',
        'model/fso-propagation-delay-model.h',
        #p2p
        'model/space-point-to-point-channel.h',
        'model/space-point-to-point-net-device.h',
        #qkd
        'model/qkd-key.h',
        'model/qkd-key-pool.h',
        'model/qkd-device.h',
        'model/qkd-device-container.h',
        'model/qkd-node.h',
        'model/qkd-satellite.h',
        'model/qkd-satellite-list.h',
        'model/qkd-satellite-container.h',
        'model/qkd-station.h',
        'model/qkd-station-list.h',
        'model/qkd-station-container.h',
        #qkd-protocol
        'model/qkd-header.h',
        'model/qkd-tag.h',
        'model/qnet-ipv4-l3-protocol.h',
        'model/qkd-util.h',
        'model/q3p-calc.h',
        'model/q3p-cache.h',
        'model/q3p-tx-cache.h',
        'model/q3p-rx-cache.h',
        'model/q3p-header.h',
        'model/q3p-tag.h',
        'model/q3p-l3-protocol.h',
        'model/aodv/qkd-aodv-dpd.h',
        'model/aodv/qkd-aodv-id-cache.h',
        'model/aodv/qkd-aodv-neighbor.h',
        'model/aodv/qkd-aodv-packet.h',
        'model/aodv/qkd-aodv-routing-protocol.h',
        'model/aodv/qkd-aodv-rqueue.h',
        'model/aodv/qkd-aodv-rtable.h',
        #helper
        'helper/adi-helper.h',
        'helper/constellation-helper.h',
        'helper/qkd-device-helper.h',
        'helper/qkd-fso-device-helper.h',
        'helper/qkd-net-device-helper.h',
        'helper/qkd-net-stack-helper.h',
        'helper/qkd-aodv-helper.h',
        'helper/access-manager.h',
        #headers
        ]

    if (bld.env['ENABLE_EXAMPLES']):
        bld.recurse('examples')


   # bld.ns3_python_bindings()
