#include "estruturas.hpp"

#include <algorithm>

using namespace std;

// Metodos relacionados a celulas
celula::celula() {}
celula::~celula() {}
const bool celula::operator<(const celula& oth) const {
	return mcc != oth.mcc ? mcc < oth.mcc :
		(mnc != oth.mnc ? mnc < oth.mnc :
			(lac_sac != oth.lac_sac ? lac_sac < oth.lac_sac :
				(cid < oth.cid)
			)
		);
}
const bool celula::operator==(const celula& oth) const {
	return (cid == oth.cid) && (lac_sac == oth.lac_sac) && (mnc == oth.mnc) && (mcc == oth.mcc);
}
void celula::set_cgi(const int& mcc_, const int& mnc_, const int& lac_sac_, const int& cid_) {
	mcc=mcc_;mnc=mnc_;lac_sac=lac_sac_;cid=cid_;
}
void celula::set_name(const std::string& name_)     { name=name_;}
void celula::set_mcc(const int& mcc_)               { mcc=mcc_; }
void celula::set_mnc(const int& mnc_)               { mnc=mnc_; }
void celula::set_lac_sac(const int& lac_sac_)       { lac_sac=lac_sac_; }
void celula::set_cid(const int& cid_)               { cid=cid_; }
void celula::set_status(const string& status_)      { status=status_; }
const string celula::get_name() const               { return name; }
const int celula::get_mcc() const                   { return mcc; }
const int celula::get_mnc() const                   { return mnc; }
const int celula::get_lac_sac() const               { return lac_sac; }
const int celula::get_cid() const                   { return cid; }
const string celula::get_status() const             { return status; }

//Metodos relacionados a controller
controller::controller()	{}
controller::~controller()	{}
const bool controller::has_lac_sac(const int& lac_sac) const {
	return (lista_de_lacs_sacs.find(lac_sac) != lista_de_lacs_sacs.cend());
}
vector<celula>::iterator controller::add_celula(const celula& celula_) {
	vector<celula>::iterator localizer(find_celula(celula_));
	if (lista_de_celulas.end() != localizer) return localizer;
	return lista_de_celulas.insert(lista_de_celulas.end(),celula_);
}
vector<celula>::iterator controller::find_celula(const celula& celula_) {
	for (vector<celula>::iterator citer=lista_de_celulas.begin(); citer!=lista_de_celulas.end(); ++citer)
		if (*citer == celula_) return citer;
	return lista_de_celulas.end();
}
const bool controller::operator==(const controller& oth) const { return name==oth.name; }

const bool controller::has_celula(const celula& celula_) {
	return !(lista_de_celulas.end() == find_celula(celula_));
}
const string controller::get_name() const           { return name; }
void controller::set_name(const string& name_)      { name=name_; }
void controller::add_lac_sac(const int& lac_sac_)   { lista_de_lacs_sacs.insert(lac_sac_); }


//Metodos relacionados a BSCs



//Metodos relacionados a RNCs



//Metodos relacionados a mobswitches
mobswitch::mobswitch() {}
mobswitch::~mobswitch() {}
void mobswitch::set_name(const string& name_)				{ name=name_; }
vector<bsc>::iterator mobswitch::add_bsc(const bsc& bsc_) {
    vector<bsc>::iterator result(find(lista_de_bscs.begin(),lista_de_bscs.end(),bsc_.get_name()));
    if (result != lista_de_bscs.end()) return result;
    return lista_de_bscs.insert(lista_de_bscs.end(),bsc_);
}
vector<rnc>::iterator mobswitch::add_rnc(const rnc& rnc_) {
    vector<rnc>::iterator result(find(lista_de_rncs.begin(),lista_de_rncs.end(),rnc_.get_name()));
    if (result != lista_de_rncs.end()) return result;
    return lista_de_rncs.insert(lista_de_rncs.end(),rnc_);
}
vector<bsc>::iterator mobswitch::find_bsc_by_name(const string& bsc_) {
	vector<bsc>::iterator result(lista_de_bscs.begin());
	for(;result!=lista_de_bscs.end();++result) {
		if (result->get_name() == bsc_) break;
	}
	return result;
}
vector<rnc>::iterator mobswitch::find_rnc_by_name(const string& rnc_) {
	vector<rnc>::iterator result(lista_de_rncs.begin());
	for(;result!=lista_de_rncs.end();++result) {
		if (result->get_name() == rnc_) break;
	}
	return result;
}
vector<rnc>::iterator mobswitch::find_rnc_by_sac(const int& sac)
{
	for (vector<rnc>::iterator riter=lista_de_rncs.begin();riter!=lista_de_rncs.end();++riter)
		if (riter->has_lac_sac(sac)) return riter;
	return lista_de_rncs.end();
}
const bool mobswitch::has_sac(const int& sac) {
	return (lista_de_rncs.end() != find_rnc_by_sac(sac));
}




















