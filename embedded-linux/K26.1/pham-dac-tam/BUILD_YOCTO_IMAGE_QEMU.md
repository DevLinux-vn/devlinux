# Hướng dẫn Build Image QEMU với Yocto Poky

Tài liệu này hướng dẫn cách setup và build image Linux cho QEMU từ đầu sử dụng Yocto Project.

## Yêu cầu hệ thống

- **OS**: Ubuntu 22.04 LTS hoặc cao hơn (hoặc các distro Linux tương tự)
- **Dung lượng disk**: Tối thiểu 50GB (khuyến nghị 100GB)
- **RAM**: Tối thiểu 8GB (khuyến nghị 10GB)
- **CPU**: 4 cores trở lên

## Bước 1: Cài đặt Dependencies

**Phiên bản các gói (đã kiểm chứa ổn định):**

| Gói | Phiên bản |
|---|---|
| gawk | 1:5.1.0-1ubuntu0.1 |
| wget | 1.21.2-2ubuntu1.1 |
| git | 1:2.34.1-1ubuntu1.17 |
| diffstat | 1.64-1build2 |
| unzip | 6.0-26ubuntu3.2 |
| texinfo | 6.8-4build1 |
| build-essential | 12.9ubuntu3 |
| chrpath | 0.16-2 |
| socat | 1.7.4.1-3ubuntu4 |
| cpio | 2.13+dfsg-7ubuntu0.1 |
| python3 | 3.10.6-1~22.04.1 |
| python3-pip | 22.0.2+dfsg-1ubuntu0.7 |
| python3-pexpect | 4.8.0-2ubuntu1 |
| xz-utils | 5.2.5-2ubuntu1.1 |
| debianutils | 5.5-1ubuntu2 |
| iputils-ping | 3:20211215-1ubuntu0.1 |
| libsdl1.2-dev | 1.2.15+dfsg2-6 |
| xterm | 372-1ubuntu1 |
| python3-subunit | 1.4.0-3 |
| mesa-common-dev | 23.2.1-1ubuntu3.1~22.04.4 |
| git-lfs | 3.0.2-1ubuntu0.3 |

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    gawk=1:5.1.0-1ubuntu0.1 \
    wget=1.21.2-2ubuntu1.1 \
    git=1:2.34.1-1ubuntu1.17 \
    diffstat=1.64-1build2 \
    unzip=6.0-26ubuntu3.2 \
    texinfo=6.8-4build1 \
    build-essential=12.9ubuntu3 \
    chrpath=0.16-2 \
    socat=1.7.4.1-3ubuntu4 \
    cpio=2.13+dfsg-7ubuntu0.1 \
    python3=3.10.6-1~22.04.1 \
    python3-pip=22.0.2+dfsg-1ubuntu0.7 \
    python3-pexpect=4.8.0-2ubuntu1 \
    xz-utils=5.2.5-2ubuntu1.1 \
    debianutils=5.5-1ubuntu2 \
    iputils-ping=3:20211215-1ubuntu0.1 \
    libsdl1.2-dev=1.2.15+dfsg2-6 \
    xterm=372-1ubuntu1 \
    python3-subunit=1.4.0-3 \
    mesa-common-dev=23.2.1-1ubuntu3.1~22.04.4 \
    git-lfs=3.0.2-1ubuntu0.3
```

**Lưu ý**: Cài đặt phiên bản cụ thể giúp đảm bảo build ổn định và reproducible trên tất cả máy.

## Bước 2: Clone các repositories

Tạo thư mục làm việc:

```bash
mkdir -p ~/yocto-build
cd ~/yocto-build
```

### 2.1 Clone Poky (main build system)

```bash
git clone https://git.yoctoproject.org/poky.git
cd poky
git checkout 5049bc18a8
cd ..
```

**Commit sử dụng**: `5049bc18a8` (Phiên bản Whinlatter - sau release)

**Tại sao phải checkout commit cụ thể?**

Checkout commit cụ thể giúp:
- ✅ **Reproducible Build**: Mọi người build được kết quả y hệt, không phụ thuộc vào khi nào clone
- ✅ **Stable Version**: Commit này đã được kiểm chứng, biết là hoạt động tốt
- ✅ **Avoid Breaking Changes**: Master branch có thể có changes chưa ổn định từ commits mới nhất
- ✅ **Track Version**: Dễ dàng quay lại phiên bản cũ nếu gặp vấn đề

**Nếu muốn dùng master branch mới nhất (risk):**
```bash
cd poky
# Bỏ qua git checkout - sử dụng master branch hiện tại
cd ..
```

#### Poky là gì?

**Poky** là **integrated build system** chính thức của Yocto Project. Nó là bộ tổng hợp đầy đủ cho embedded Linux development.

**Poky chứa:**
- 📦 **BitBake** — Build engine (công cụ build chính)
- 📚 **OpenEmbedded-Core (OE-Core)** — Core recipes & metadata
- 🏗️ **Yocto documentation** — Tài liệu, hướng dẫn
- 🎯 **meta-poky** — Poky distribution configuration
- 📡 **meta-yocto-bsp** — Board Support Packages (generic)
- 🔧 **Scripts & tools** — Build utilities, helpers

**Ý nghĩa:**
- ✅ Là **base layer** không thể thiếu
- ✅ Cung cấp những packages **cơ bản nhất** (libc, kernel, bootloader...)
- ✅ Là **reference implementation** của Yocto Project
- ✅ Được bảo trì chính thức bởi Yocto Project

**So sánh**: Nếu coi Yocto Project là một framework, thì **Poky là concrete implementation sẵn sàng sử dụng ngay**.

**Tương tự**: Nếu Ubuntu là Linux distro, Poky cũng là một "embedded Linux distro" sẵn sàng cho development.

---

### 2.2 Clone meta-openembedded (trong poky)

```bash
cd poky
git clone https://github.com/openembedded/meta-openembedded.git
cd meta-openembedded
git checkout 2de5071f9a
cd ..
```

**Commit sử dụng**: `2de5071f9a` (klibc: Use libgcc for compiler runtime)

#### meta-openembedded là gì?

**meta-openembedded** là một **collection of optional layers** cung cấp hàng ngàn recipes bổ sung cho Poky.

**meta-openembedded chứa:**
- 📦 **meta-oe** — Recipes cho OpenEmbedded (chính)
- 🐍 **meta-python** — Python packages & libraries
- 🔤 **meta-perl** — Perl packages & libraries
- 📱 **meta-gnome** — GNOME desktop packages
- 🖥️ **meta-xfce** — XFCE desktop packages
- 📊 **meta-multimedia** — Media libraries (ffmpeg, vlc...)
- 🔐 **meta-security** — Security tools & libs
- 🌐 **meta-networking** — Networking tools & applications
- ... và hơn 50 layers khác

**Ý nghĩa:**
- ✅ **Bổ sung** cho Poky những packages không có sẵn
- ✅ Cung cấp **optional features** & applications
- ✅ Được **bảo trì bởi cộng đồng** OpenEmbedded
- ✅ Không bắt buộc, nhưng **rất hữu ích** cho development
- ✅ Tiếp cận nhiều **3rd-party libraries & tools**

**Ví dụ**: 
- Poky cung cấp: bootloader, kernel, libc, shell, coreutils
- meta-openembedded cung cấp: vim, curl, git, python, perl, ffmpeg, openssl...

---

#### Mối quan hệ giữa Poky và meta-openembedded:

```
┌─────────────────────────────────────────┐
│  Yocto Project (Framework)              │
│  ├─ Build system & tools                │
│  └─ Documentation & standards           │
└─────────────────────────────────────────┘
          ▼
┌─────────────────────────────────────────┐
│  Poky (Base Layer - Bắt buộc)           │
│  ├─ BitBake build engine                │
│  ├─ OE-Core recipes (essential)         │
│  └─ Basic Linux distro                  │
└─────────────────────────────────────────┘
          ▼
┌─────────────────────────────────────────┐
│  meta-openembedded (Optional Layers)    │
│  ├─ Extra recipes (thousands)           │
│  ├─ Applications & tools                │
│  └─ Extended functionality              │
└─────────────────────────────────────────┘
```

**Tương tự với Ubuntu:**
- **Ubuntu Core** = Poky (base system)
- **Ubuntu Universe/Multiverse repos** = meta-openembedded (extra packages)

---

#### Tóm tắt:

| Thành phần | Vai trò | Bắt buộc? | Bảo trì |
|-----------|--------|----------|--------|
| **Poky** | Base build system + essential packages | ✅ Bắt buộc | Yocto Project |
| **meta-openembedded** | Extra packages & applications | ❌ Tùy chọn | OpenEmbedded community |

**Trong project này:**
- ✅ Poky là nền tảng build
- ✅ meta-openembedded cung cấp thêm tools (vim, curl, git, python...)

### Kiểm tra cấu trúc thư mục

```bash
# Từ thư mục ~/yocto-build
ls -la

# Nên thấy:
# drwxrwxr-x  poky/

# Sau đó vào poky
cd poky
ls -la

# Nên thấy:
# drwxrwxr-x  meta/
# drwxrwxr-x  meta-poky/
# drwxrwxr-x  meta-yocto-bsp/
# drwxrwxr-x  meta-openembedded/
```

## Bước 3: Khởi tạo Build Environment

```bash
cd poky
source oe-init-build-env build-qemu
```

**Lưu ý**: Lệnh này sẽ:

**Nếu thư mục chưa tạo:**
- ✅ Tạo thư mục `../build-qemu/` mới
- ✅ Copy template configuration files (`local.conf`, `bblayers.conf`)
- ✅ Cài đặt shell environment (PATH, PYTHONPATH, v.v.)
- ✅ Hiển thị guide "Hãy chỉnh sửa conf/local.conf..."

**Nếu thư mục đã tạo:**
- ✅ Reuse cấu hình cũ (không ghi đè `local.conf`, `bblayers.conf`)
- ✅ Chỉ cài đặt shell environment lại
- ✅ Giữ nguyên các thay đổi cấu hình trước đó
- ℹ️ Có thể chạy lại lệnh này mà không sợ mất cấu hình

## Bước 4: Cấu hình Build

### 4.1 Mở file local.conf

```bash
# Hiện đang ở trong build directory
nano conf/local.conf
```

### 4.2 Cấu hình cơ bản (các dòng cần sửa)

**a) Chọn Machine (QEMU target)**

Tìm dòng `MACHINE ?= ...` và sửa thành một trong các option sau.

#### **Cách Tìm Các Machine Values:**

**Machine values chính là tên file `.conf` (bỏ đuôi) trong `poky/meta/conf/machine/`**

Để xem tất cả available machines:

```bash
# Cách 1: List files trong machine directory
ls poky/meta/conf/machine/ | grep -E "\.conf$" | sed 's/\.conf$//'

# Cách 2: Xem chi tiết files
ls -lh poky/meta/conf/machine/*.conf

# Cách 3: Dùng bitbake-layers (sau khi init build env)
cd poky
source oe-init-build-env ../build-qemu
bitbake-layers show-machines
```

**Output ví dụ:**
```
qemuarm.conf          → MACHINE = "qemuarm"
qemuarm64.conf        → MACHINE = "qemuarm64"
qemuarmv5.conf        → MACHINE = "qemuarmv5"
qemuloongarch64.conf  → MACHINE = "qemuloongarch64"
qemumips.conf         → MACHINE = "qemumips"
qemumips64.conf       → MACHINE = "qemumips64"
qemuoppc.conf         → MACHINE = "qemuoppc"
qemuoppc64.conf       → MACHINE = "qemuoppc64"
qemurcscv32.conf      → MACHINE = "qemurcscv32"
qemurcscv64.conf      → MACHINE = "qemurcscv64"
qemux86.conf          → MACHINE = "qemux86"
qemux86-64.conf       → MACHINE = "qemux86-64"
```

---

#### Các tùy chọn MACHINE:

**1. `MACHINE ?= "qemuarm"` — ARM 32-bit**
- **Kiến trúc**: ARMv7 (32-bit ARM)
- **CPU mô phỏng**: Cortex-A9
- **Kích thước image**: ~100-200MB
- **Tốc độ build**: Nhanh
- **Dùng khi**: 
  - Muốn test cho ARM 32-bit devices (embedded systems, older devices)
  - Device chạy ARM legacy architecture
  - Học tập về ARM 32-bit

**2. `MACHINE ?= "qemuarm64"` — ARM 64-bit**
- **Kiến trúc**: ARMv8 (64-bit ARM)
- **CPU mô phỏng**: Cortex-A53
- **Kích thước image**: ~150-250MB
- **Tốc độ build**: Trung bình
- **Dùng khi**:
  - Muốn test cho ARM 64-bit devices (modern embedded systems)
  - Raspberry Pi 4, NVIDIA Jetson, servers ARM
  - Cần memory lớn hơn 4GB
  - **Khuyến nghị** cho hầu hết trường hợp

**3. `MACHINE ?= "qemux86-64"` — x86 64-bit** ⭐ (Đang dùng)
- **Kiến trúc**: x86-64 (Intel/AMD 64-bit)
- **CPU mô phỏng**: x86-64
- **Kích thước image**: ~100-200MB
- **Tốc độ build**: Nhanh nhất
- **Tốc độ chạy**: Nhanh nhất (vì host cũng là x86-64)
- **Dùng khi**:
  - Muốn test cho x86 servers/PCs
  - Máy host là x86-64 (tối ưu hiệu suất)
  - Muốn performance cao trong QEMU
  - **Lựa chọn tốt cho development/testing**

**4. `MACHINE ?= "qemux86"` — x86 32-bit**
- **Kiến trúc**: x86 (Intel 32-bit)
- **CPU mô phỏng**: Pentium
- **Kích thước image**: ~100-150MB
- **Tốc độ build**: Nhanh
- **Dùng khi**:
  - Muốn test cho x86 legacy architecture
  - Test software chạy trên 32-bit x86

#### So sánh nhanh:

| Machine | Arch | Bit | Tốc độ Build | Tốc độ Chạy | Use Case |
|---------|------|-----|-------------|------------|----------|
| qemuarm | ARM | 32 | ⚡⚡ | ⚡ | Embedded ARM cũ |
| qemuarm64 | ARM | 64 | ⚡⚡ | ⚡⚡ | Modern ARM devices |
| **qemux86-64** | x86 | 64 | ⚡⚡⚡ | ⚡⚡⚡ | **x86 servers (hiện tại)** |
| qemux86 | x86 | 32 | ⚡⚡ | ⚡ | x86 legacy |

#### Recommendation:
```
# Máy host là x86-64 nên chọn qemux86-64 (phù hợp nhất)
MACHINE ?= "qemux86-64"
```

Vì:
- ✅ Host machine cũng là x86-64 → **không cần emulation/translation** → **nhanh nhất**
- ✅ Build nhanh, QEMU chạy mượt mà
- ✅ Phù hợp test server/desktop applications trên x86-64

**Lưu ý**: Nếu máy host là ARM, hãy chọn `qemuarm` hoặc `qemuarm64` thay vào

**b) Tối ưu hóa build (tùy chọn)**

Thêm/uncomment các dòng sau:

```bash
# Tăng số thread build (đặt bằng số CPU cores)
BB_NUMBER_THREADS = "4"
PARALLEL_MAKE = "-j 4"
```

#### ⚡ **CẤU HÌNH TỐI ƯU TỐI THIỂU (Khuyến nghị cho học viên)**

Chỉ cần thêm **2 dòng này** để build nhanh gấp 3-4 lần:

```bash
# ⏱️ Bỏ SPDX manifest generation (không cần cho testing)
SPDX_MANIFEST_CREATE = "0"

# ⏱️ Bỏ graphics (opengl, x11) → tự động bỏ clang/mesa/llvm
DISTRO_FEATURES:remove = "opengl x11"
```

**Kết quả:**
- ✅ Tự động loại bỏ: clang-native, llvm-native, mesa (không cần config riêng)
- ✅ Image chạy bình thường với QEMU nographic
- ⚡ **Tiết kiệm 40-70 phút build time**

**Thời gian build:**
- ✅ Lần đầu: **10-20 phút** (thay vì 30-90 phút)
- ✅ Lần tiếp theo: **1-5 phút** (cache đầy đủ)

---

**Giải thích:**
- Mesa (graphics library) require `llvm-native`
- `llvm-native` được cung cấp bởi `clang-native`
- Khi bỏ graphics → tự động bỏ mesa → tự động bỏ clang/llvm
- **Không cần config riêng biệt để bỏ clang** ✅

#### 🔧 **Tối ưu hóa thêm (tùy chọn)**

Ngoài 2 dòng bắt buộc trên, có thể thêm:

```bash
# Bỏ clang hoàn toàn (tiết kiệm 30-60 phút)
TOOLCHAIN_HOST_TASK:remove = "nativesdk-clang"
TOOLCHAIN_TARGET_TASK:remove = "clang"
BBFILES_DYNAMIC:remove = "clang-layer:.*"

# Tăng parallel build (đặt = số CPU cores - ví dụ 4)
BB_NUMBER_THREADS = "4"
PARALLEL_MAKE = "-j 4"
```

### 4.3 Cấu hình Layers

```bash
# Mở file bblayers.conf
nano conf/bblayers.conf
```

Đảm bảo file có nội dung tương tự (update đường dẫn của bạn):

```
BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  /path/to/poky/meta \
  /path/to/poky/meta-poky \
  /path/to/poky/meta-yocto-bsp \
  /path/to/meta-openembedded/meta-oe \
  "
```

**Lưu ý**: Thay `/path/to/` bằng đường dẫn tuyệt đối thực tế của bạn.

## Bước 5: Build Image

### Lựa chọn image để build:

```bash
# Minimal image (nhẹ nhất, chỉ bootable, ~100MB)
bitbake core-image-minimal

# Base image (có toàn bộ tools cơ bản, ~300MB)
bitbake core-image-base

# Sato image (với GUI, ~1.5GB)
bitbake core-image-sato
```

### Chạy build:

```bash
# Nhập lệnh bitbake
bitbake core-image-minimal

# Nếu muốn verbose output
BB_VERBOSE = "1" bitbake core-image-minimal

# Hoặc show mỗi command
bitbake -vv core-image-minimal
```

**Thời gian build**:
- **Không tối ưu**: Lần đầu 30-90 phút, lần tiếp theo 5-15 phút
- **Với tối ưu** (bỏ graphics): Lần đầu 10-20 phút, lần tiếp theo 1-5 phút ⚡

---

## Bước 6: Chạy Image trên QEMU

Sau khi build thành công, sẽ có output trong: `tmp/deploy/images/<machine>/`

### Chạy QEMU

#### **Dùng runqemu** (khuyến nghị)

```bash
cd /home/vietnq/linux_training/yocto/poky
source oe-init-build-env build-qemu

runqemu qemux86-64 core-image-minimal nographic
```

**Lưu ý:** Cần có file `.qemuboot.conf` (có thể không tạo nếu bỏ graphics)

### Login vào QEMU:

```
login: root
password: (nhấn Enter, không cần password)
```

**Thoát QEMU**: Nhấn `Ctrl+A` rồi `X`

---

## Bước 7: Thêm gói vào Image (Ví dụ: valgrind và htop)

Sau khi build xong, có thể muốn thêm packages mới. Dưới đây là hướng dẫn thêm **valgrind** (debug tool) và **htop** (system monitor).

### 7.1 Kiểm tra xem gói có sẵn không

Trước tiên, kiểm tra xem các gói này có trong Yocto recipes:

```bash
cd /home/vietnq/linux_training/yocto/poky
source oe-init-build-env build-qemu

# Cách 1: Dùng bitbake-layers show-recipes
bitbake-layers show-recipes | grep valgrind
bitbake-layers show-recipes | grep htop

# Hoặc Cách 2: Thử build để xem recipe có không
bitbake valgrind -e | head -5   # Nếu có recipe sẽ hiển thị thông tin
bitbake htop -e | head -5
```

**Kết quả mong đợi**: Phải tìm thấy cả 2 packages (thường có sẵn trong meta-oe)

**Nếu không tìm thấy**: Có thể chưa add `meta-oe` layer vào `bblayers.conf`

### 7.2 Cấu hình thêm gói vào local.conf

Mở `conf/local.conf` và tìm dòng `IMAGE_INSTALL:append`:

```bash
nano conf/local.conf
```

Thêm hoặc cập nhật dòng:

```bash
# Thêm valgrind và htop vào image
IMAGE_INSTALL:append = " valgrind htop"
```

**Hoặc nếu đã có IMAGE_INSTALL:**

```bash
# Ví dụ cấu hình hiện tại
IMAGE_INSTALL:append = " vim curl openssh-server openssh-client git python3 perl"

# Thêm valgrind và htop
IMAGE_INSTALL:append = " valgrind htop"
```

### 7.3 Rebuild image

```bash
# Chỉ cần rebuild (không cần clean vì chỉ thêm packages)
bitbake core-image-minimal
```

**Lưu ý**: 
- ✅ Không cần `bitbake -c clean` (sẽ rebuild từ đầu, rất lâu)
- ✅ Chỉ thêm packages → bitbake reuse cache → build nhanh

**Thời gian rebuild**: ~2-5 phút (reuse toàn bộ cache)

### 7.4 Kiểm tra gói trong image mới

Sau khi build xong, chạy QEMU và kiểm tra:

```bash
# Chạy QEMU
runqemu qemux86-64 core-image-minimal nographic

# Trong QEMU, đăng nhập root rồi test:
which valgrind    # → /usr/bin/valgrind
which htop        # → /usr/bin/htop

# Chạy htop để xem system info
htop

# Thoát htop: nhấn q
```

**Kết quả mong đợi**: 
- ✅ Cả 2 command đều tìm thấy
- ✅ htop chạy bình thường và hiển thị process list

---

## Bước 8: Troubleshooting

### 1. Lỗi: "python3: command not found"

```bash
sudo apt-get install python3
```

### 2. Lỗi: "Downloading xxxx failed" (network timeout)

- Kiểm tra kết nối internet
- Hoặc chạy lại build: `bitbake core-image-minimal` (sẽ resume)

### 3. Lỗi: "ERROR: Nothing RPROVIDES 'xxx'"

- Đảm bảo tất cả layers được thêm đúng vào `bblayers.conf`
- Chạy: `bitbake-layers show-layers` để kiểm tra

### 4. Build bị stuck

- Dùng `Ctrl+C` để stop
- Xóa TMPDIR và rebuild: `rm -rf tmp/`
- Hoặc xóa file lock: `rm tmp/bitbake.lock`

### 5. Tối ưu hóa rebuild

```bash
# Xóa tmp nhưng giữ download cache
rm -rf tmp/

# Xóa sstate cache (rebuild from source)
rm -rf sstate-cache/

# Clean một recipe cụ thể
bitbake -c clean core-image-minimal
```

### 6. Lỗi build clang/llvm (build lâu quá)

**Nếu thấy clang-native đang build (lâu 30-60 phút)**:

**Cách 1: Bỏ graphics** (khuyến nghị - tự động bỏ clang)
```bash
# Thêm vào conf/local.conf
DISTRO_FEATURES:remove = "opengl x11"
```

**Cách 2: Bỏ clang trực tiếp** (nhanh, rõ ràng)
```bash
# Thêm vào conf/local.conf
TOOLCHAIN_HOST_TASK:remove = "nativesdk-clang"
TOOLCHAIN_TARGET_TASK:remove = "clang"
BBFILES_DYNAMIC:remove = "clang-layer:.*"
```

Lệnh rebuild:
```bash
bitbake -c clean core-image-minimal
bitbake core-image-minimal
```

**Kết quả**: Clang sẽ **không được build** ✅ (tiết kiệm 30-60 phút)

## Các lệnh hữu ích

```bash
# Kiểm tra layers đã add
bitbake-layers show-layers

# Kiểm tra recipes của một layer
bitbake-layers show-recipes | grep "meta-oe"

# Tìm recipe cụ thể
bitbake-layers find-recipes "*nginx*"

# Xem dependencies của một package
bitbake -g core-image-minimal -u depexp

# Kiểm tra config hiện tại
bitbake-env | grep MACHINE
bitbake-env | grep DISTRO
```

## Cấu trúc thư mục

```
~/yocto-build/
├── poky/                          # Main Yocto repository
│   ├── bitbake/                   # Build engine
│   ├── meta/                      # Core layer
│   ├── meta-poky/                 # Poky distribution layer
│   ├── meta-yocto-bsp/            # Board support packages
│   └── meta-openembedded/         # Extra recipes layer (TRONG poky)
│       ├── meta-oe/               # OpenEmbedded recipes
│       ├── meta-perl/
│       └── ...
└── build-qemu/                    # Build directory
    ├── conf/
    │   ├── local.conf             # Build configuration
    │   └── bblayers.conf          # Enabled layers
    ├── tmp/                       # Temporary build files
    └── downloads/                 # Downloaded sources (cache)
```

**Lưu ý:** Tất cả meta layers (meta, meta-poky, meta-yocto-bsp, meta-openembedded) nằm **TRONG poky**, không phải cạnh poky ✅

## Bước 9: Cấu hình Image tùy chỉnh (Option thêm)

Nếu muốn tuỳ chỉnh image, trước khi build hoặc rebuild, bạn có thể cấu hình các tùy chọn sau trong `conf/local.conf`:

### 8.1 Yêu cầu mật khẩu cho root

```bash
INHERIT += "extrausers"
EXTRA_USERS_PARAMS = "usermod -p $(openssl passwd -1 root123) root; "
```

### 8.2 Thêm user mới

```bash
INHERIT += "extrausers"
EXTRA_USERS_PARAMS = "useradd -P mypassword myuser; usermod -a -G wheel myuser; "
```

### 8.3 Thay đổi hostname

```bash
hostname_pn-base-files = "yocto-machine"
```

### 8.4 Thêm packages vào image

```bash
IMAGE_INSTALL:append = " vim curl openssh-server openssh-client"
IMAGE_INSTALL:append = " git python3 perl htop"
```

**Packages phổ biến**: vim, nano, curl, wget, openssh-server, gdb, git, python3, perl

### 8.5 Bật SSH server

```bash
IMAGE_INSTALL:append = " openssh-server"
EXTRA_IMAGE_FEATURES:append = " ssh-server-openssh"
```

### 8.6 Cấu hình locale và timezone

```bash
DEFAULT_TIMEZONE = "Asia/Ho_Chi_Minh"
IMAGE_LINGUAS = "en-us"
```

### 8.7 Tùy chỉnh kích thước image

```bash
IMAGE_ROOTFS_EXTRA_SPACE = "102400"  # +100MB
```

### 8.8 Ví dụ cấu hình đầy đủ

```bash
# ⏱️ TỐI ƯU HÓA BUILD TIME
SPDX_MANIFEST_CREATE = "0"
DISTRO_FEATURES:remove = "opengl x11"

# Bỏ clang hoàn toàn (tiết kiệm 30-60 phút)
TOOLCHAIN_HOST_TASK:remove = "nativesdk-clang"
TOOLCHAIN_TARGET_TASK:remove = "clang"
BBFILES_DYNAMIC:remove = "clang-layer:.*"

BB_NUMBER_THREADS = "4"
PARALLEL_MAKE = "-j 4"

# IMAGE CONFIGURATION
IMAGE_INSTALL:append = " vim curl openssh-server openssh-client git python3 perl htop"
EXTRA_IMAGE_FEATURES:append = " ssh-server-openssh"
hostname_pn-base-files = "yocto-embedded"
DEFAULT_TIMEZONE = "Asia/Ho_Chi_Minh"
IMAGE_LINGUAS = "en-us"
IMAGE_ROOTFS_EXTRA_SPACE = "102400"
```

**Rebuild sau khi cấu hình:**

```bash
bitbake -c clean core-image-minimal
bitbake core-image-minimal
```

---

## Các commit được sử dụng

| Repository | Commit | Mô tả |
|-----------|--------|-------|
| poky | `5049bc18a8` | Post release whinlatter |
| meta-openembedded | `2de5071f9a` | klibc: Use libgcc for compiler runtime |

## Tài liệu tham khảo

- [Yocto Project Documentation](https://docs.yoctoproject.org/)
- [Poky Repository](https://git.yoctoproject.org/poky)
- [Meta-OpenEmbedded](https://github.com/openembedded/meta-openembedded)
