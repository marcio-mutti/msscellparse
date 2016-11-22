#include "estruturas.hpp"

#include <algorithm>

using namespace std;

// Metodos relacionados a celulas
celula::celula() {}
celula::~celula() {}
bool celula::operator<(const celula& oth) const {
    return mcc != oth.mcc ? mcc < oth.mcc :
           (mnc != oth.mnc ? mnc < oth.mnc :
            (lac_sac != oth.lac_sac ? lac_sac < oth.lac_sac :
             (cid < oth.cid)
            )
           );
}
bool celula::operator==(const celula& oth) const {
    return (cid == oth.cid) && (lac_sac == oth.lac_sac) && (mnc == oth.mnc) && (mcc == oth.mcc);
}
void celula::set_cgi(const int& mcc_, const int& mnc_, const int& lac_sac_, const int& cid_) {
    mcc=mcc_;
    mnc=mnc_;
    lac_sac=lac_sac_;
    cid=cid_;
}
void celula::set_cgi(const string& mcc_, const string& mnc_, const string& lac_sac_,
                     const string& cid_) {
    set_cgi(stoi(mcc_),stoi(mnc_),stoi(lac_sac_),stoi(cid_));
}
void celula::set_name(const std::string& name_)     {
    name=name_;
}
void celula::set_mcc(const int& mcc_)               {
    mcc=mcc_;
}
void celula::set_mnc(const int& mnc_)               {
    mnc=mnc_;
}
void celula::set_lac_sac(const int& lac_sac_)       {
    lac_sac=lac_sac_;
}
void celula::set_cid(const int& cid_)               {
    cid=cid_;
}
void celula::set_status(const string& status_)      {
    status=status_;
}
const string celula::get_name() const               {
    return name;
}
string celula::get_mcc() const                   {
    return to_string(mcc);
}
string celula::get_mnc() const                   {
    return to_string(mnc);
}
string celula::get_lac_sac() const               {
    return to_string(lac_sac);
}
string celula::get_cid() const                   {
    return to_string(cid);
}
const string celula::get_status() const             {
    return status;
}

//Metodos relacionados a controller
controller::controller() : name(), mnc(-1)  {}
controller::~controller()                   {}
bool controller::has_lac_sac(const int& lac_sac) const {
    return (lista_de_lacs_sacs.find(lac_sac) != lista_de_lacs_sacs.cend());
}
vector<celula>::iterator controller::add_celula(const celula& celula_) {
    vector<celula>::iterator localizer(find_celula(celula_));
    if (lista_de_celulas.end() != localizer) return localizer;
    return lista_de_celulas.insert(lista_de_celulas.end(),celula_);
}
vector<celula>::iterator controller::find_celula(const celula& celula_) {
    for (vector<celula>::iterator citer=lista_de_celulas.begin(); citer!=lista_de_celulas.end();
            ++citer)
        if (*citer == celula_) return citer;
    return lista_de_celulas.end();
}

vector<celula>::const_iterator controller::cell_begin() const {
    return lista_de_celulas.cbegin();
}

vector<celula>::const_iterator controller::cell_end() const {
    return lista_de_celulas.cend();
}
bool controller::operator==(const controller& oth) const {
    return name==oth.name;
}

bool controller::has_celula(const celula& celula_) {
    return !(lista_de_celulas.end() == find_celula(celula_));
}
const string controller::get_name() const               {
    return name;
}
void controller::set_name(const string& name_)          {
    name=name_;
}
void controller::add_lac_sac(const int& lac_sac_)       {
    lista_de_lacs_sacs.insert(lac_sac_);
}
void controller::add_lac_sac(const string& lac_sac_)    {
    add_lac_sac(stoi(lac_sac_));
}
void controller::set_mnc ( const int& mnc_)             {
    mnc=mnc_;
}
int controller::get_mnc() const                   {
    return mnc;
}

void controller::operator+=(const controller & oth) {
    for (vector<celula>::const_iterator citer=oth.lista_de_celulas.cbegin();
            citer!=lista_de_celulas.cend(); ++citer) {
        add_celula(*citer);
    }
}

//Metodos relacionados a BSCs



//Metodos relacionados a RNCs
void rnc::set_id(const std::string& id_)                {
    id=stoi(id_);
}
void rnc::set_mnc(const string& mnc_)                   {
    controller::set_mnc(stoi(mnc_));
}

//Metodos relacionados a mobswitches
mobswitch::mobswitch() {}
mobswitch::~mobswitch() {}
void mobswitch::set_name(const string& name_)           {
    name=name_;
}
string mobswitch::get_name() const {
    return name;
}
vector<bsc>::iterator mobswitch::add_bsc(const bsc& bsc_) {
    vector<bsc>::iterator result(find(lista_de_bscs.begin(),lista_de_bscs.end(),bsc_));
    if (result != lista_de_bscs.end()) return result;
    return lista_de_bscs.insert(lista_de_bscs.end(),bsc_);
}
vector<rnc>::iterator mobswitch::add_rnc(const rnc& rnc_) {
    vector<rnc>::iterator result(find(lista_de_rncs.begin(),lista_de_rncs.end(),rnc_));
    if (result != lista_de_rncs.end()) return result;
    return lista_de_rncs.insert(lista_de_rncs.end(),rnc_);
}
vector<bsc>::iterator mobswitch::find_bsc_by_name(const string& bsc_) {
    vector<bsc>::iterator result(lista_de_bscs.begin());
    for(; result!=lista_de_bscs.end(); ++result) {
        if (result->get_name() == bsc_) break;
    }
    return result;
}
vector<rnc>::iterator mobswitch::find_rnc_by_name(const string& rnc_) {
    vector<rnc>::iterator result(lista_de_rncs.begin());
    for(; result!=lista_de_rncs.end(); ++result) {
        if (result->get_name() == rnc_) break;
    }
    return result;
}
vector<rnc>::iterator mobswitch::find_rnc_by_sac(const int& sac) {
    for (vector<rnc>::iterator riter=lista_de_rncs.begin(); riter!=lista_de_rncs.end(); ++riter)
        if (riter->has_lac_sac(sac)) return riter;
    return lista_de_rncs.end();
}

vector<bsc>::const_iterator mobswitch::bsc_begin() const {
    return lista_de_bscs.cbegin();
}

vector<bsc>::const_iterator mobswitch::bsc_end() const {
    return lista_de_bscs.cend();
}

vector<rnc>::const_iterator mobswitch::rnc_begin() const {
    return lista_de_rncs.cbegin();
}

vector<rnc>::const_iterator mobswitch::rnc_end() const {
    return lista_de_rncs.cend();
}
bool mobswitch::has_sac(const int& sac) {
    return (lista_de_rncs.end() != find_rnc_by_sac(sac));
}

void mobswitch::operator+=(const mobswitch &oth) {
    if (oth.lista_de_bscs.size() > 0)
        for (size_t i=0; i!=lista_de_bscs.size(); ++i)
            add_bsc(oth.lista_de_bscs.at(i));
    /*
        for (vector<bsc>::const_iterator biter=oth.lista_de_bscs.cbegin();
                biter!=lista_de_bscs.cend(); ++biter) {
            add_bsc(*biter);
        }*/
    if (oth.lista_de_rncs.size() > 0)
        for (size_t i = 0; i!=oth.lista_de_rncs.size(); ++i)
            add_rnc(oth.lista_de_rncs.at(i));
    /*for (vector<rnc>::const_iterator riter=oth.lista_de_rncs.cbegin();
            riter!=lista_de_rncs.cend(); ++riter) {
        add_rnc(*riter);
    }*/
}

bool mobswitch::operator==(const mobswitch &oth) const {
    return name == oth.name;
}
size_t mobswitch::get_number_bscs() const {
    return lista_de_bscs.size();
}
size_t mobswitch::get_number_rncs() const {
    return lista_de_rncs.size();
}

enodeb::enodeb() : ip(), mcc(0), mnc(0), tac(0), enbid(0), s1ca(0) {

}

enodeb::~enodeb() {

}

void enodeb::set_ip(const string & ip_) {
    ip=ip_;
}

void enodeb::set_mcc(const string & mcc_) {
    mcc=stoul(mcc_);
}

void enodeb::set_mnc(const string &mnc_) {
    mnc=stoul(mnc_);
}

void enodeb::set_tac(const string & tac_) {
    tac=stoul(tac_);
}

void enodeb::set_enbid(const string &enbid_) {
    enbid=stoul(enbid_);
}

void enodeb::set_s1ca(const string &s1ca_) {
    s1ca=stoul(s1ca_);
}

string enodeb::get_ip() const {
    return ip;
}

string enodeb::get_mcc() const {
    return to_string(mcc);
}

string enodeb::get_mnc() const {
    return to_string(mnc);
}

string enodeb::get_tac() const {
    return to_string(tac);
}

string enodeb::get_enbid() const {
    return to_string(enbid);
}

string enodeb::get_s1ca() const {
    return to_string(s1ca);
}

bool enodeb::operator==(const enodeb &oth) const {
    return ip == oth.ip;
}

mme::mme() {

}

mme::~mme() {

}

void mme::set_name(const string &name_) {
    name=name_;
}

string mme::get_name() const {
    return name;
}

vector<enodeb>::iterator mme::add_enodeb(const enodeb & n_enodeb) {
    vector<enodeb>::iterator result=find_enodeb(n_enodeb);
    if (result==lista_de_enodebs.end()) {
        result=lista_de_enodebs.insert(result,n_enodeb);
    }
    return result;
}

vector<enodeb>::iterator mme::find_enodeb(const enodeb & f_enodeb) {
    for (vector<enodeb>::iterator eiter=lista_de_enodebs.begin(); eiter!=lista_de_enodebs.end();
            ++eiter) {
        if (*eiter == f_enodeb) return eiter;
    }
    return lista_de_enodebs.end();
}

vector<enodeb>::const_iterator mme::enodeb_begin() const {
    return lista_de_enodebs.cbegin();
}

vector<enodeb>::const_iterator mme::enodeb_end() const {
    return  lista_de_enodebs.cend();
}
