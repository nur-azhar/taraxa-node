P2POBJECTFILES= \
	${OBJECTDIR}/libdevcore/Address.o \
	${OBJECTDIR}/libdevcore/Base64.o \
	${OBJECTDIR}/libdevcore/Common.o \
	${OBJECTDIR}/libdevcore/CommonData.o \
	${OBJECTDIR}/libdevcore/CommonIO.o \
	${OBJECTDIR}/libdevcore/CommonJS.o \
	${OBJECTDIR}/libdevcore/DBFactory.o \
	${OBJECTDIR}/libdevcore/FileSystem.o \
	${OBJECTDIR}/libdevcore/FixedHash.o \
	${OBJECTDIR}/libdevcore/Guards.o \
	${OBJECTDIR}/libdevcore/JsonUtils.o \
	${OBJECTDIR}/libdevcore/LevelDB.o \
	${OBJECTDIR}/libdevcore/Log.o \
	${OBJECTDIR}/libdevcore/LoggingProgramOptions.o \
	${OBJECTDIR}/libdevcore/MemoryDB.o \
	${OBJECTDIR}/libdevcore/OverlayDB.o \
	${OBJECTDIR}/libdevcore/RLP.o \
	${OBJECTDIR}/libdevcore/RocksDB.o \
	${OBJECTDIR}/libdevcore/SHA3.o \
	${OBJECTDIR}/libdevcore/StateCacheDB.o \
	${OBJECTDIR}/libdevcore/TransientDirectory.o \
	${OBJECTDIR}/libdevcore/TrieCommon.o \
	${OBJECTDIR}/libdevcore/TrieHash.o \
	${OBJECTDIR}/libdevcore/Worker.o \
	${OBJECTDIR}/libdevcrypto/AES.o \
	${OBJECTDIR}/libdevcrypto/Common.o \
	${OBJECTDIR}/libdevcrypto/CryptoPP.o \
	${OBJECTDIR}/libdevcrypto/Hash.o \
	${OBJECTDIR}/libdevcrypto/LibSnark.o \
	${OBJECTDIR}/libdevcrypto/SecretStore.o \
	${OBJECTDIR}/libp2p/CapabilityHost.o \
	${OBJECTDIR}/libp2p/Common.o \
	${OBJECTDIR}/libp2p/Host.o \
	${OBJECTDIR}/libp2p/Network.o \
	${OBJECTDIR}/libp2p/NodeTable.o \
	${OBJECTDIR}/libp2p/Peer.o \
	${OBJECTDIR}/libp2p/RLPXFrameCoder.o \
	${OBJECTDIR}/libp2p/RLPxHandshake.o \
	${OBJECTDIR}/libp2p/Session.o \
	${OBJECTDIR}/libp2p/UDP.o \
	${OBJECTDIR}/libp2p/UPnP.o


${OBJECTDIR}/libdevcore/Address.o: libdevcore/Address.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Address.o libdevcore/Address.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/Base64.o: libdevcore/Base64.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Base64.o libdevcore/Base64.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/Common.o: libdevcore/Common.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Common.o libdevcore/Common.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/CommonData.o: libdevcore/CommonData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/CommonData.o libdevcore/CommonData.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/CommonIO.o: libdevcore/CommonIO.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/CommonIO.o libdevcore/CommonIO.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/CommonJS.o: libdevcore/CommonJS.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/CommonJS.o libdevcore/CommonJS.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/DBFactory.o: libdevcore/DBFactory.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/DBFactory.o libdevcore/DBFactory.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/FileSystem.o: libdevcore/FileSystem.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/FileSystem.o libdevcore/FileSystem.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/FixedHash.o: libdevcore/FixedHash.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/FixedHash.o libdevcore/FixedHash.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/Guards.o: libdevcore/Guards.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Guards.o libdevcore/Guards.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/JsonUtils.o: libdevcore/JsonUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/JsonUtils.o libdevcore/JsonUtils.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/LevelDB.o: libdevcore/LevelDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/LevelDB.o libdevcore/LevelDB.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/Log.o: libdevcore/Log.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Log.o libdevcore/Log.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/LoggingProgramOptions.o: libdevcore/LoggingProgramOptions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/LoggingProgramOptions.o libdevcore/LoggingProgramOptions.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/MemoryDB.o: libdevcore/MemoryDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/MemoryDB.o libdevcore/MemoryDB.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/OverlayDB.o: libdevcore/OverlayDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/OverlayDB.o libdevcore/OverlayDB.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/RLP.o: libdevcore/RLP.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/RLP.o libdevcore/RLP.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/RocksDB.o: libdevcore/RocksDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/RocksDB.o libdevcore/RocksDB.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/SHA3.o: libdevcore/SHA3.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/SHA3.o libdevcore/SHA3.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/StateCacheDB.o: libdevcore/StateCacheDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/StateCacheDB.o libdevcore/StateCacheDB.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/TransientDirectory.o: libdevcore/TransientDirectory.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/TransientDirectory.o libdevcore/TransientDirectory.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/TrieCommon.o: libdevcore/TrieCommon.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/TrieCommon.o libdevcore/TrieCommon.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/TrieHash.o: libdevcore/TrieHash.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/TrieHash.o libdevcore/TrieHash.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcore/Worker.o: libdevcore/Worker.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcore
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcore/Worker.o libdevcore/Worker.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/AES.o: libdevcrypto/AES.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/AES.o libdevcrypto/AES.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/Common.o: libdevcrypto/Common.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/Common.o libdevcrypto/Common.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/CryptoPP.o: libdevcrypto/CryptoPP.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/CryptoPP.o libdevcrypto/CryptoPP.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/Hash.o: libdevcrypto/Hash.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/Hash.o libdevcrypto/Hash.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/LibSnark.o: libdevcrypto/LibSnark.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/LibSnark.o libdevcrypto/LibSnark.cpp $(CPPFLAGS)

${OBJECTDIR}/libdevcrypto/SecretStore.o: libdevcrypto/SecretStore.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libdevcrypto
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libdevcrypto/SecretStore.o libdevcrypto/SecretStore.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/CapabilityHost.o: libp2p/CapabilityHost.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/CapabilityHost.o libp2p/CapabilityHost.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/Common.o: libp2p/Common.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/Common.o libp2p/Common.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/Host.o: libp2p/Host.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/Host.o libp2p/Host.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/Network.o: libp2p/Network.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/Network.o libp2p/Network.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/NodeTable.o: libp2p/NodeTable.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/NodeTable.o libp2p/NodeTable.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/Peer.o: libp2p/Peer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/Peer.o libp2p/Peer.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/RLPXFrameCoder.o: libp2p/RLPXFrameCoder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/RLPXFrameCoder.o libp2p/RLPXFrameCoder.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/RLPxHandshake.o: libp2p/RLPxHandshake.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/RLPxHandshake.o libp2p/RLPxHandshake.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/Session.o: libp2p/Session.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/Session.o libp2p/Session.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/UDP.o: libp2p/UDP.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/UDP.o libp2p/UDP.cpp $(CPPFLAGS)

${OBJECTDIR}/libp2p/UPnP.o: libp2p/UPnP.cpp
	${MKDIR} -p ${OBJECTDIR}
	${MKDIR} -p ${OBJECTDIR}/libp2p
	${RM} "$@.d"
	${COMPILE} ${CXXFLAGS} "$@.d" -o ${OBJECTDIR}/libp2p/UPnP.o libp2p/UPnP.cpp $(CPPFLAGS)
