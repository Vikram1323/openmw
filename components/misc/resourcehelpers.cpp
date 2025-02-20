#include "resourcehelpers.hpp"

#include <algorithm>
#include <sstream>
#include <string_view>

#include <components/esm/common.hpp>
#include <components/esm/refid.hpp>

#include <components/misc/pathhelpers.hpp>
#include <components/misc/strings/algorithm.hpp>
#include <components/misc/strings/lower.hpp>

#include <components/vfs/manager.hpp>

namespace
{

    struct MatchPathSeparator
    {
        bool operator()(char ch) const { return ch == '\\' || ch == '/'; }
    };

    std::string getBasename(std::string const& pathname)
    {
        return std::string(
            std::find_if(pathname.rbegin(), pathname.rend(), MatchPathSeparator()).base(), pathname.end());
    }

}

bool changeExtension(std::string& path, std::string_view ext)
{
    std::string::size_type pos = path.rfind('.');
    if (pos != std::string::npos && path.compare(pos, path.length() - pos, ext) != 0)
    {
        path.replace(pos, path.length(), ext);
        return true;
    }
    return false;
}

bool Misc::ResourceHelpers::changeExtensionToDds(std::string& path)
{
    return changeExtension(path, ".dds");
}

std::string Misc::ResourceHelpers::correctResourcePath(
    std::string_view topLevelDirectory, std::string_view resPath, const VFS::Manager* vfs)
{
    /* Bethesda at some point converted all their BSA
     * textures from tga to dds for increased load speed, but all
     * texture file name references were kept as .tga.
     */

    std::string correctedPath = Misc::StringUtils::lowerCase(resPath);

    // Apparently, leading separators are allowed
    while (correctedPath.size() && (correctedPath[0] == '/' || correctedPath[0] == '\\'))
        correctedPath.erase(0, 1);

    if (!correctedPath.starts_with(topLevelDirectory) || correctedPath.size() <= topLevelDirectory.size()
        || (correctedPath[topLevelDirectory.size()] != '/' && correctedPath[topLevelDirectory.size()] != '\\'))
        correctedPath = std::string{ topLevelDirectory } + '\\' + correctedPath;

    std::string origExt = correctedPath;

    // since we know all (GOTY edition or less) textures end
    // in .dds, we change the extension
    bool changedToDds = changeExtensionToDds(correctedPath);
    if (vfs->exists(correctedPath))
        return correctedPath;
    // if it turns out that the above wasn't true in all cases (not for vanilla, but maybe mods)
    // verify, and revert if false (this call succeeds quickly, but fails slowly)
    if (changedToDds && vfs->exists(origExt))
        return origExt;

    // fall back to a resource in the top level directory if it exists
    std::string fallback{ topLevelDirectory };
    fallback += '\\';
    fallback += getBasename(correctedPath);
    if (vfs->exists(fallback))
        return fallback;

    if (changedToDds)
    {
        fallback = topLevelDirectory;
        fallback += '\\';
        fallback += getBasename(origExt);
        if (vfs->exists(fallback))
            return fallback;
    }

    return correctedPath;
}

std::string Misc::ResourceHelpers::correctTexturePath(std::string_view resPath, const VFS::Manager* vfs)
{
    return correctResourcePath("textures", resPath, vfs);
}

std::string Misc::ResourceHelpers::correctIconPath(std::string_view resPath, const VFS::Manager* vfs)
{
    return correctResourcePath("icons", resPath, vfs);
}

std::string Misc::ResourceHelpers::correctBookartPath(std::string_view resPath, const VFS::Manager* vfs)
{
    return correctResourcePath("bookart", resPath, vfs);
}

std::string Misc::ResourceHelpers::correctBookartPath(
    std::string_view resPath, int width, int height, const VFS::Manager* vfs)
{
    std::string image = correctBookartPath(resPath, vfs);

    // Apparently a bug with some morrowind versions, they reference the image without the size suffix.
    // So if the image isn't found, try appending the size.
    if (!vfs->exists(image))
    {
        std::stringstream str;
        str << image.substr(0, image.rfind('.')) << "_" << width << "_" << height << image.substr(image.rfind('.'));
        image = Misc::ResourceHelpers::correctBookartPath(str.str(), vfs);
    }

    return image;
}

std::string Misc::ResourceHelpers::correctActorModelPath(const std::string& resPath, const VFS::Manager* vfs)
{
    std::string mdlname = resPath;
    std::string::size_type p = mdlname.find_last_of("/\\");
    if (p != std::string::npos)
        mdlname.insert(mdlname.begin() + p + 1, 'x');
    else
        mdlname.insert(mdlname.begin(), 'x');
    std::string kfname = mdlname;
    if (Misc::StringUtils::ciEndsWith(kfname, ".nif"))
        kfname.replace(kfname.size() - 4, 4, ".kf");

    if (!vfs->exists(kfname))
    {
        return resPath;
    }
    return mdlname;
}

std::string Misc::ResourceHelpers::correctMeshPath(const std::string& resPath, const VFS::Manager* vfs)
{
    return "meshes\\" + resPath;
}

std::string Misc::ResourceHelpers::correctSoundPath(std::string_view resPath, const VFS::Manager* vfs)
{
    // Workaround: Bethesda at some point converted some of the files to mp3, but the references were kept as .wav.
    if (!vfs->exists(resPath))
    {
        std::string sound{ resPath };
        changeExtension(sound, ".mp3");
        return vfs->normalizeFilename(sound);
    }
    return vfs->normalizeFilename(resPath);
}

bool Misc::ResourceHelpers::isHiddenMarker(const ESM::RefId& id)
{
    return id == "prisonmarker" || id == "divinemarker" || id == "templemarker" || id == "northmarker";
}

namespace
{
    std::string getLODMeshNameImpl(std::string resPath, const VFS::Manager* vfs, std::string_view pattern)
    {
        if (auto w = Misc::findExtension(resPath); w != std::string::npos)
            resPath.insert(w, pattern);
        return vfs->normalizeFilename(resPath);
    }

    std::string getBestLODMeshName(std::string const& resPath, const VFS::Manager* vfs, std::string_view pattern)
    {
        if (const auto& result = getLODMeshNameImpl(resPath, vfs, pattern); vfs->exists(result))
            return result;
        return resPath;
    }
}

std::string Misc::ResourceHelpers::getLODMeshName(
    int esmVersion, std::string resPath, const VFS::Manager* vfs, unsigned char lod)
{
    const std::string distantMeshPattern = [&esmVersion] {
        switch (esmVersion)
        {
            case ESM::VER_120:
            case ESM::VER_130:
                return "_dist";
            case ESM::VER_080:
            case ESM::VER_100:
                return "_far";
            case ESM::VER_094:
            case ESM::VER_170:
                return "_lod";
            default:
                return "";
        }
    }();
    for (int l = lod; l >= 0; --l)
    {
        std::stringstream patern;
        patern << distantMeshPattern << "_" << l;
        std::string const meshName = getBestLODMeshName(resPath, vfs, patern.str());
        if (meshName != resPath)
            return meshName;
    }
    return getBestLODMeshName(resPath, vfs, distantMeshPattern);
}
