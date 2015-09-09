/*
 * Copyright (c) 2011-2014, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "NonCopyable.hpp"
#include "BinaryStream.h"

#include <cstdint>
#include <string>
#include <vector>

class CParameterBlackboard : private utility::NonCopyable
{
public:
    // Size
    void setSize(uint32_t uiSize);
    uint32_t getSize() const;

    // Single parameter access
    void writeInteger(const void* pvSrcData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian);
    void readInteger(void* pvDstData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian) const;

    void writeString(const std::string &input, uint32_t uiOffset);
    void readString(std::string &output, uint32_t uiOffset) const;

    // Access from/to subsystems
    uint8_t* getLocation(uint32_t uiOffset);

    // Configuration handling
    void restoreFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset);
    void saveTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const;

    // Serialization
    void serialize(CBinaryStream& binaryStream);

private:
    void assertValidAccess(size_t offset, size_t size) const;

    using Blackboard = std::vector<uint8_t>;
    Blackboard mBlackboard;

    Blackboard::iterator atOffset(size_t offset) { return begin(mBlackboard) + offset; }
    Blackboard::const_iterator atOffset(size_t offset) const { return begin(mBlackboard) + offset; }
};

