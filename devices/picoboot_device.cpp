#include "picoboot_device.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QtEndian>
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

#include "ardwiinolookup.h"
#include "boot/uf2.h"
#include "devices/serialdevice.h"
#include "picoboot_connection_cxx.h"
#define CONFIG_SECTION FLASH_START + (256 * 1024)
#define RF_SECTION FLASH_START + (512 * 1024)
PicobootDevice::PicobootDevice(UsbDevice_t devt, QObject *parent) : Device(devt, parent) {
    m_board = ArdwiinoLookup::empty;
    setBoardType("pico");
}
QString PicobootDevice::getDescription() {
    return "Raspberry PI Pico (PicoBoot)";
}
bool PicobootDevice::isReady() {
    return true;
}
void PicobootDevice::close() {
}
bool PicobootDevice::open() {
    return true;
}
void PicobootDevice::bootloader() {
}

// Most of the stuff below is copied from main.cpp from picoboot.
// from -> to
struct range {
    range() : from(0), to(0) {}
    range(uint32_t from, uint32_t to) : from(from), to(to) {}
    uint32_t from;
    uint32_t to;

    bool empty() const {
        return from >= to;
    }
    bool contains(uint32_t addr) const { return addr >= from && addr < to; }
    uint32_t clamp(uint32_t addr) const {
        if (addr < from) addr = from;
        if (addr > to) addr = to;
        return addr;
    }

    void intersect(const range &other) {
        from = other.clamp(from);
        to = other.clamp(to);
    }
};

// ranges should not overlap
template <typename T>
struct range_map {
    struct mapping {
        mapping(uint32_t offset, uint32_t max_offset) : offset(offset), max_offset(max_offset) {}
        const uint32_t offset;
        const uint32_t max_offset;
    };

    void check_overlap(uint32_t p) {
        auto f = m.lower_bound(p);
        if (f != m.end()) {
            assert(p >= f->first);
            assert(p < f->second.first);
        }
    }

    void insert(const range &r, T t) {
        if (r.to != r.from) {
            assert(r.to > r.from);
            check_overlap(r.from);
            check_overlap(r.to);
            m.insert(std::make_pair(r.from, std::make_pair(r.to, t)));
        }
    }

    struct not_mapped_exception : std::exception {
        const char *what() const noexcept override {
            return "Hmm uncaught not mapped";
        }
    };
    std::pair<mapping, T> get(uint32_t p) {
        auto f = m.upper_bound(p);
        if (f == m.end()) {
            if (m.empty())
                throw not_mapped_exception();
        } else if (f == m.begin()) {
            throw not_mapped_exception();
        }
        f--;
        assert(p >= f->first);
        if (p > f->second.first) {
            throw not_mapped_exception();
        }
        return std::make_pair(mapping(p - f->first, f->second.first - f->first), f->second.second);
    }

    uint32_t next(uint32_t p) {
        auto f = m.upper_bound(p);
        if (f == m.end()) {
            std::numeric_limits<uint32_t>::max();
        }
        return f->first;
    }

    std::vector<range> ranges() {
        std::vector<range> r;
        r.reserve(m.size());
        for (const auto &e : m) {
            r.emplace_back(range(e.first, e.second.first));
        }
        return r;
    }

    size_t size() const { return m.size(); }

   private:
    std::map<uint32_t, std::pair<uint32_t, T>> m;
};

struct command_failure : std::exception {
    command_failure(int code, std::string s) : c(code), s(std::move(s)) {}

    const char *what() const noexcept override {
        return s.c_str();
    }

    int code() const { return c; }

   private:
    int c;
    std::string s;
};

struct memory_access {
    virtual void read(uint32_t, uint8_t *buffer, uint size) = 0;

    virtual bool is_device() { return false; }

    virtual uint32_t get_binary_start() = 0;

    uint32_t read_int(uint32_t addr) {
        assert(!(addr & 3u));
        uint32_t rc;
        read(addr, (uint8_t *)&rc, 4);
        return rc;
    }

    uint32_t read_short(uint32_t addr) {
        assert(!(addr & 1u));
        uint16_t rc;
        read(addr, (uint8_t *)&rc, 2);
        return rc;
    }

    // read a vector of types that have a raw_type_mapping
    template <typename T>
    void read_raw(uint32_t addr, T &v) {
        typename raw_type_mapping<T>::access_type &check = v;  // ugly check that we aren't trying to read into something we shouldn't
        read(addr, (uint8_t *)&v, sizeof(typename raw_type_mapping<T>::access_type));
    }

    // read a vector of types that have a raw_type_mapping
    template <typename T>
    std::vector<T> read_vector(uint32_t addr, uint count) {
        assert(count);
        std::vector<typename raw_type_mapping<T>::access_type> buffer(count);
        read(addr, (uint8_t *)buffer.data(), count * sizeof(typename raw_type_mapping<T>::access_type));
        std::vector<T> v;
        v.reserve(count);
        for (const auto &e : buffer) {
            v.push_back(e);
        }
        return v;
    }

    template <typename T>
    void read_into_vector(uint32_t addr, uint count, std::vector<T> &v) {
        std::vector<typename raw_type_mapping<T>::access_type> buffer(count);
        if (count) read(addr, (uint8_t *)buffer.data(), count * sizeof(typename raw_type_mapping<T>::access_type));
        v.clear();
        v.reserve(count);
        for (const auto &e : buffer) {
            v.push_back(e);
        }
    }
};
struct file_memory_access : public memory_access {
    file_memory_access(QFile *file, range_map<size_t> &rmap, uint32_t binary_start) : file(file), rmap(rmap), binary_start(binary_start) {
    }

    uint32_t get_binary_start() override {
        return binary_start;
    }

    void read(uint32_t address, uint8_t *buffer, uint32_t size) override {
        while (size) {
            auto result = rmap.get(address);
            uint this_size = std::min(size, result.first.max_offset - result.first.offset);
            assert(this_size);
            file->seek(result.second + result.first.offset);
            file->read((char *)buffer, this_size);
            buffer += this_size;
            address += this_size;
            size -= this_size;
        }
    }

    const range_map<size_t> &get_rmap() {
        return rmap;
    }

    ~file_memory_access() {
        file->close();
    }

   private:
    QFile *file;
    range_map<size_t> rmap;
    uint32_t binary_start;
};

static char error_msg[512];
static void __noreturn fail(int code, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(error_msg, sizeof(error_msg), format, args);
    va_end(args);
    fail(code, std::string(error_msg).c_str());
}
static void __noreturn fail(int code, std::string msg) {
    throw command_failure(code, msg);
}

uint32_t bootrom_func_lookup(memory_access &access, uint16_t tag) {
    auto magic = access.read_int(BOOTROM_MAGIC_ADDR);
    magic &= 0xffffff;  // ignore bootrom version
    if (magic != BOOTROM_MAGIC) {
        if (!((magic ^ BOOTROM_MAGIC) & 0xffff))
            fail(ERROR_INCOMPATIBLE, "Incorrect RP2040 BOOT ROM version");
        else
            fail(ERROR_INCOMPATIBLE, "RP2040 BOOT ROM not found");
    }

    // dereference the table pointer
    uint32_t table_entry = access.read_short(BOOTROM_MAGIC_ADDR + 4);
    uint16_t entry_tag;
    do {
        entry_tag = access.read_short(table_entry);
        if (entry_tag == tag) {
            // 16 bit symbol is next
            return access.read_short(table_entry + 2);
        }
        table_entry += 4;
    } while (entry_tag);
    fail(ERROR_INCOMPATIBLE, "Reboot function not found in BOOT ROM");
}
static std::string hex_string(int value, int width = 8, bool prefix = true) {
    std::stringstream ss;
    if (prefix) ss << "0x";
    ss << std::setfill('0') << std::setw(width) << std::hex << value;
    return ss.str();
}
static inline uint32_t rom_table_code(char c1, char c2) {
    return (c2 << 8u) | c1;
}
struct picoboot_memory_access : public memory_access {
    explicit picoboot_memory_access(picoboot::connection &connection) : connection(connection) {}

    bool is_device() override {
        return true;
    }

    uint32_t get_binary_start() override {
        return FLASH_START;
    }

    void read(uint32_t address, uint8_t *buffer, uint size) override {
        if (flash == get_memory_type(address)) {
            connection.exit_xip();
        }
        if (rom == get_memory_type(address) && (address + size) >= 0x2000) {
            // read by memcpy instead
            uint program_base = SRAM_START + 0x4000;
            // program is "return memcpy(SRAM_BASE, 0, 0x4000);"
            std::vector<uint32_t> program = {
                0x07482101,  // movs r1, #1;       lsls r0, r1, #29
                0x2100038a,  // lsls r2, r1, #14;  movs r1, #0
                0x47184b00,  // ldr  r3, [pc, #0]; bx r3
                bootrom_func_lookup(*this, rom_table_code('M', 'C'))};
            write_vector(program_base, program);
            connection.exec(program_base);
            // 4k is copied into the start of RAM
            connection.read(SRAM_START + address, (uint8_t *)buffer, size);
        } else if (is_transfer_aligned(address) && is_transfer_aligned(address + size)) {
            connection.read(address, (uint8_t *)buffer, size);
        } else {
            if (flash == get_memory_type(address)) {
                uint32_t aligned_start = address & ~(PAGE_SIZE - 1);
                uint32_t aligned_end = (address + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
                std::vector<uint8_t> tmp_buffer(aligned_end - aligned_start);
                connection.read(aligned_start, tmp_buffer.data(), aligned_end - aligned_start);
                std::copy(tmp_buffer.cbegin() + (address - aligned_start), tmp_buffer.cbegin() + (address + size - aligned_start), buffer);
            } else {
                std::stringstream sstream;
                sstream << "Address range " << hex_string(address) << " + " << hex_string(size);
                throw std::invalid_argument(sstream.str());
            }
        }
    }

    // note this does not automatically erase flash
    void write(uint32_t address, uint8_t *buffer, uint size) {
        if (flash == get_memory_type(address)) {
            connection.exit_xip();
        }
        if (is_transfer_aligned(address) && is_transfer_aligned(address + size)) {
            connection.write(address, (uint8_t *)buffer, size);
        } else {
            // for write, we must be correctly sized/aligned in 256 byte chunks
            std::stringstream sstream;
            sstream << "Address range " << hex_string(address) << " + " << hex_string(size);
            throw std::invalid_argument(sstream.str());
        }
    }

    template <typename T>
    void write_vector(uint32_t addr, std::vector<T> v) {
        assert(!v.empty());
        write(addr, (uint8_t *)v.data(), v.size() * sizeof(typename raw_type_mapping<T>::access_type));
    }

   private:
    picoboot::connection &connection;
};
void build_rmap_uf2(QFile *file, range_map<size_t> &rmap) {
    file->seek(0);
    uf2_block block;
    uint pos = 0;
    do {
        if (1 > file->read((char *)&block, sizeof(uf2_block))) {
            if (file->atEnd()) break;
            fail(ERROR_READ_FAILED, "unexpected end of input file");
        }
        if (block.magic_start0 == UF2_MAGIC_START0 && block.magic_start1 == UF2_MAGIC_START1 &&
            block.magic_end == UF2_MAGIC_END) {
            if (block.flags & UF2_FLAG_FAMILY_ID_PRESENT && block.file_size == RP2040_FAMILY_ID &&
                !(block.flags & UF2_FLAG_NOT_MAIN_FLASH) && block.payload_size == PAGE_SIZE) {
                rmap.insert(range(block.target_addr, block.target_addr + PAGE_SIZE), pos + offsetof(uf2_block, data[0]));
            }
        }
        pos += sizeof(uf2_block);
    } while (true);
}

uint32_t find_binary_start(range_map<size_t> &rmap) {
    range flash(FLASH_START, FLASH_END);
    range sram(SRAM_START, SRAM_END);
    range xip_sram(XIP_SRAM_BASE, XIP_SRAM_END);
    uint32_t binary_start = std::numeric_limits<uint32_t>::max();
    for (const auto &r : rmap.ranges()) {
        if (r.contains(FLASH_START)) {
            return FLASH_START;
        }
        if (sram.contains(r.from) || xip_sram.contains((r.from))) {
            if (r.from < binary_start || (xip_sram.contains(binary_start) && sram.contains(r.from))) {
                binary_start = r.from;
            }
        }
    }
    if (get_memory_type(binary_start) == invalid) {
        return 0;
    }
    return binary_start;
}

auto memory_names = std::map<enum memory_type, std::string>{
    {memory_type::sram, "RAM"},
    {memory_type::sram_unstriped, "Unstriped RAM"},
    {memory_type::flash, "Flash"},
    {memory_type::xip_sram, "XIP RAM"},
    {memory_type::rom, "ROM"}};

std::vector<range> get_colaesced_ranges(file_memory_access &file_access) {
    auto rmap = file_access.get_rmap();
    auto ranges = rmap.ranges();
    std::sort(ranges.begin(), ranges.end(), [](const range &a, const range &b) {
        return a.from < b.from;
    });
    // coalesce all the contiguous ranges
    for (auto i = ranges.begin(); i < ranges.end();) {
        if (i != ranges.end() - 1) {
            if (i->to == (i + 1)->from) {
                i->to = (i + 1)->to;
                i = ranges.erase(i + 1) - 1;
                continue;
            }
        }
        i++;
    }
    return ranges;
}

void PicobootDevice::program(QFile *file, std::function<void(long, long, int, int)> progress) {
    file->open(QIODevice::ReadOnly);
    range_map<size_t> r;
    build_rmap_uf2(file, r);
    uint32_t binary_start;
    binary_start = find_binary_start(r);
    file_memory_access file_access(file, r, binary_start);
    auto ranges = get_colaesced_ranges(file_access);
    for (auto mem_range : ranges) {
        enum memory_type t1 = get_memory_type(mem_range.from);
        enum memory_type t2 = get_memory_type(mem_range.to);
        if (t1 != t2 || t1 == invalid || t1 == rom) {
            fail(ERROR_FORMAT, "File to load contained an invalid memory range 0x%08x-0x%08x", mem_range.from,
                 mem_range.to);
        }
    }
    libusb_device_handle *handle;
    libusb_open(m_deviceID.dev, &handle);
    picoboot::connection con(handle, true);
    picoboot_memory_access raw_access(con);
    // Write and verify
    int totalCount = ranges.size() * 2;
    int currentSection = 0;
    // Write / Verify
    for (auto mem_range : ranges) {
        if (mem_range.from >= CONFIG_SECTION) {
            // Don't flash the config section so that it persists across updates
            // TODO: RF
            progress(mem_range.to, mem_range.to - mem_range.from, currentSection++, totalCount);
            currentSection++;
            continue;
        }
        enum memory_type type = get_memory_type(mem_range.from);
        // Write
        // TODO: do we care enough to put this stuff on the console?
        // progress_bar bar("Loading into " + memory_names[type] + ": ");
        uint32_t batch_size = FLASH_SECTOR_ERASE_SIZE;
        bool ok = true;
        std::vector<uint8_t> file_buf;
        std::vector<uint8_t> device_buf;
        for (uint32_t base = mem_range.from; base < mem_range.to && ok;) {
            uint32_t this_batch = std::min(mem_range.to - base, batch_size);
            if (type == flash) {
                // we have to erase an entire page, so then fill with zeros
                range aligned_range(base & ~(FLASH_SECTOR_ERASE_SIZE - 1), (base & ~(FLASH_SECTOR_ERASE_SIZE - 1)) + FLASH_SECTOR_ERASE_SIZE);
                range read_range(base, base + this_batch);
                read_range.intersect(aligned_range);
                file_access.read_into_vector(read_range.from, read_range.to - read_range.from, file_buf);
                // zero padding up to FLASH_SECTOR_ERASE_SIZE
                file_buf.insert(file_buf.begin(), read_range.from - aligned_range.from, 0);
                file_buf.insert(file_buf.end(), aligned_range.to - read_range.to, 0);
                assert(file_buf.size() == FLASH_SECTOR_ERASE_SIZE);
                con.exit_xip();
                con.flash_erase(aligned_range.from, FLASH_SECTOR_ERASE_SIZE);
                raw_access.write_vector(aligned_range.from, file_buf);
                base = read_range.to;  // about to add batch_size
            } else {
                file_access.read_into_vector(base, this_batch, file_buf);
                raw_access.write_vector(base, file_buf);
                base += this_batch;
            }
            progress(base - mem_range.from, mem_range.to - mem_range.from, currentSection, totalCount);
        }
        currentSection++;
        // Verify
        ok = true;
        // progress_bar bar("Verifying " + memory_names[type] + ":    ");
        file_buf.clear();
        device_buf.clear();
        uint32_t pos = mem_range.from;
        for (uint32_t base = mem_range.from; base < mem_range.to && ok; base += batch_size) {
            uint32_t this_batch = std::min(mem_range.to - base, batch_size);
            file_access.read_into_vector(base, this_batch, file_buf);
            raw_access.read_into_vector(base, this_batch, device_buf);
            assert(file_buf.size() == device_buf.size());
            for (uint i = 0; i < this_batch; i++) {
                if (file_buf[i] != device_buf[i]) {
                    pos = base + i;
                    ok = false;
                    break;
                }
            }
            if (ok) {
                pos = base + this_batch;
            }
            progress(pos - mem_range.from, mem_range.to - mem_range.from, currentSection, totalCount);
        }
        if (ok) {
            std::cout << "  OK\n";
        } else {
            std::cout << "  FAILED\n";
            fail(ERROR_VERIFICATION_FAILED, "The device contents did not match the file");
        }
        currentSection++;
    }
    // Execute
    uint32_t start = file_access.get_binary_start();
    if (!start) {
        fail(ERROR_FORMAT, "Cannot execute as file does not contain a valid RP2 executable image");
    }
    con.reboot(flash == get_memory_type(start) ? 0 : start, SRAM_END, 500);
}