export module Sorter;
import std;
import CheckPath;
import Data;

export bool sorter()
{
    auto downloads = getDownloadsPath();

    if (!downloads)
    {
        std::println(std::cerr, "Не удалось получить путь к папке «Загрузки». Выход.");
        return false;
    }

    std::println("Папка «Загрузки»: {}\n", downloads->string());
}