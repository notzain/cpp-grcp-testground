add_library(
  core SHARED
  core/models/net/Device.cpp
  core/net/IPv4Address.cpp
  core/net/MacAddress.cpp
  core/net/NetworkInterface.cpp
  core/net/packet/layer/EthLayerBuilder.cpp
  core/net/packet/layer/IcmpLayerBuilder.cpp
  core/net/packet/layer/IPv4LayerBuilder.cpp
  core/net/scanner/IcmpScanner.cpp
  core/net/scanner/IScanner.cpp
  core/net/snmp/Oid.cpp
  core/net/snmp/SnmpClient.cpp
  core/net/snmp/SnmpTarget.cpp
  core/net/socket/filter/IcmpFilter.cpp
  core/net/socket/filter/PacketFilter.cpp
  core/net/socket/SocketPool.cpp
  core/net/socket/v2/IcmpSocket.cpp
  core/net/socket/v2/RawSocket.cpp
  core/net/socket/v2/ReceivingSocket.cpp
  core/repo/device/DeviceRepository.cpp
  core/services/ArpScanner.cpp
  core/services/device_discovery/DeviceDiscoveryService.cpp
  core/services/device_discovery/DeviceDiscoveryTask.cpp
  core/services/device_discovery/icmp/IcmpDeviceDiscoveryTask.cpp
  core/traits/serializer/JsonSerializer.cpp
  core/util/async/Task.cpp
  core/util/logger/Logger.cpp
  core/util/Result.cpp
  core/util/Thread.cpp
)

target_include_directories(core PUBLIC .)
target_link_directories(core PUBLIC /usr/local/lib/)

target_link_libraries(
  core
  PUBLIC CONAN_PKG::boost
         CONAN_PKG::expected-lite
         CONAN_PKG::magic_enum
         CONAN_PKG::nlohmann_json
         CONAN_PKG::pcapplusplus
         CONAN_PKG::span-lite
         CONAN_PKG::spdlog
         CONAN_PKG::units
         snmp++
         proto
)

# find_program(iwyu_path NAMES include-what-you-use iwyu) if(iwyu_path)
# set_property(TARGET core PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
# else() message("Could not find the program include-what-you-use") endif()

target_precompile_headers(core PRIVATE pch.h)
# set_target_properties(core PROPERTIES UNITY_BUILD ON)
