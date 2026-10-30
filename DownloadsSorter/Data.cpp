export module Data;

import std;

export inline constexpr std::wstring_view kDuplicateFolder = L"+0_Duplicate";

export inline constexpr std::array<std::wstring_view, 10> kCategoryFolders = {
    L"+Archive", L"+Audio", L"+Book", L"+Code", L"+Document",
    L"+Image",   L"+Installer", L"+Torrent", L"+Video", L"+_Other",
};

export inline const std::unordered_map<std::wstring_view, std::wstring_view> kExtToFolder = {
    // +Archive
    { L".7z", L"+Archive" },{ L".bz2", L"+Archive" },{ L".cab", L"+Archive" },
    { L".gz", L"+Archive" },{ L".iso", L"+Archive" },{ L".jar", L"+Archive" },
    { L".rar", L"+Archive" },{ L".tar", L"+Archive" },{ L".tgz", L"+Archive" },
    { L".xz", L"+Archive" },{ L".zip", L"+Archive" },

    // +Audio
    { L".aac", L"+Audio" },{ L".aiff", L"+Audio" },{ L".alac", L"+Audio" },
    { L".flac", L"+Audio" },{ L".m4a", L"+Audio" },{ L".mid", L"+Audio" },
    { L".midi", L"+Audio" },{ L".mp3", L"+Audio" },{ L".ogg", L"+Audio" },
    { L".opus", L"+Audio" },{ L".wav", L"+Audio" },{ L".wma", L"+Audio" },

    // +Book
    { L".azw", L"+Book" },{ L".azw3", L"+Book" },{ L".cbr", L"+Book" },
    { L".cbz", L"+Book" },{ L".djvu", L"+Book" },{ L".epub", L"+Book" },
    { L".fb2", L"+Book" },{ L".lit", L"+Book" },{ L".mobi", L"+Book" },

    // +Code
    { L".bat", L"+Code" },{ L".c", L"+Code" },{ L".cpp", L"+Code" },
    { L".cs", L"+Code" },{ L".css", L"+Code" },{ L".go", L"+Code" },
    { L".h", L"+Code" },{ L".hpp", L"+Code" },{ L".htm", L"+Code" },
    { L".html", L"+Code" },{ L".ini", L"+Code" },{ L".ipynb", L"+Code" },
    { L".java", L"+Code" },{ L".js", L"+Code" },{ L".json", L"+Code" },
    { L".jsx", L"+Code" },{ L".kt", L"+Code" },{ L".less", L"+Code" },
    { L".php", L"+Code" },{ L".ps1", L"+Code" },{ L".py", L"+Code" },
    { L".rb", L"+Code" },{ L".rs", L"+Code" },{ L".sass", L"+Code" },
    { L".scss", L"+Code" },{ L".sh", L"+Code" },{ L".sql", L"+Code" },
    { L".svelte", L"+Code" },{ L".swift", L"+Code" },{ L".toml", L"+Code" },
    { L".ts", L"+Code" },{ L".tsx", L"+Code" },{ L".vue", L"+Code" },
    { L".xml", L"+Code" },{ L".yaml", L"+Code" },{ L".yml", L"+Code" },

    // +Document
    { L".doc", L"+Document" },{ L".docx", L"+Document" },{ L".odp", L"+Document" },
    { L".ods", L"+Document" },{ L".odt", L"+Document" },{ L".pdf", L"+Document" },
    { L".ppt", L"+Document" },{ L".pptx", L"+Document" },{ L".rtf", L"+Document" },
    { L".txt", L"+Document" },{ L".xls", L"+Document" },{ L".xlsx", L"+Document" },

    // +Image
    { L".ai", L"+Image" },{ L".arw", L"+Image" },{ L".avif", L"+Image" },
    { L".bmp", L"+Image" },{ L".cr2", L"+Image" },{ L".dng", L"+Image" },
    { L".eps", L"+Image" },{ L".gif", L"+Image" },{ L".heic", L"+Image" },
    { L".heif", L"+Image" },{ L".ico", L"+Image" },{ L".jpeg", L"+Image" },
    { L".jpg", L"+Image" },{ L".nef", L"+Image" },{ L".png", L"+Image" },
    { L".psd", L"+Image" },{ L".raw", L"+Image" },{ L".svg", L"+Image" },
    { L".tif", L"+Image" },{ L".tiff", L"+Image" },{ L".webp", L"+Image" },

    // +Installer
    { L".apk", L"+Installer" },{ L".appimage", L"+Installer" },{ L".appx", L"+Installer" },
    { L".appxbundle", L"+Installer" },{ L".deb", L"+Installer" },{ L".dmg", L"+Installer" },
    { L".exe", L"+Installer" },{ L".ipa", L"+Installer" },{ L".msi", L"+Installer" },
    { L".msix", L"+Installer" },{ L".msixbundle", L"+Installer" },{ L".pkg", L"+Installer" },
    { L".rpm", L"+Installer" },

    // +Torrent
    { L".torrent", L"+Torrent" },

    // +Video
    { L".3gp", L"+Video" },{ L".avi", L"+Video" },{ L".flv", L"+Video" },
    { L".m4v", L"+Video" },{ L".mkv", L"+Video" },{ L".mov", L"+Video" },
    { L".mp4", L"+Video" },{ L".mpeg", L"+Video" },{ L".mpg", L"+Video" },
    { L".ogv", L"+Video" },{ L".ts", L"+Video" },{ L".vob", L"+Video" },
    { L".webm", L"+Video" },{ L".wmv", L"+Video" },
};

export inline constexpr std::array<std::wstring_view, 5> kTemporaryExts = {
    L".crdownload", L".download", L".part", L".partial", L".tmp",
};

export inline std::wstring_view FindFolderForExtension(std::wstring_view ext) noexcept {
    if (ext.empty() || ext.size() > 15) return {};

    wchar_t lower_buf[16] = {}; // <-- ИСПРАВЛЕНО: Добавлена инициализация нулями
    for (size_t i = 0; i < ext.size(); ++i) {
        wchar_t c = ext[i];
        if (c >= L'A' && c <= L'Z') c = c + (L'a' - L'A');
        lower_buf[i] = c;
    }

    auto it = kExtToFolder.find(std::wstring_view(lower_buf, ext.size()));
    return it != kExtToFolder.end() ? it->second : std::wstring_view{};
}

export inline bool IsTemporaryExtension(std::wstring_view ext) noexcept {
    if (ext.empty() || ext.size() > 15) return false;

    wchar_t lower_buf[16] = {}; // <-- ИСПРАВЛЕНО: Добавлена инициализация нулями
    for (size_t i = 0; i < ext.size(); ++i) {
        wchar_t c = ext[i];
        if (c >= L'A' && c <= L'Z') c = c + (L'a' - L'A');
        lower_buf[i] = c;
    }

    std::wstring_view lower(lower_buf, ext.size());
    for (auto tmp : kTemporaryExts)
        if (tmp == lower) return true;
    return false;
}