#pragma once
#include "Data/Storage/Util/BasicRecord.hpp"

namespace Serialization {

    // Helper functions for compression/decompression using LZ4
    namespace Compression {

        static bool Compress(const void* a_src, size_t a_srclen, std::vector<unsigned char>& a_dst) {

            if (a_srclen == 0) {
                a_dst.clear();
                return true;
            }

            // Get maximum compressed size needed for worst case
            int32_t maxCompressedSize = LZ4_compressBound(static_cast<int32_t>(a_srclen));
            a_dst.resize(maxCompressedSize);

            // Compress the data
            int32_t compressedSize = LZ4_compress_default(
                static_cast<const char*>(a_src),
                reinterpret_cast<char*>(a_dst.data()),
                static_cast<int32_t>(a_srclen),
                maxCompressedSize
            );

            if (compressedSize <= 0) {
                a_dst.clear();
                return false;
            }

            // Resize to actual compressed size
            a_dst.resize(compressedSize);
            return true;
        }

        static bool Decompress(const void* a_src, size_t a_srclen, std::vector<unsigned char>& a_dst, size_t a_dstlen) {
            if (a_srclen == 0) {
                a_dst.clear();
                return true;
            }

            a_dst.resize(a_dstlen);

            int32_t decompressedSize = LZ4_decompress_safe(
                static_cast<const char*>(a_src),
                reinterpret_cast<char*>(a_dst.data()),
                static_cast<int32_t>(a_srclen),
                static_cast<int32_t>(a_dstlen)
            );

            if (decompressedSize <= 0 || static_cast<size_t>(decompressedSize) != a_dstlen) {
                a_dst.clear();
                return false;
            }

            return true;
        }
    }

    template <const uint32_t uid, const uint32_t ver = 1>
    struct CompressedStringRecord {
        std::string value;
        static constexpr auto ID = std::byteswap(uid);

        CompressedStringRecord() = default;
        CompressedStringRecord(const std::string& val) : value(val) {}

        void Load(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version, uint32_t size) {
            if (type == ID) {
                logger::trace("{}: Compressed string is being read", Uint32ToStr(ID));
                if (version == ver) {
                    // Read the uncompressed string length
                    uint32_t uncompressedLength = 0;
                    if (!serializationInterface->ReadRecordData(&uncompressedLength, sizeof(uint32_t))) {
                        logger::error("{}: Failed to read uncompressed length", Uint32ToStr(ID));
                        return;
                    }

                    // Read the compressed string length
                    uint32_t compressedLength = 0;
                    if (!serializationInterface->ReadRecordData(&compressedLength, sizeof(uint32_t))) {
                        logger::error("{}: Failed to read compressed length", Uint32ToStr(ID));
                        return;
                    }

                    // Read the compressed data
                    std::vector<unsigned char> compressedData(compressedLength);
                    if (compressedLength > 0 &&
                        !serializationInterface->ReadRecordData(compressedData.data(), compressedLength)) {
                        logger::error("{}: Failed to read compressed data", Uint32ToStr(ID));
                        return;
                    }

                    // Decompress the data
                    std::vector<unsigned char> uncompressedData;
                    if (!Compression::Decompress(compressedData.data(), compressedLength,
                        uncompressedData, uncompressedLength)) {
                        logger::error("{}: Failed to decompress data", Uint32ToStr(ID));
                        return;
                    }

                    // Convert to string
                    value.assign(reinterpret_cast<char*>(uncompressedData.data()), uncompressedLength);
                    logger::trace("{}: Compressed string read OK! Original size: {} Bytes, Compressed size: {} Bytes",
                        Uint32ToStr(ID), uncompressedLength, compressedLength);
                }
            }
        }

        void Save(SKSE::SerializationInterface* serializationInterface) const {
            logger::trace("{}: Compressed string is being saved! Original length: {}", Uint32ToStr(ID), value.length());

            if (value.empty()) {
                logger::trace("{}: No data to save.", Uint32ToStr(ID));
                return;
            }

        	if (serializationInterface->OpenRecord(ID, ver)) {

            	// Store original size
                const uint32_t uncompressedLength = static_cast<uint32_t>(value.length());

                // Compress the string data
                std::vector<unsigned char> compressedData;
                if (!Compression::Compress(value.data(), uncompressedLength, compressedData)) {
                    logger::error("{}: Failed to compress string data", Uint32ToStr(ID));
                    return;
                }

                const uint32_t compressedLength = static_cast<uint32_t>(compressedData.size());
                const float compressionRatio = uncompressedLength > 0 ? (compressedLength * 100.0f / uncompressedLength) : 100.0f;

                logger::trace("{}: Compression ratio: {:.2f}%", Uint32ToStr(ID), compressionRatio);

                // Write the uncompressed size
                if (!serializationInterface->WriteRecordData(&uncompressedLength, sizeof(uint32_t))) {
                    logger::error("{}: Failed to write uncompressed length", Uint32ToStr(ID));
                    return;
                }

                // Write the compressed size
                if (!serializationInterface->WriteRecordData(&compressedLength, sizeof(uint32_t))) {
                    logger::error("{}: Failed to write compressed length", Uint32ToStr(ID));
                    return;
                }

                // Write the compressed data
                if (serializationInterface->WriteRecordData(compressedData.data(), compressedLength)) {
                    logger::trace("{}: Compressed string save OK!", Uint32ToStr(ID));
                    return;
                }

                logger::error("{}: Failed to write compressed data", Uint32ToStr(ID));
                return;
            }
            logger::error("{}: Failed to open record for compressed string", Uint32ToStr(ID));
        }
    };
}