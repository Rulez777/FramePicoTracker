#pragma once
#include "roy_owner.hpp"
namespace pico {
std::string sha256(const Bytes &);
struct ModuleImage {
    uint32_t entry;
    Bytes code;
};
struct RoyLayoutReport {
    bool ok = false;
    std::string module_sha, flash_sha;
};
RoyLayoutReport roy_layout_report();
ModuleImage elf_module(const Bytes &);
// Exact protected stock/original210 ELF, never the copied release ELF.
ModuleImage admit_original210(const Bytes &stock, const Bytes &elf);
void install_original210(SramOwner&,const Bytes& stock,const Bytes& elf,const Bytes& prepared_state);
} // namespace pico
