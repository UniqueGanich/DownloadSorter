# DownloadSorter

## English
### 🚀 How to run?
### WINDOWS
1. Download the _Windows archive
2. Extract the archive to any location on your PC.
3. Run the .exe file.
4. Select the “Run sorter” option.
5. As a bonus, you will get detailed statistics in the console after the process finishes.
6. DO NOT rename the folders created by the program! They will no longer be recognized as "internal" folders.
7. Deleting folders is ALLOWED. If you don't need a folder and its contents, you can delete them; it won't affect the program's operation.

### LINUX
1. Download the _Linux archive
2. Extract the archive to any location on your PC
3. Open the terminal in the folder with the downloaded file and enter the command:
```
chmod +x DownloadSorter
```
4. Run the program
5. Select the “Run sorter” option
6. As a bonus, you will get detailed statistics in the console after the process finishes
7. DO NOT rename the folders created by the program! They will no longer be recognized as "internal" folders
8. Deleting folders is ALLOWED. If you don't need a folder and its contents, you can delete them; it won't affect the program's operation

### ⚙️ How does it work?
Before moving any file, the program verifies that it is not currently locked/running, is older than 24 hours, and is not a temporary file.
It then moves the file into one of the following folders:
```
+0_Duplicate
+Archive
+Audio
+Book
+Code
+Document
+Image
+Installer
+Torrent
+Video
+_Other
```

(The + symbols at the beginning are used to keep these folders at the top of the directory list and grouped together, rather than scattered among user folders).

## Русский
### 🚀 Как запустить?
### WINDOWS
1. Скачайте архив _Windows
2. Разархивируйте архив в любое место на вашем ПК
3. Запустите .exe файл
4. Выберите пункт "Запустить сортировщик"
5. Бонусом получите подробную статистику в консоли, после завершения работы
6. Папки созданные программой переименовывать НЕЛЬЗЯ! Они перестанут считаться "своими"
7. Удалять папки - МОЖНО. Если вам не нужна папка и её содержимое - удаляйте, на работу программы это не повлияет

### LINUX
1. Скачайте архив _Linux
2. Разархивируйте архив в любое место на вашем ПК
3. Откройте терминал в папке со скачанным файлом и введите команду:
```
chmod +x DownloadSorter
```
5. Запустите программу
6. Выберите пункт "Запустить сортировщик"
7. Бонусом получите подробную статистику в консоли, после завершения работы
8. Папки созданные программой переименовывать НЕЛЬЗЯ! Они перестанут считаться "своими"
9. Удалять папки - МОЖНО. Если вам не нужна папка и её содержимое - удаляйте, на работу программы это не повлияет

### ⚙️ А как оно работает?
1. Перед перемещением файла проверяет чтобы он не был запущен, был старше 24 часов и не был временным файлом
2. Перемещает в одну из следующих папок:
```
+0_Duplicate
+Archive
+Audio
+Book
+Code
+Document
+Image
+Installer
+Torrent
+Video
+_Other
```
(Символы в начале нужны чтобы папки были выше всех остальных и были друг с другом, а не в разнобой)
Что конкретно попадёт в каждую папку вы можете посмотреть в самом низу README

### 📂 Folder Contents (What goes where)
Below is the exact list of file extensions that the program will sort into each specific folder:
```
+0_Duplicate - Identical files (duplicates) found by comparing their SHA-256 hashes.

+Archive - .7z, .bz2, .cab, .gz, .iso, .jar, .rar, .tar, .tgz, .xz, .zip

+Audio - .aac, .aiff, .alac, .flac, .m4a, .mid, .midi, .mp3, .ogg, .opus, .wav, .wma

+Book - .azw, .azw3, .cbr, .cbz, .djvu, .epub, .fb2, .lit, .mobi

+Code - .bat, .c, .cpp, .cs, .css, .go, .h, .hpp, .htm, .html, .ini, .ipynb, .java, .js, .json, .jsx, .kt, .less, .php, .ps1, .py, .rb, .rs, .sass, .scss, .sh, .sql, .svelte, .swift, .toml, .ts, .tsx, .vue, .xml, .yaml, .yml

+Document - .doc, .docx, .odp, .ods, .odt, .pdf, .ppt, .pptx, .rtf, .txt, .xls, .xlsx

+Image - .ai, .arw, .avif, .bmp, .cr2, .dng, .eps, .gif, .heic, .heif, .ico, .jpeg, .jpg, .nef, .png, .psd, .raw, .svg, .tif, .tiff, .webp

+Installer - .apk, .appimage, .appx, .appxbundle, .deb, .dmg, .exe, .ipa, .msi, .msix, .msixbundle, .pkg, .rpm

+Torrent - .torrent

+Video - .3gp, .avi, .flv, .m4v, .mkv, .mov, .mp4, .mpeg, .mpg, .ogv, .ts, .vob, .webm, .wmv

+_Other - Any file extension not listed above, or files without an extension.
```
