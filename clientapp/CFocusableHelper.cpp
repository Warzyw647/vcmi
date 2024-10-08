/*
 * CFocusableHelper.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "CFocusableHelper.h"
#include "../client/widgets/CTextInput.h"

void removeFocusFromActiveInput()
{
    if(CFocusable::inputWithFocus != nullptr)
        CFocusable::inputWithFocus->removeFocus();
}
