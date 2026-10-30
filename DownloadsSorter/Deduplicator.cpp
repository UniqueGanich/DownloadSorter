export module Deduplicator;
import std;
import Data;
import Hasher;
import Mover;

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size = 0;
    std::filesystem::file_time_type mtime{};
};

class ThreadPool {
public:
    ThreadPool(size_t threads) : stop(false) {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
                });
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) worker.join();
    }
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

static const FileInfo& PickKeeper(const std::vector<FileInfo>& group, const std::filesystem::path& downloads) {
    const FileInfo* best = &group[0];
    for (size_t i = 1; i < group.size(); ++i) {
        const auto& curr = group[i];
        const bool curr_in_cat = curr.path.parent_path() != downloads;
        const bool best_in_cat = best->path.parent_path() != downloads;

        if (curr_in_cat != best_in_cat) {
            if (curr_in_cat) best = &curr;
            continue;
        }
        if (curr.mtime < best->mtime || (curr.mtime == best->mtime && curr.path < best->path))
            best = &curr;
    }
    return *best;
}

static std::string HashToString(const std::array<unsigned char, 32>& hash) {
    return std::string(reinterpret_cast<const char*>(hash.data()), hash.size());
}

export int Deduplicate(const std::filesystem::path& downloads, const std::vector<std::filesystem::path>& files) {
    if (files.size() < 2) return 0;

    std::vector<FileInfo> file_infos;
    std::error_code ec;
    for (const auto& p : files) {
        auto size = std::filesystem::file_size(p, ec);
        if (ec) continue;
        auto mtime = std::filesystem::last_write_time(p, ec);
        if (ec) continue;
        file_infos.push_back({ p, size, mtime });
    }

    int duplicates_moved = 0;
    std::println("── Начинаем поиск дубликатов ({} файлов) ──", file_infos.size());

    std::unordered_map<std::uintmax_t, std::vector<FileInfo>> size_groups;
    for (const auto& fi : file_infos) size_groups[fi.size].push_back(fi);

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    ThreadPool pool(num_threads);

    for (auto& [_, group] : size_groups) {
        if (group.size() < 2) continue;

        // ЭТАП 1: Быстрый хэш
        std::unordered_map<std::string, std::vector<FileInfo>> quick_groups;
        std::mutex quick_mutex;
        std::latch quick_latch(group.size());

        for (const auto& fi : group) {
            pool.enqueue([fi, &quick_groups, &quick_mutex, &quick_latch]() {
                try {
                    auto hash = GetQuickHash(fi.path);
                    if (hash) {
                        std::lock_guard lock(quick_mutex);
                        quick_groups[HashToString(*hash)].push_back(fi);
                    }
                }
                catch (...) { /* Игнорируем сбой, чтобы не зависнуть */ }
                quick_latch.count_down();
                });
        }
        quick_latch.wait();

        for (auto& [q_hash, q_group] : quick_groups) {
            if (q_group.size() < 2) continue;

            std::unordered_map<std::string, std::vector<FileInfo>> full_groups;
            std::mutex full_mutex;
            std::latch full_latch(q_group.size());

            for (const auto& fi : q_group) {
                pool.enqueue([fi, &full_groups, &full_mutex, &full_latch]() {
                    try {
                        auto hash = GetFullHash(fi.path);
                        if (hash) {
                            std::lock_guard lock(full_mutex);
                            full_groups[HashToString(*hash)].push_back(fi);
                        }
                    }
                    catch (...) { /* Игнорируем сбой */ }
                    full_latch.count_down();
                    });
            }
            full_latch.wait();

            for (auto& [f_hash, f_group] : full_groups) {
                if (f_group.size() < 2) continue;
                const FileInfo& keeper = PickKeeper(f_group, downloads);
                for (const auto& fi : f_group) {
                    if (fi.path != keeper.path && MoveFileToDuplicate(fi.path, downloads))
                        duplicates_moved++;
                }
            }
        }
    }

    std::println("── Поиск дубликатов завершен. Перемещено копий: {} ──", duplicates_moved);
    return duplicates_moved;
}