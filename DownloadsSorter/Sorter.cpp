export module Sorter;
import std;
import CheckPath;

export void sorter()
{
    auto downloads = getDownloadsPath();

    if (!downloads)
    {
        std::println(std::cerr, "Не удалось получить путь к папке «Загрузки». Выход.");
        return;
    }

    std::println("Папка «Загрузки»: {}", downloads->string());
}