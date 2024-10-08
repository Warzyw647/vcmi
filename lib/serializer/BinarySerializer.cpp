/*
 * BinarySerializer.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "BinarySerializer.h"

VCMI_LIB_NAMESPACE_BEGIN

BinarySerializer::BinarySerializer(IBinaryWriter * w): CSaverBase(w)
{
}

VCMI_LIB_NAMESPACE_END
