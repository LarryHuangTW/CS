**快閃記憶體 (Flash memory)：是一種非揮發性的記憶體儲存裝置 (non-volatile memory storage device, NVRAM)。**

* NOR flash:
   + 演進：ROM -> PROM -> EPROM -> EEPROM -> NOR flash memory
   + 用途：儲存於主機板上的韌體 (UEFI BIOS)；SDRAM 上有 SPD EEPROM ...
   + 分類：
      - Serial   NOR flash
      - Parallel NOR flash

* NAND flash:
   + 用途：
      - SSD (solid-state drive)
      - USB flash drive
      - SD card (secure digital card)
         - Standard SD card
         - Mini     SD card
         - Micro    SD card
   + 分類：SLC, MLC, TLC, QLC NAND flash

* MMC (MultiMedia Card), eMMC (embedded MMC)

<br>

所謂 NOR flash 具有 XIP (eXecute In Place, 原地執行) 的能力，是指 NOR flash 的結構與 NAND flash 有些許不同，

使得 NOR flash 支援隨機存取，其讀取資料的方式類似 RAM，資料讀取速度也相較 NAND flash 為快，

因此在運作上，當要執行儲存於 NOR flash 中的程式時，可以不必複製到 DRAM 中，

而是由 NOR flash 直接載入到 CPU 的 Cache 中開始執行，例如現行的 UEFI BIOS 即以此種模式運行。
