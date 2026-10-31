#include "Data.h"
#include <algorithm>
#include <cctype>
#include <unordered_map>

const std::string kDuplicateFolder = "+0_Duplicate";
const std::array<std::string_view, 10> kCategoryFolders = {
    "+Archive", "+Audio", "+Book", "+Code", "+Document",
    "+Image", "+Installer", "+Torrent", "+Video", "+_Other"
};

static const std::unordered_map<std::string_view, std::string_view> kExtToFolder = {.
    // +Archive
    { ".7z", "+Archive" },{ ".bz2", "+Archive" },{ ".cab", "+Archive" },
    { ".gz", "+Archive" },{ ".iso", "+Archive" },{ ".jar", "+Archive" },
    { ".rar", "+Archive" },{ ".tar", "+Archive" },{ ".tgz", "+Archive" },
    { ".xz", "+Archive" },{ ".zip", "+Archive" },
    // +Audio
    { ".aac", "+Audio" },{ ".aiff", "+Audio" },{ ".alac", "+Audio" },
    { ".flac", "+Audio" },{ ".m4a", "+Audio" },{ ".mid", "+Audio" },
    { ".midi", "+Audio" },{ ".mp3", "+Audio" },{ ".ogg", "+Audio" },
    { ".opus", "+Audio" },{ ".wav", "+Audio" },{ ".wma", "+Audio" },
    // +Book
    { ".azw", "+Book" },{ ".azw3", "+Book" },{ ".cbr", "+Book" },
    { ".cbz", "+Book" },{ ".djvu", "+Book" },{ ".epub", "+Book" },
    { ".fb2", "+Book" },{ ".lit", "+Book" },{ ".mobi", "+Book" },
    // +Code
    { ".bat", "+Code" },{ ".c", "+Code" },{ ".cpp", "+Code" },
    { ".cs", "+Code" },{ ".css", "+Code" },{ ".go", "+Code" },
    { ".h", "+Code" },{ ".hpp", "+Code" },{ ".htm", "+Code" },
    { ".html", "+Code" },{ ".ini", "+Code" },{ ".ipynb", "+Code" },
    { ".java", "+Code" },{ ".js", "+Code" },{ ".json", "+Code" },
    { ".jsx", "+Code" },{ ".kt", "+Code" },{ ".less", "+Code" },
    { ".php", "+Code" },{ ".ps1", "+Code" },{ ".py", "+Code" },
    { ".rb", "+Code" },{ ".rs", "+Code" },{ ".sass", "+Code" },
    { ".scss", "+Code" },{ ".sh", "+Code" },{ ".sql", "+Code" },
    { ".svelte", "+Code" },{ ".swift", "+Code" },{ ".toml", "+Code" },
    { ".ts", "+Code" },{ ".tsx", "+Code" },{ ".vue", "+Code" },
    { ".xml", "+Code" },{ ".yaml", "+Code" },{ ".yml", "+Code" },
    // +Document
    { ".doc", "+Document" },{ ".docx", "+Document" },{ ".odp", "+Document" },
    { ".ods", "+Document" },{ ".odt", "+Document" },{ ".pdf", "+Document" },
    { ".ppt", "+Document" },{ ".pptx", "+Document" },{ ".rtf", "+Document" },
    { ".txt", "+Document" },{ ".xls", "+Document" },{ ".xlsx", "+Document" },
    // +Image
    { ".ai", "+Image" },{ ".arw", "+Image" },{ ".avif", "+Image" },
    { ".bmp", "+Image" },{ ".cr2", "+Image" },{ ".dng", "+Image" },
    { ".eps", "+Image" },{ ".gif", "+Image" },{ ".heic", "+Image" },
    { ".heif", "+Image" },{ ".ico", "+Image" },{ ".jpeg", "+Image" },
    { ".jpg", "+Image" },{ ".nef", "+Image" },{ ".png", "+Image" },
    { ".psd", "+Image" },{ ".raw", "+Image" },{ ".svg", "+Image" },
    { ".tif", "+Image" },{ ".tiff", "+Image" },{ ".webp", "+Image" },
    // +Installer
    { ".apk", "+Installer" },{ ".appimage", "+Installer" },{ ".appx", "+Installer" },
    { ".appxbundle", "+Installer" },{ ".deb", "+Installer" },{ ".dmg", "+Installer" },
    { ".exe", "+Installer" },{ ".ipa", "+Installer" },{ ".msi", "+Installer" },
    { ".msix", "+Installer" },{ ".msixbundle", "+Installer" },{ ".pkg", "+Installer" },
    { ".rpm", "+Installer" },
    // +Torrent
    { ".torrent", "+Torrent" },
    // +Video
    { ".3gp", "+Video" },{ ".avi", "+Video" },{ ".flv", "+Video" },
    { ".m4v", "+Video" },{ ".mkv", "+Video" },{ ".mov", "+Video" },
    { ".mp4", "+Video" },{ ".mpeg", "+Video" },{ ".mpg", "+Video" },
    { ".ogv", "+Video" },{ ".ts", "+Video" },{ ".vob", "+Video" },
    { ".webm", "+Video" },{ ".wmv", "+Video" }
};

std::string_view FindFolderForExtension(std::string_view ext) {
    if (ext.empty() || ext.size() > 15) return {};

    std::string lower(ext);
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    auto it = kExtToFolder.find(lower);
    return it != kExtToFolder.end() ? it->second : std::string_view{};
}

bool IsTemporaryExtension(std::string_view ext) {
    if (ext.empty() || ext.size() > 15) return false;
    std::string lower(ext);
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return lower == ".crdownload" || lower == ".download" ||
        lower == ".part" || lower == ".partial" || lower == ".tmp";
}