#include "native_test_runtime.hpp"
#include "data_plane.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cctype>
using namespace pico;
namespace {
// Small strict JSON reader. Integers remain decimal strings until requested;
// no double conversion of nanosecond evidence. Nested unknown fields accepted.
struct Value {
    char type = 'n';
    std::string text;
    std::map<std::string, Value> object;
    std::vector<Value> array;
};
class Json {
    const std::string &s;
    size_t p = 0;
    void ws() {
        while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p])))
            ++p;
    }
    char take() {
        if (p == s.size())
            throw std::runtime_error("JSON EOF");
    return s[p++];
    }
    std::string str() {
        if (take() != '"')
            throw std::runtime_error("JSON string");
        std::string o;
        while (true) {
            char c = take();
            if (c == '"')
                return o;
            if (static_cast<unsigned char>(c) < 32)
                throw std::runtime_error("JSON control");
            if (c == '\\') {
                c = take();
                switch (c) {
                case '"':
                case '\\':
                case '/':
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'u': {
                    unsigned code = 0;
                    for (int i = 0; i < 4; ++i) {
                        char h = take();
                        int v = h >= '0' && h <= '9'   ? h - '0'
                                : h >= 'a' && h <= 'f' ? h - 'a' + 10
                                : h >= 'A' && h <= 'F' ? h - 'A' + 10
                                                       : -1;
                        if (v < 0)
                            throw std::runtime_error("JSON unicode");
                        code = code * 16 + unsigned(v);
                    }
                    if (code >= 0xd800 && code <= 0xdfff)
                        throw std::runtime_error("JSON surrogate unsupported");
                    if (code < 128)
                        o += char(code);
                    else if (code < 2048) {
                        o += char(0xc0 | (code >> 6));
                        o += char(0x80 | (code & 63));
                    } else {
                        o += char(0xe0 | (code >> 12));
                        o += char(0x80 | ((code >> 6) & 63));
                        o += char(0x80 | (code & 63));
                    }
                    continue;
                }
                default:
                    throw std::runtime_error("JSON escape");
                }
            }
            o += c;
        }
    }
    Value value(unsigned depth) {
        if (depth > 64)
            throw std::runtime_error("JSON depth");
        ws();
        Value v;
        if (p == s.size())
            throw std::runtime_error("JSON empty");
        char c = s[p];
        if (c == '"') {
            v.type = 's';
            v.text = str();
            return v;
        }
        if (c == '{' || c == '[') {
            ++p;
            v.type = c;
            ws();
            char close = c == '{' ? '}' : ']';
            if (p < s.size() && s[p] == close) {
                ++p;
                return v;
            }
            while (true) {
                ws();
                if (c == '{') {
                    auto key = str();
                    ws();
                    if (take() != ':')
                        throw std::runtime_error("JSON colon");
                    if (!v.object.emplace(key, value(depth + 1)).second)
                        throw std::runtime_error("JSON duplicate key");
                } else
                    v.array.push_back(value(depth + 1));
                ws();
                char sep = take();
                if (sep == close)
                    break;
                if (sep != ',')
                    throw std::runtime_error("JSON separator");
            }
            return v;
        }
        size_t begin = p;
        while (p < s.size() && s[p] != ',' && s[p] != ']' && s[p] != '}' &&
               !std::isspace(static_cast<unsigned char>(s[p])))
            ++p;
        v.text = s.substr(begin, p - begin);
        if (v.text == "true" || v.text == "false") {
            v.type = 'b';
            return v;
        }
        if (v.text == "null")
            return v;
        // Validate the complete JSON number grammar, even unknown fields.
        size_t i = 0;
        if (i < v.text.size() && v.text[i] == '-')
            ++i;
        if (i == v.text.size())
            throw std::runtime_error("JSON number");
        if (v.text[i] == '0')
            ++i;
        else {
            if (v.text[i] < '1' || v.text[i] > '9')
                throw std::runtime_error("JSON token");
            while (i < v.text.size() && std::isdigit(static_cast<unsigned char>(v.text[i])))
                ++i;
        }
        for (char marker : {'.', 'e'}) {
            if (i < v.text.size() && (v.text[i] == marker || (marker == 'e' && v.text[i] == 'E'))) {
                ++i;
                if (marker == 'e' && i < v.text.size() && (v.text[i] == '+' || v.text[i] == '-'))
                    ++i;
                size_t b = i;
                while (i < v.text.size() && std::isdigit(static_cast<unsigned char>(v.text[i])))
                    ++i;
                if (i == b)
                    throw std::runtime_error("JSON number digits");
            }
        }
        if (i != v.text.size())
            throw std::runtime_error("JSON number tail");
        v.type = 'i';
        return v;
    }

  public:
    explicit Json(const std::string &text) : s(text) {}
    Value parse() {
        Value v = value(0);
        ws();
        if (p != s.size())
            throw std::runtime_error("JSON tail");
    return v;
    }
};
const Value &field(const Value &v, const char *k) {
    auto i = v.object.find(k);
    if (i == v.object.end())
        throw std::runtime_error(std::string("missing ") + k);
    return i->second;
}
int64_t integer(const Value &v) {
    if (v.type != 'i' || v.text.find_first_of(".eE") != std::string::npos)
        throw std::runtime_error("expected integer");
    size_t end = 0;
    auto n = std::stoll(v.text, &end);
    if (end != v.text.size())
        throw std::runtime_error("integer tail");
    return n;
}
int64_t number(const Value &v, const char *k) { return integer(field(v, k)); }
uint32_t u32(const Value &v, const char *k) {
    auto n = number(v, k);
    if (n < 0 || n > UINT32_MAX)
        throw std::runtime_error("u32 range");
    return uint32_t(n);
}
bool flag(const Value &v, const char *k) {
    const auto &b = field(v, k);
    return b.type == 'b' && b.text == "true";
}
void emit(DataPlane &dp) {
    while (auto e = dp.next_event()) {
        const auto &s = e->item.sample;
        const auto &t = e->item.stamps;
        std::cout << "{\"version\":1,\"event\":\"imu_release\",\"key\":" << e->item.key
                  << ",\"chip\":" << e->chip << ",\"sequence\":" << e->sequence
                  << ",\"slot\":" << e->slot << ",\"counter\":" << unsigned(s.counter)
                  << ",\"roy_cycles\":" << e->item.cycles << ",\"gyro\":[" << s.gyro[0] << ','
                  << s.gyro[1] << ',' << s.gyro[2] << "],\"accel\":[" << s.accel[0] << ','
                  << s.accel[1] << ',' << s.accel[2] << "],\"temperature\":" << s.temperature
                  << ",\"sync_ticks\":" << s.ticks << ",\"disposition\":\"" << e->disposition
                  << "\",\"pti1_hex\":";
        if (e->wire)
            std::cout << '"' << hex(e->wire->data(), 32) << '"';
        else
            std::cout << "null";
        std::cout << ",\"stamps\":{\"ring_start\":" << t.ring_start
                  << ",\"ring_end\":" << t.ring_end << ",\"snapshot_free\":" << t.snapshot_free
                  << ",\"poll_start\":" << t.poll_start << ",\"poll_end\":" << t.poll_end
                  << ",\"admitted\":" << t.admitted << ",\"decode\":" << t.decode
                  << ",\"insert\":" << t.insert << ",\"release\":" << t.release
                  << ",\"convert\":" << t.convert << ",\"send_begin\":" << t.send_begin
                  << ",\"send_end\":" << t.send_end << "}}\n";
    }
}
} // namespace
int main(int argc, char **argv) {
    try {
        if (argc != 2) {
            std::cerr << "usage: radio_replay records.jsonl (or radio-run.json)\n";
            return 2;
        }
        std::ifstream file(argv[1]);
        if (!file)
            throw std::runtime_error("open input");
        int64_t now = 0;
        Hooks hooks;
        hooks.now = [&] { return now; };
        hooks.canonical = [](const Record &r, const std::vector<Command> &c,
                             const std::optional<AckContext> &context) {
            std::cout << "{\"version\":1,\"event\":\"canonical\",\"sequence\":" << r.sequence
                      << ",\"commands\":[";
            for (size_t i = 0; i < c.size(); ++i) {
                if (i)
                    std::cout << ',';
                std::cout << "{\"tag\":" << unsigned(c[i].tag)
                          << ",\"command\":" << unsigned(c[i].command) << ",\"body_hex\":\""
                          << hex(c[i].body.data(), c[i].body.size()) << "\"}";
            }
            std::cout << "],\"ack_context\":";
            if (context)
                std::cout << "{\"generation\":" << context->generation
                          << ",\"source_ns\":" << context->source_ns
                          << ",\"uncertainty_ns\":" << context->uncertainty_ns << '}';
            else
                std::cout << "null";
            std::cout << "}\n";
        };
        hooks.discovery = [](const Record &) {
            std::cout << "{\"version\":1,\"event\":\"discovery_control_required\"}\n";
        };
        hooks.decoded = [](const Record &r, const Sample &s) {
            std::cout << "{\"version\":1,\"event\":\"imu_decoded\",\"sequence\":" << r.sequence
                      << ",\"counter\":" << unsigned(s.counter) << ",\"roy_cycles\":" << r.cycles
                      << ",\"gyro\":[" << s.gyro[0] << ',' << s.gyro[1] << ',' << s.gyro[2]
                      << "],\"accel\":[" << s.accel[0] << ',' << s.accel[1] << ',' << s.accel[2]
                      << "],\"temperature\":" << s.temperature << ",\"sync_ticks\":" << s.ticks
                      << "}\n";
        };
        DataPlane dp(hooks); // explicit replay enrollment only, no identity assertion
        dp.enrolled = true;
        auto process = [&](const Value &v) {
            auto kind = v.object.find("event");
            if (kind != v.object.end() && kind->second.text == "clock_probe") {
                std::optional<int64_t> rb, ra;
                if (v.object.count("raw_before_ns"))
                    rb = number(v, "raw_before_ns");
                if (v.object.count("raw_after_ns"))
                    ra = number(v, "raw_after_ns");
                bool accepted = dp.clock.observe(number(v, "roy_cycles"), number(v, "before_ns"),
                                                 number(v, "after_ns"), rb, ra);
                std::cout << "{\"version\":1,\"event\":\"clock_fit\",\"accepted\":"
                          << (accepted ? "true" : "false") << "}\n";
                return;
            }
            Record r;
            r.host_read_ns = number(v, "host_read_monotonic_ns");
            now = r.host_read_ns;
            // Declared schedule: host read time stands in for processing time.
            // It is not claimed to be the original live reorder insertion time.
            if (v.object.count("replay_processing_ns"))
                now = number(v, "replay_processing_ns");
            r.cycles = u32(v, "roy_cycles");
            r.chip = u32(v, "source_chip_id");
            r.crc = u32(v, "crc");
            if (v.object.count("tx_generation"))
                r.generation = u32(v, "tx_generation");
            r.channel = u32(v, "channel");
            auto pi = v.object.find("profile_index");
            if (pi != v.object.end() && pi->second.type != 'n') {
                auto profile = integer(pi->second);
                if (profile < 0 || profile > 2)
                    throw std::runtime_error("profile range");
                r.profile = int(profile);
            }
            r.hardware_crc = flag(v, "hardware_crc_valid");
            r.partial = flag(v, "partial");
            if (field(v, "partial").type != 'b')
                throw std::runtime_error("partial must be boolean");
            if (v.object.count("sequence"))
                r.sequence = u32(v, "sequence");
            if (v.object.count("slot_number_mod32768"))
                r.slot = u32(v, "slot_number_mod32768");
            const auto &dma = field(v, "dma_hex");
            if (dma.type != 's')
                throw std::runtime_error("dma string");
            r.dma = unhex(dma.text);
            Stamps t;
            t.ring_end = r.host_read_ns;
            dp.record(r, t);
            emit(dp);
        };
        std::ostringstream contents;
        contents << file.rdbuf();
        std::string input = contents.str();
        std::optional<Value> document;
        try {
            document = Json(input).parse();
        } catch (const std::exception &) {
        }
        std::string line;
        size_t lines = 0;
        bool failed = false;
        if (document && document->object.count("raw_records")) {
            std::cerr << "radio-run raw_records: retention may be truncated\n";
            const auto &records = field(*document, "raw_records");
            if (records.type != '[')
                throw std::runtime_error("raw_records array required");
            for (const auto &r : records.array)
                process(r);
        } else {
            std::istringstream stream(input);
            while (std::getline(stream, line)) {
                ++lines;
                if (line.empty())
                    continue;
                try {
                    process(Json(line).parse());
                } catch (const std::exception &e) {
                    failed = true;
                    std::cerr << "line " << lines << ": " << e.what() << '\n';
                }
            }
        }
        const auto &c = dp.counters;
        const auto &r = dp.reorder.stats;
        auto histogram = [](const char *name, const Histogram &h) {
            std::cout << "{\"version\":1,\"event\":\"histogram\",\"name\":\"" << name
                      << "\",\"upper_bounds_ns\":[1000000,10000000,50000000,100000000,250000000,"
                         "null],\"bins\":[";
            for (size_t i = 0; i < h.bins.size(); ++i) {
                if (i)
                    std::cout << ',';
                std::cout << h.bins[i];
            }
            std::cout << "],\"negative\":" << h.negative << "}\n";
        };
        histogram("receive_age", c.receive_age);
        const char *hops[] = {"ring_decode", "decode_release", "release_convert", "convert_send",
                              "send_duration"};
        for (size_t i = 0; i < 5; ++i)
            histogram(hops[i], c.hop_latency[i]);
        std::cout << "{\"version\":1,\"event\":\"summary\",\"decoded\":" << c.decoded
                  << ",\"released\":" << c.samples << ",\"encoded\":" << c.encoded
                  << ",\"pending\":" << dp.reorder.pending() << ",\"malformed\":" << c.malformed
                  << ",\"checksum\":" << c.checksum << ",\"source\":" << c.source
                  << ",\"crc\":" << c.crc << ",\"no_clock\":" << c.no_clock
                  << ",\"stale_roy\":" << c.stale_roy << ",\"duplicates\":" << r.identical_repeats
                  << ",\"conflicts\":" << r.conflicting_counters
                  << ",\"late\":" << r.late_after_release << "}\n";
        return failed ? 1 : 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
