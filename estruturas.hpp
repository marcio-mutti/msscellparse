#ifndef ESTRUTURAS
#define ESTRUTURAS

# include <set>
# include <string>
# include <vector>

class celula {

public:
  celula();
  ~celula();
  const bool operator<(const celula&) const;
  const bool operator == (const celula&) const;
  void set_cgi(const int&, const int&, const int&, const int&);
  void set_name(const std::string&);
  void set_mcc(const int&);
  void set_mnc(const int&);
  void set_lac_sac(const int&);
  void set_cid(const int&);
  void set_status(const std::string&);
  const std::string get_name() const;
  const int get_mcc() const;
  const int get_mnc() const;
  const int get_lac_sac() const;
  const int get_cid() const;
  const std::string get_status() const;

protected:
  int mcc, mnc, lac_sac, cid;
  std::string name, status;
};

class controller {

public:
  controller();
  ~controller();
  const bool has_lac_sac(const int&) const;
  std::vector<celula>::iterator add_celula(const celula&);
  std::vector<celula>::iterator find_celula(const celula&);
  const bool has_celula(const celula&);
  const std::string get_name() const;
  void set_name(const std::string&);
  void add_lac_sac(const int&);
  const bool operator==(const controller&) const;

protected:
  std::string name;
  std::vector<celula> lista_de_celulas;
  std::set<int> lista_de_lacs_sacs;
};

class bsc : public controller {
  
};

class rnc : public controller {
  
};

class mobswitch {

public:
  mobswitch();
  ~mobswitch();
  void set_name(const std::string&);
  std::vector<bsc>::iterator add_bsc(const bsc&);
  std::vector<rnc>::iterator add_rnc(const rnc&);
  std::vector<bsc>::iterator find_bsc_by_name(const std::string&);
  std::vector<rnc>::iterator find_rnc_by_name(const std::string&);
  std::vector<rnc>::iterator find_rnc_by_sac(const int&);
  const bool has_sac(const int&);

protected:
  std::string name;
  std::vector<bsc> lista_de_bscs;
  std::vector<rnc> lista_de_rncs;
};

class mme {
    
};
#endif

