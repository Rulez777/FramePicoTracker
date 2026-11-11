#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string path = "/home/steamos/pico-ram-work/native-port/native/native_identity.cpp";
    std::ifstream in(path);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    
    std::string target = "bool NativeIdentitySession::begin_if_ready(PeerEngine &e,const std::string &link,int64_t now,bool controllers) {\n    if(!std::getenv(\"PICO_IDENTITY_EPISODES\")) return false;";
    std::string repl = "bool NativeIdentitySession::begin_if_ready(PeerEngine &e,const std::string &link,int64_t now,bool controllers) {\n    if(!std::getenv(\"PICO_IDENTITY_EPISODES\")) { std::cerr << \"begin_if_ready: no env PICO_IDENTITY_EPISODES\\n\"; return false; }";
    auto pos = content.find(target);
    if (pos == std::string::npos) {
        std::cerr << "target not found!\n";
        return 1;
    }
    content.replace(pos, target.length(), repl);
    
    std::string target2 = "if(invalid_ || recovery_only_ || owner.abort_requested || link!=link_ || !fresh(stamp,now)) return false;";
    std::string repl2 = "if(invalid_ || recovery_only_ || owner.abort_requested || link!=link_ || !fresh(stamp,now)) {\n        std::cerr << \"begin_if_ready: invalid=\" << invalid_ << \" rec=\" << recovery_only_ << \" abort=\" << owner.abort_requested << \" link_mismatch=\" << (link!=link_) << \" fresh=\" << fresh(stamp,now) << \" stamp=\" << stamp << \" now=\" << now << \"\\n\";\n        return false;\n    }";
    pos = content.find(target2);
    if (pos == std::string::npos) {
        std::cerr << "target2 not found!\n";
        return 1;
    }
    content.replace(pos, target2.length(), repl2);
    
    std::ofstream out(path);
    out << content;
    out.close();
    std::cout << "Applied patch successfully\n";
    return 0;
}
