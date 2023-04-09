#include "types.hpp"

#include <components/esm3/loadmisc.hpp>
#include <components/lua/luastate.hpp>
#include <components/misc/resourcehelpers.hpp>
#include <components/resource/resourcesystem.hpp>

#include <apps/openmw/mwbase/environment.hpp>
#include <apps/openmw/mwbase/world.hpp>
#include <apps/openmw/mwworld/esmstore.hpp>

namespace sol
{
    template <>
    struct is_automagical<ESM::Miscellaneous> : std::false_type
    {
    };
}

namespace MWLua
{
    void addMiscellaneousBindings(sol::table miscellaneous, const Context& context)
    {
        auto vfs = MWBase::Environment::get().getResourceSystem()->getVFS();

        addRecordFunctionBinding<ESM::Miscellaneous>(miscellaneous, context);

        sol::usertype<ESM::Miscellaneous> record
            = context.mLua->sol().new_usertype<ESM::Miscellaneous>("ESM3_Miscellaneous");
        record[sol::meta_function::to_string]
            = [](const ESM::Miscellaneous& rec) { return "ESM3_Miscellaneous[" + rec.mId.toDebugString() + "]"; };
        record["id"] = sol::readonly_property(
            [](const ESM::Miscellaneous& rec) -> std::string { return rec.mId.serializeText(); });
        record["name"] = sol::readonly_property([](const ESM::Miscellaneous& rec) -> std::string { return rec.mName; });
        record["model"] = sol::readonly_property([vfs](const ESM::Miscellaneous& rec) -> std::string {
            return Misc::ResourceHelpers::correctMeshPath(rec.mModel, vfs);
        });
        record["mwscript"] = sol::readonly_property(
            [](const ESM::Miscellaneous& rec) -> std::string { return rec.mScript.serializeText(); });
        record["icon"] = sol::readonly_property([vfs](const ESM::Miscellaneous& rec) -> std::string {
            return Misc::ResourceHelpers::correctIconPath(rec.mIcon, vfs);
        });
        record["isKey"] = sol::readonly_property(
            [](const ESM::Miscellaneous& rec) -> bool { return rec.mData.mFlags & ESM::Miscellaneous::Key; });
        record["value"] = sol::readonly_property([](const ESM::Miscellaneous& rec) -> int { return rec.mData.mValue; });
        record["weight"]
            = sol::readonly_property([](const ESM::Miscellaneous& rec) -> float { return rec.mData.mWeight; });
    }
}
