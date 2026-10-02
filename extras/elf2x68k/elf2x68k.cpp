// elf2x68k.cpp
// ======================================================================
// Original by yunkya2
// Converted from Python by DeepSeek.
// ======================================================================
//
// Convert an m68k (Motorola 68000) ELF32 executable (big endian) into an
// X68000 executable file (.x).
//
// This is a complete C++17 port of the Python reference tool elf2x68k.py.
// The structure, processing order and produced output bytes are identical
// to the original.
//
// Build (POSIX; iconv from libc is used for CP932 symbol names):
//     g++ -std=c++17 -O2 -Wall -Wextra -o elf2x68k elf2x68k.cpp
//
// Notes:
//   * Symbol names written into the X68k symbol table are encoded as CP932
//     (Shift-JIS), matching Python's "cp932" codec. ASCII names - by far
//     the common case - are identical in both encodings.
//   * The Python original validated its input with assert/struct.error;
//     this port throws std::runtime_error for the same conditions. main()
//     catches those exceptions, prints an error and exits with status 1.
//

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if !defined(_WIN32)
#include <cerrno>
#include <iconv.h>
#endif

namespace {

// ---------------------------------------------------------------------------
// Low level helpers
// ---------------------------------------------------------------------------

// Read exactly n bytes from `is` or throw.  (In Python the equivalent
// combination of fh.read() and struct.unpack()/indexing would fail for the
// same input.)
void must_read(std::istream& is, void* dst, size_t n) {
    char* p = static_cast<char*>(dst);
    while (n > 0) {
        is.read(p, static_cast<std::streamsize>(n));
        std::streamsize got = is.gcount();
        if (got <= 0)
            throw std::runtime_error("unexpected end of file while reading ELF structure");
        p += got;
        n -= static_cast<size_t>(got);
    }
}

uint16_t read_u16be(std::istream& is) {
    uint8_t b[2];
    must_read(is, b, sizeof(b));
    return static_cast<uint16_t>((static_cast<uint16_t>(b[0]) << 8) | b[1]);
}

uint32_t read_u32be(std::istream& is) {
    uint8_t b[4];
    must_read(is, b, sizeof(b));
    return (static_cast<uint32_t>(b[0]) << 24) | (static_cast<uint32_t>(b[1]) << 16) |
           (static_cast<uint32_t>(b[2]) << 8) | static_cast<uint32_t>(b[3]);
}

int32_t read_i32be(std::istream& is) {
    return static_cast<int32_t>(read_u32be(is));
}

uint32_t buf_u32be(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

void put_u16be(std::vector<uint8_t>& v, uint16_t x) {
    v.push_back(static_cast<uint8_t>(x >> 8));
    v.push_back(static_cast<uint8_t>(x & 0xff));
}

void put_u32be(std::vector<uint8_t>& v, uint32_t x) {
    v.push_back(static_cast<uint8_t>(x >> 24));
    v.push_back(static_cast<uint8_t>((x >> 16) & 0xff));
    v.push_back(static_cast<uint8_t>((x >> 8) & 0xff));
    v.push_back(static_cast<uint8_t>(x & 0xff));
}

void append_zeros(std::vector<uint8_t>& v, size_t n) {
    if (n > v.max_size() - v.size())
        throw std::runtime_error("section contents too large");
    v.resize(v.size() + n);  // value-initialized -> zero bytes
}

// Read a whole section payload that starts at `offset` in the file.
std::vector<uint8_t> read_section_data(std::istream& is, uint32_t offset, uint32_t size) {
    is.seekg(offset);
    std::vector<uint8_t> data(size);
    if (size != 0)
        must_read(is, data.data(), size);
    return data;
}

// NUL-terminated string starting at `idx` inside a string table.
std::string get_cstr(const std::vector<uint8_t>& table, uint32_t idx) {
    if (idx >= table.size())
        throw std::runtime_error("string index out of range in string table");
    std::string s;
    for (size_t i = idx; i < table.size() && table[i] != 0; ++i)
        s += static_cast<char>(table[i]);
    return s;
}

// Check that a value fits in an unsigned 32-bit field.  Python's
// struct.pack(">L", ...) raises struct.error for anything else.
uint32_t u32_checked(int64_t v, const char* what) {
    if (v < 0 || v > 0xffffffffLL)
        throw std::runtime_error(std::string(what) + " does not fit in a 32-bit value");
    return static_cast<uint32_t>(v);
}

// ---------------------------------------------------------------------------
// CP932 (Shift-JIS) encoding of symbol names.
//
// The Python original decodes symbol names from the ELF string table as
// UTF-8 and re-encodes them with the "cp932" codec for the X68k symbol
// table.  We do the same here with iconv where available; names consisting
// of pure ASCII are identical in every case.
// ---------------------------------------------------------------------------

#if defined(_WIN32)
std::vector<uint8_t> encode_cp932(const std::string& name) {
    // Windows CRT has no libc iconv; ASCII names (the norm) pass through
    // unchanged, which is exactly what CP932 would produce for them.
    return std::vector<uint8_t>(name.begin(), name.end());
}
#else
std::vector<uint8_t> encode_cp932(const std::string& name) {
    iconv_t cd = iconv_open("CP932", "UTF-8");
    if (cd == (iconv_t)(-1))
        return std::vector<uint8_t>(name.begin(), name.end());

    std::vector<uint8_t> out;
    std::vector<char> input(name.begin(), name.end());
    char* inptr = input.empty() ? nullptr : input.data();
    size_t inleft = input.size();
    char buf[256];
    bool ok = false;

    for (;;) {
        char* outptr = buf;
        size_t outleft = sizeof(buf);
        size_t r = iconv(cd, inleft ? &inptr : nullptr, inleft ? &inleft : nullptr,
                         &outptr, &outleft);
        for (size_t k = 0; k < sizeof(buf) - outleft; ++k)
            out.push_back(static_cast<uint8_t>(buf[k]));
        if (r != static_cast<size_t>(-1)) {
            if (inleft == 0) { ok = true; break; }
            continue;
        }
        if (errno == E2BIG) continue;   // output buffer was too small, retry
        break;                          // EILSEQ/EINVAL: cannot encode
    }
    iconv_close(cd);

    if (!ok)
        throw std::runtime_error("cannot encode symbol name \"" + name + "\" as CP932");
    return out;
}
#endif

}  // anonymous namespace

// ===========================================================================
// ELF structures (big endian, ELF32)
// ===========================================================================

class ElfHeader {
public:
    // e_ident fields
    uint8_t ei_cls        = 0;
    uint8_t ei_data       = 0;
    uint8_t ei_version    = 0;
    uint8_t ei_osabi      = 0;
    uint8_t ei_abiversion = 0;
    // ">2H5L6H" fields
    uint16_t type      = 0;
    uint16_t machine   = 0;
    uint32_t version   = 0;
    uint32_t entry     = 0;
    uint32_t phoff     = 0;
    uint32_t shoff     = 0;
    uint32_t flags     = 0;
    uint16_t ehsize    = 0;
    uint16_t phentsize = 0;
    uint16_t phnum     = 0;
    uint16_t shentsize = 0;
    uint16_t shnum     = 0;
    uint16_t shstrndx  = 0;

    explicit ElfHeader(std::istream& fh) {
        fh.seekg(0);
        uint8_t elfident[16];
        must_read(fh, elfident, sizeof(elfident));
        if (memcmp(elfident, "\x7f" "ELF", 4) != 0)
            throw std::runtime_error("not an ELF file (bad magic number)");

        ei_cls        = elfident[4];
        ei_data       = elfident[5];
        ei_version    = elfident[6];
        ei_osabi      = elfident[7];
        ei_abiversion = elfident[8];

        if (ei_cls != 1)  // ELF32
            throw std::runtime_error("not an ELF32 file (EI_CLASS != 1)");
        if (ei_data != 2) // Big endian
            throw std::runtime_error("only big endian ELF files are supported (EI_DATA != 2)");

        // correspond to unpack(">2H5L6H", ...)
        type      = read_u16be(fh);
        machine   = read_u16be(fh);
        version   = read_u32be(fh);
        entry     = read_u32be(fh);
        phoff     = read_u32be(fh);
        shoff     = read_u32be(fh);
        flags     = read_u32be(fh);
        ehsize    = read_u16be(fh);
        phentsize = read_u16be(fh);
        phnum     = read_u16be(fh);
        shentsize = read_u16be(fh);
        shnum     = read_u16be(fh);
        shstrndx  = read_u16be(fh);

        if (machine != 4) // Machine 68000
            throw std::runtime_error("unsupported machine type (expected EM_68K = 4)");
    }
};

class ProgramHeader {
public:
    uint32_t type = 0;
    uint32_t offset = 0;
    uint32_t vaddr = 0;
    uint32_t paddr = 0;
    uint32_t filesz = 0;
    uint32_t memsz = 0;
    uint32_t flags = 0;
    uint32_t align = 0;

    explicit ProgramHeader(const ElfHeader& eh, std::istream& fh) {
        if (eh.phentsize < 32)
            throw std::runtime_error("program header entry is too small");
        std::vector<uint8_t> data(eh.phentsize);
        must_read(fh, data.data(), eh.phentsize);
        // correspond to unpack(">8L", data)
        type   = buf_u32be(&data[0]);
        offset = buf_u32be(&data[4]);
        vaddr  = buf_u32be(&data[8]);
        paddr  = buf_u32be(&data[12]);
        filesz = buf_u32be(&data[16]);
        memsz  = buf_u32be(&data[20]);
        flags  = buf_u32be(&data[24]);
        align  = buf_u32be(&data[28]);
    }

    // matches ProgramHeader.__repr__ of the Python original
    std::string repr() const {
        char buf[96];
        std::snprintf(buf, sizeof(buf),
                      "0x%02lx 0x%06lx 0x%08lx 0x%08lx 0x%06lx 0x%06lx 0x%02lx 0x%02lx",
                      (unsigned long)type, (unsigned long)offset, (unsigned long)vaddr,
                      (unsigned long)paddr, (unsigned long)filesz, (unsigned long)memsz,
                      (unsigned long)flags, (unsigned long)align);
        return buf;
    }
};

class SectionHeader {
public:
    // ">10L" fields
    uint32_t nameidx   = 0;
    uint32_t type      = 0;
    uint32_t flags     = 0;
    uint32_t addr      = 0;
    uint32_t offset    = 0;
    uint32_t size      = 0;
    uint32_t link      = 0;
    uint32_t info      = 0;
    uint32_t addralign = 0;
    uint32_t entsize   = 0;
    // derived fields
    std::string name;
    int relidx = -1;   // unused placeholder kept from the Python original
    int segtype = -1;  // 0 = .text, 1 = .data, 2 = .bss; -1 = not loaded

    explicit SectionHeader(const ElfHeader& eh, std::istream& fh) {
        if (eh.shentsize < 40)
            throw std::runtime_error("section header entry is too small");
        std::vector<uint8_t> data(eh.shentsize);
        must_read(fh, data.data(), eh.shentsize);
        nameidx   = buf_u32be(&data[0]);
        type      = buf_u32be(&data[4]);
        flags     = buf_u32be(&data[8]);
        addr      = buf_u32be(&data[12]);
        offset    = buf_u32be(&data[16]);
        size      = buf_u32be(&data[20]);
        link      = buf_u32be(&data[24]);
        info      = buf_u32be(&data[28]);
        addralign = buf_u32be(&data[32]);
        entsize   = buf_u32be(&data[36]);
    }

    // matches SectionHeader.__repr__ of the Python original
    std::string repr() const {
        char buf[128];
        std::snprintf(buf, sizeof(buf),
                      "0x%02lx 0x%02lx 0x%08lx 0x%06lx 0x%06lx %02lu 0x%02lx 0x%02lx 0x%02lx",
                      (unsigned long)type, (unsigned long)flags, (unsigned long)addr,
                      (unsigned long)offset, (unsigned long)size, (unsigned long)link,
                      (unsigned long)info, (unsigned long)addralign, (unsigned long)entsize);
        return buf;
    }
};

class Rela {
public:
    uint32_t offset = 0;
    uint32_t info   = 0;
    int32_t addend  = 0;
    uint32_t sym    = 0;
    uint32_t type   = 0;

    explicit Rela(const ElfHeader& /*eh*/, std::istream& fh) {
        // correspond to unpack(">2Ll", data)
        offset = read_u32be(fh);
        info   = read_u32be(fh);
        addend = read_i32be(fh);
        sym    = info >> 8;
        type   = info & 0xff;
    }

    // matches Rela.__repr__ of the Python original
    std::string repr() const {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "0x%08lx 0x%08lx 0x%08lx",
                      (unsigned long)offset, (unsigned long)info,
                      (unsigned long)(uint32_t)addend);
        return buf;
    }
};

class Symbol {
public:
    // ">3L2bH" fields
    uint32_t nameidx = 0;
    uint32_t value   = 0;
    uint32_t size    = 0;
    int8_t   info    = 0;
    int8_t   other   = 0;
    uint16_t shndx   = 0;
    // derived fields
    uint8_t bind = 0;
    uint8_t type = 0;
    std::string name;

    explicit Symbol(const ElfHeader& /*eh*/, std::istream& fh) {
        nameidx = read_u32be(fh);
        value   = read_u32be(fh);
        size    = read_u32be(fh);
        uint8_t b[2];
        must_read(fh, b, sizeof(b));
        info  = static_cast<int8_t>(b[0]);
        other = static_cast<int8_t>(b[1]);
        shndx = read_u16be(fh);
        bind  = static_cast<uint8_t>((static_cast<int>(info) >> 4) & 0xf);
        type  = static_cast<uint8_t>(info & 0xf);
    }

    // matches Symbol.__repr__ of the Python original
    std::string repr() const {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "0x%08lx 0x%08lx 0x%02x 0x%02x 0x%04x %s",
                      (unsigned long)value, (unsigned long)size,
                      (unsigned)(uint8_t)info, (unsigned)(uint8_t)other,
                      (unsigned)shndx, name.c_str());
        return buf;
    }
};

// ===========================================================================
// X68000 executable structures
// ===========================================================================

class X68kHeader {
public:
    uint32_t base;
    uint32_t entry;
    uint32_t textsz;
    uint32_t datasz;
    uint32_t bsssz;
    uint32_t relsz;
    uint32_t symsz;

    X68kHeader(uint32_t base_, uint32_t entry_, uint32_t textsz_, uint32_t datasz_,
               uint32_t bsssz_, uint32_t relsz_, uint32_t symsz_)
        : base(base_), entry(entry_), textsz(textsz_), datasz(datasz_),
          bsssz(bsssz_), relsz(relsz_), symsz(symsz_) {}

    // 'HU\0\0' + 7 big endian longs + 32 zero bytes, as in the Python original
    std::vector<uint8_t> encode() const {
        std::vector<uint8_t> out;
        out.push_back('H');
        out.push_back('U');
        out.push_back(0);
        out.push_back(0);
        put_u32be(out, base);
        put_u32be(out, entry);
        put_u32be(out, textsz);
        put_u32be(out, datasz);
        put_u32be(out, bsssz);
        put_u32be(out, relsz);
        put_u32be(out, symsz);
        out.insert(out.end(), 32, 0);
        return out;
    }
};

class X68kSymbol {
public:
    uint16_t type;
    uint32_t value;
    std::string name;

    X68kSymbol(uint16_t type_, uint32_t value_, const std::string& name_)
        : type(type_), value(value_), name(name_) {}

    std::vector<uint8_t> encode(uint32_t base = 0) const {
        std::vector<uint8_t> out;
        put_u16be(out, type);
        put_u32be(out, value + base);
        std::vector<uint8_t> enname = encode_cp932(name);
        out.insert(out.end(), enname.begin(), enname.end());
        // Pad the name with (2 - (len & 1)) zero bytes, exactly like the
        // Python original, so that every symbol entry stays word aligned.
        out.insert(out.end(), (enname.size() & 1) ? 1 : 2, 0);
        return out;
    }
};

// ===========================================================================
// Convert ELF to X68k execute file (fixed load address)
// ===========================================================================

std::vector<uint8_t> elf2x68k(std::istream& fh, uint32_t xbase = 0, bool strip = false,
                              const std::set<std::string>& force_reloc_syms = {}) {
    // Read ELF header
    ElfHeader eh(fh);

    // Read program headers
    fh.seekg(eh.phoff);
    std::vector<ProgramHeader> phlist;
    phlist.reserve(eh.phnum);
    for (uint16_t i = 0; i < eh.phnum; ++i)
        phlist.emplace_back(eh, fh);

    // Read section headers
    std::vector<SectionHeader> shlist;
    shlist.reserve(eh.shnum);
    SectionHeader* sh_symtab = nullptr;
    std::vector<size_t> sh_rela;
    fh.seekg(eh.shoff);
    for (uint16_t i = 0; i < eh.shnum; ++i) {
        shlist.emplace_back(eh, fh);
        SectionHeader& sh = shlist.back();
        if (sh.type == 2)             // SHT_SYMTAB
            sh_symtab = &sh;
        else if (sh.type == 4)        // SHT_RELA
            sh_rela.push_back(shlist.size() - 1);
    }

    // Read section names from the section header string table
    if (eh.shstrndx >= shlist.size())
        throw std::runtime_error("invalid section header string table index");
    const SectionHeader& sh_shstrtab = shlist[eh.shstrndx];
    std::vector<uint8_t> shstrtab =
        read_section_data(fh, sh_shstrtab.offset, sh_shstrtab.size);
    for (auto& sh : shlist)
        sh.name = get_cstr(shstrtab, sh.nameidx);

    // Determine text/data/bss segment for each loaded (SHF_ALLOC) section.
    // Sections are treated as text until the first writable (SHF_WRITE)
    // section is reached; that section and every section after it are treated
    // as data.  SHT_NOBITS sections remain bss regardless of the boundary.
    bool in_data = false;
    for (auto& sh : shlist) {
        sh.segtype = -1;
        if (sh.flags & 0x2u) {        // SHF_ALLOC
            if (sh.flags & 0x1u)      // SHF_WRITE
                in_data = true;
            if (sh.type == 8)         // SHT_NOBITS
                sh.segtype = 2;
            else                      // has file content (PROGBITS, NOTE, etc.)
                sh.segtype = in_data ? 1 : 0;
        }
    }

    // Read relocation table
    std::vector<Rela> rellist;
    for (size_t idx : sh_rela) {
        const SectionHeader& sh = shlist[idx];
        if (sh.info >= shlist.size())
            throw std::runtime_error("relocation section has an invalid target section index");
        if (shlist[sh.info].flags & 0x2u) {  // SHF_ALLOC
            fh.seekg(sh.offset);
            std::streamoff start = fh.tellg();
            while (fh.tellg() - start < static_cast<std::streamoff>(sh.size))
                rellist.emplace_back(eh, fh);
        }
    }

    // Read symbol table
    std::vector<Symbol> symlist;
    if (sh_symtab != nullptr) {
        if (sh_symtab->link >= shlist.size())
            throw std::runtime_error("symbol table has an invalid string table index");
        fh.seekg(sh_symtab->offset);
        std::streamoff start = fh.tellg();
        while (fh.tellg() - start < static_cast<std::streamoff>(sh_symtab->size))
            symlist.emplace_back(eh, fh);

        // Get symbol names from strtab
        const SectionHeader& sh_strtab = shlist[sh_symtab->link];
        std::vector<uint8_t> strtab = read_section_data(fh, sh_strtab.offset, sh_strtab.size);
        for (auto& sym : symlist)
            sym.name = get_cstr(strtab, sym.nameidx);
    }

    // Read section data
    bool have_base = false;
    uint32_t baseaddr = 0;
    uint32_t curaddr = 0;
    int prevtype = -1;
    std::vector<uint8_t> contents[3];  // [0] text, [1] data, [2] bss

    for (const SectionHeader& sh : shlist) {
        if (!(sh.flags & 0x2u))        // SHF_ALLOC
            continue;
        if (!have_base) {
            baseaddr = sh.addr;
            curaddr = sh.addr;
            have_base = true;
        }
        if (prevtype >= 0 && sh.addr > curaddr)
            append_zeros(contents[prevtype], static_cast<size_t>(sh.addr - curaddr));

        if (sh.type == 8)             // SHT_NOBITS       ... .bss
        {
            prevtype = 2;
            append_zeros(contents[prevtype], sh.size);
        } else {                      // any ALLOC section with file content
            std::vector<uint8_t> data = read_section_data(fh, sh.offset, sh.size);
            prevtype = sh.segtype;
            if (prevtype < 0 || prevtype > 2)
                throw std::runtime_error("internal error: ALLOC section has no segment type");
            contents[prevtype].insert(contents[prevtype].end(), data.begin(), data.end());
        }
        curaddr = sh.addr + sh.size;
    }

    std::vector<uint8_t> body = contents[0];
    body.insert(body.end(), contents[1].begin(), contents[1].end());

    // Create relocation information for X68k.  Even-address relocations go to
    // `reldata` and odd-address ones to `oreldata`; each is a stream of
    // 16-bit offset deltas between consecutive targets, or {1, 32-bit delta}
    // when a delta does not fit in 16 bits.
    std::vector<uint8_t> reldata;
    std::vector<uint8_t> oreldata;
    int64_t prevoffset = 0;
    int64_t oprevoffset = 0;

    auto emplace_offdiff = [](std::vector<uint8_t>& out, int64_t offdiff) {
        if (offdiff < 0)
            throw std::runtime_error(
                "relocation entries are not ordered by increasing offset");
        if (offdiff < 0x10000) {
            put_u16be(out, static_cast<uint16_t>(offdiff));
        } else {
            put_u16be(out, 1);
            put_u32be(out, static_cast<uint32_t>(offdiff));
        }
    };

    for (const Rela& r : rellist) {
        if (r.sym >= symlist.size())
            throw std::runtime_error("relocation refers to a nonexistent symbol");
        const Symbol& sym = symlist[r.sym];
        bool force = force_reloc_syms.count(sym.name) != 0;
        if ((force || (sym.shndx != 0 && sym.shndx != 0xfff1)) && r.type < 4) {
            if (r.type != 1)  // R_68K_32
                throw std::runtime_error(
                    "unsupported relocation type (expected R_68K_32 = 1)");

            int64_t off = static_cast<int64_t>(r.offset) - static_cast<int64_t>(baseaddr);
            if (off < 0)
                throw std::runtime_error("relocation offset lies before the base address");
            if (static_cast<size_t>(off) + 4 > body.size())
                body.resize(static_cast<size_t>(off) + 4, 0);

            // Use the symbol value + explicit addend (SHT_RELA), rather than
            // the in-place bytes (which may be left as 0 by the linker).
            int64_t val = static_cast<int64_t>(sym.value) + r.addend
                        - static_cast<int64_t>(baseaddr) + static_cast<int64_t>(xbase);
            uint32_t val32 = static_cast<uint32_t>(val);   // Python: & 0xffffffff
            body[static_cast<size_t>(off) + 0] = static_cast<uint8_t>(val32 >> 24);
            body[static_cast<size_t>(off) + 1] = static_cast<uint8_t>((val32 >> 16) & 0xff);
            body[static_cast<size_t>(off) + 2] = static_cast<uint8_t>((val32 >> 8) & 0xff);
            body[static_cast<size_t>(off) + 3] = static_cast<uint8_t>(val32 & 0xff);

            if ((r.offset & 1) == 0) {  // normal relocation information
                int64_t offdiff = off - prevoffset;
                prevoffset = off;
                emplace_offdiff(reldata, offdiff);
            } else {                    // odd relocation information
                int64_t offdiff = off - oprevoffset;
                oprevoffset = off;
                emplace_offdiff(oreldata, offdiff);
            }
        }
    }

    size_t orlen = oreldata.size();
    if (orlen > 0) {
        for (const Symbol& sym : symlist) {
            if (sym.name == "__cxx_x68k_odd_relocation") {
                // Set the odd relocation info offset (points at the pool that
                // is appended right after the body).
                int64_t off = static_cast<int64_t>(sym.value) - static_cast<int64_t>(baseaddr);
                if (off < 0)
                    throw std::runtime_error(
                        "__cxx_x68k_odd_relocation symbol lies before the base address");
                if (static_cast<size_t>(off) + 4 > body.size())
                    body.resize(static_cast<size_t>(off) + 4, 0);
                uint32_t blen = static_cast<uint32_t>(body.size());
                body[static_cast<size_t>(off) + 0] = static_cast<uint8_t>(blen >> 24);
                body[static_cast<size_t>(off) + 1] = static_cast<uint8_t>((blen >> 16) & 0xff);
                body[static_cast<size_t>(off) + 2] = static_cast<uint8_t>((blen >> 8) & 0xff);
                body[static_cast<size_t>(off) + 3] = static_cast<uint8_t>(blen & 0xff);

                put_u16be(oreldata, 0);  // terminator

                // Add the odd relocation info into the data section
                if (contents[2].size() > orlen)
                    contents[2].resize(contents[2].size() - orlen);
                else
                    contents[2].clear();
                contents[1].insert(contents[1].end(), oreldata.begin(), oreldata.end());
                body.insert(body.end(), oreldata.begin(), oreldata.end());
                break;
            }
        }
    }

    // Create symbol table for X68k
    std::vector<uint8_t> symtbl;
    if (!strip) {
        for (const Symbol& sym : symlist) {
            if (sym.bind != 1)  // STB_GLOBAL
                continue;
            const SectionHeader* sh = nullptr;
            if (sym.shndx < 0xff00) {
                if (sym.shndx >= shlist.size())
                    throw std::runtime_error("symbol refers to a nonexistent section");
                sh = &shlist[sym.shndx];
            }
            if (sh != nullptr && (sh->flags & 0x2u)) {  // SHF_ALLOC
                uint16_t symtype = 0;
                if (sh->segtype == 0)             // ... .text
                    symtype = 0x0201;
                else if (sh->segtype == 1)        // ... .data
                    symtype = 0x0202;
                else if (sh->segtype == 2)        // ... .bss
                    symtype = 0x0203;

                if (symtype != 0) {
                    int64_t v = static_cast<int64_t>(sym.value)
                              - static_cast<int64_t>(baseaddr)
                              + static_cast<int64_t>(xbase);
                    std::vector<uint8_t> enc =
                        X68kSymbol(symtype, u32_checked(v, "symbol value"), sym.name).encode();
                    symtbl.insert(symtbl.end(), enc.begin(), enc.end());
                }
            }
        }
    }

    X68kHeader hdr(xbase,
                   u32_checked(static_cast<int64_t>(eh.entry) - static_cast<int64_t>(baseaddr)
                               + static_cast<int64_t>(xbase),
                               "entry point"),
                   static_cast<uint32_t>(contents[0].size()),
                   static_cast<uint32_t>(contents[1].size()),
                   static_cast<uint32_t>(contents[2].size()),
                   static_cast<uint32_t>(reldata.size()),
                   static_cast<uint32_t>(symtbl.size()));

    std::vector<uint8_t> out = hdr.encode();
    out.insert(out.end(), body.begin(), body.end());
    out.insert(out.end(), reldata.begin(), reldata.end());
    out.insert(out.end(), symtbl.begin(), symtbl.end());
    return out;
}

// ===========================================================================
// Command line interface (equivalent of the Python argparse setup)
// ===========================================================================

void print_usage(std::ostream& os, const char* prog) {
    os << "usage: " << prog << " [-h] [-o OUTPUT] [-b BASE] [-s] [-r SYMBOL] file\n"
       << "\n"
       << "ELF to X68k executable converter\n"
       << "\n"
       << "positional arguments:\n"
       << "  file                  Input ELF file\n"
       << "\n"
       << "options:\n"
       << "  -h, --help            show this help message and exit\n"
       << "  -o OUTPUT, --output OUTPUT\n"
       << "                        Output X68k exec file\n"
       << "  -b BASE, --base BASE  Set base address\n"
       << "  -s, --strip           Strip symbol table\n"
       << "  -r SYMBOL, --force-reloc-symbol SYMBOL\n"
       << "                        Force relocation of SYMBOL even when it is defined as\n"
       << "                        SHN_ABS (may be specified multiple times)\n";
}

int main(int argc, char** argv) {
    const char* prog = (argc > 0) ? argv[0] : "elf2x68k";

    std::string infile;
    std::string outfile;
    bool base_set = false;
    uint64_t base_val = 0;
    bool strip = false;
    std::set<std::string> force_reloc;

    auto usage_error = [&](const std::string& msg) -> int {
        print_usage(std::cerr, prog);
        std::cerr << prog << ": error: " << msg << "\n";
        return 2;
    };

    auto parse_base = [&](const std::string& v) -> int {
        try {
            base_val = std::stoull(v, nullptr, 0);  // like Python's int(x, 0)
            base_set = true;
            return 0;
        } catch (...) {
            return usage_error("argument -b/--base: invalid int value: '" + v + "'");
        }
    };

    int i = 1;
    int positional_start = argc;  // tokens after an explicit "--"

    for (; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--") { positional_start = i + 1; break; }

        // long options: --output, --output=X, --base, --base=X, ...
        if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {
            std::string opt = arg;
            std::string v;
            bool has_v = false;
            size_t eq = arg.find('=');
            if (eq != std::string::npos) {
                opt = arg.substr(0, eq);
                v = arg.substr(eq + 1);
                has_v = true;
            }
            if (opt == "--help") {
                if (has_v)
                    return usage_error("argument --help: ignored explicit argument '" + v + "'");
                print_usage(std::cout, prog);
                return 0;
            }
            if (opt == "--strip") {
                if (has_v)
                    return usage_error("argument --strip: ignored explicit argument '" + v + "'");
                strip = true;
                continue;
            }
            if (opt == "--output" || opt == "--base" || opt == "--force-reloc-symbol") {
                if (!has_v) {
                    if (i + 1 >= argc)
                        return usage_error("argument " + opt + ": expected one argument");
                    v = argv[++i];
                }
                if (opt == "--output") outfile = v;
                else if (opt == "--base") { if (int rc = parse_base(v)) return rc; }
                else force_reloc.insert(v);
                continue;
            }
            return usage_error("unrecognized arguments: " + arg);
        }

        // short options (possibly clustered): -s, -o out.x, -oout.x, -sb0x10, ...
        if (!arg.empty() && arg[0] == '-' && arg != "-") {
            size_t p = 1;
            while (p < arg.size()) {
                char c = arg[p];
                if (c == 'h') { print_usage(std::cout, prog); return 0; }
                if (c == 's') { strip = true; ++p; continue; }
                if (c == 'o' || c == 'b' || c == 'r') {
                    std::string v;
                    if (p + 1 < arg.size()) {
                        v = arg.substr(p + 1);
                    } else if (i + 1 < argc) {
                        v = argv[++i];
                    } else {
                        return usage_error(std::string("argument -") + c +
                                           ": expected one argument");
                    }
                    if (c == 'o') outfile = v;
                    else if (c == 'b') { if (int rc = parse_base(v)) return rc; }
                    else force_reloc.insert(v);
                    break;
                }
                return usage_error("unrecognized arguments: " + arg);
            }
            continue;
        }

        // positional argument (input file)
        if (infile.empty()) infile = arg;
        else return usage_error("unrecognized arguments: " + arg);
    }

    // arguments that came after "--"
    for (i = positional_start; i < argc; ++i) {
        if (infile.empty()) infile = argv[i];
        else return usage_error(std::string("unrecognized arguments: ") + argv[i]);
    }

    if (infile.empty())
        return usage_error("the following arguments are required: file");

    uint32_t base = 0;
    if (base_set) {
        if (base_val > 0xffffffffULL)
            return usage_error("argument -b/--base: value " + std::to_string(base_val) +
                               " exceeds 32 bits");
        base = static_cast<uint32_t>(base_val);
    }

    if (outfile.empty())
        outfile = infile + ".x";

    std::ifstream fi(infile, std::ios::binary);
    if (!fi) {
        std::cerr << prog << ": error: cannot open input file '" << infile << "'\n";
        return 1;
    }
    std::ofstream fo(outfile, std::ios::binary | std::ios::trunc);
    if (!fo) {
        std::cerr << prog << ": error: cannot open output file '" << outfile << "'\n";
        return 1;
    }

    try {
        std::vector<uint8_t> out = elf2x68k(fi, base, strip, force_reloc);
        fo.write(reinterpret_cast<const char*>(out.data()),
                 static_cast<std::streamsize>(out.size()));
        if (!fo) {
            std::cerr << prog << ": error: failed to write output file '" << outfile << "'\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << prog << ": error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
