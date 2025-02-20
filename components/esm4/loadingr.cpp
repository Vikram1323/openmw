/*
  Copyright (C) 2016, 2018, 2021 cc9cii

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  cc9cii cc9c@iinet.net.au

  Much of the information on the data structures are based on the information
  from Tes4Mod:Mod_File_Format and Tes5Mod:File_Formats but also refined by
  trial & error.  See http://en.uesp.net/wiki for details.

*/
#include "loadingr.hpp"

#include <cstring>
#include <stdexcept>

#include "reader.hpp"
//#include "writer.hpp"

void ESM4::Ingredient::load(ESM4::Reader& reader)
{
    mId = reader.getRefIdFromHeader();
    mFlags = reader.hdr().record.flags;

    while (reader.getSubRecordHeader())
    {
        const ESM4::SubRecordHeader& subHdr = reader.subRecordHeader();
        switch (subHdr.typeId)
        {
            case ESM4::SUB_EDID:
                reader.getZString(mEditorId);
                break;
            case ESM4::SUB_FULL:
            {
                if (mFullName.empty())
                {
                    reader.getLocalizedString(mFullName);
                    break;
                }
                else // in TES4 subsequent FULL records are script effect names
                {
                    // FIXME: should be part of a struct?
                    std::string scriptEffectName;
                    if (!reader.getZString(scriptEffectName))
                        throw std::runtime_error("INGR FULL data read error");

                    mScriptEffect.push_back(scriptEffectName);

                    break;
                }
            }
            case ESM4::SUB_DATA:
            {
                // if (reader.esmVersion() == ESM::VER_094 || reader.esmVersion() == ESM::VER_170)
                if (subHdr.dataSize == 8) // FO3 is size 4 even though VER_094
                    reader.get(mData);
                else
                    reader.get(mData.weight);

                break;
            }
            case ESM4::SUB_ICON:
                reader.getZString(mIcon);
                break;
            case ESM4::SUB_MODL:
                reader.getZString(mModel);
                break;
            case ESM4::SUB_SCRI:
                reader.getFormId(mScriptId);
                break;
            case ESM4::SUB_ENIT:
                reader.get(mEnchantment);
                break;
            case ESM4::SUB_MODB:
                reader.get(mBoundRadius);
                break;
            case ESM4::SUB_SCIT:
            {
                reader.get(mEffect);
                reader.adjustFormId(mEffect.formId);
                break;
            }
            case ESM4::SUB_MODT:
            case ESM4::SUB_MODS: // Dragonborn only?
            case ESM4::SUB_EFID:
            case ESM4::SUB_EFIT:
            case ESM4::SUB_OBND:
            case ESM4::SUB_KSIZ:
            case ESM4::SUB_KWDA:
            case ESM4::SUB_VMAD:
            case ESM4::SUB_YNAM:
            case ESM4::SUB_ZNAM:
            case ESM4::SUB_ETYP: // FO3
            {
                reader.skipSubRecordData();
                break;
            }
            default:
                throw std::runtime_error("ESM4::INGR::load - Unknown subrecord " + ESM::printName(subHdr.typeId));
        }
    }
}

// void ESM4::Ingredient::save(ESM4::Writer& writer) const
//{
// }

// void ESM4::Ingredient::blank()
//{
// }
