#ifndef ESTRUTURAS
#define ESTRUTURAS

# include <set>
# include <string>
# include <vector>

class celula {

  public:
    celula();
    ~celula();
    bool operator<(const celula&) const;
    bool operator == (const celula&) const;
    void set_cgi(const int&, const int&, const int&, const int&);
    void set_cgi(const std::string&, const std::string&, const std::string&, const std::string&);
    void set_name(const std::string&);
    void set_mcc(const int&);
    void set_mnc(const int&);
    void set_lac_sac(const int&);
    void set_cid(const int&);
    void set_status(const std::string&);
    const std::string get_name() const;
    std::string get_mcc() const;
    std::string get_mnc() const;
    std::string get_lac_sac() const;
    std::string get_cid() const;
    const std::string get_status() const;

  protected:
    int mcc, mnc, lac_sac, cid;
    std::string name, status;
};

class controller {

  public:
    controller();
    virtual ~controller();
    bool has_lac_sac(const int&) const;
    std::vector<celula>::iterator add_celula(const celula&);
    std::vector<celula>::iterator find_celula(const celula&);
    std::vector<celula>::const_iterator cell_begin() const;
    std::vector<celula>::const_iterator cell_end() const;
    bool has_celula(const celula&);
    const std::string get_name() const;
    void set_name(const std::string&);
    void add_lac_sac(const int&);
    void add_lac_sac(const std::string&);
    bool operator==(const controller&) const;
    void set_mnc(const int&);
    int get_mnc() const;
    void operator+=(const controller&);

  protected:
    std::string name;
    int mnc;
    std::vector<celula> lista_de_celulas;
    std::set<int> lista_de_lacs_sacs;
};

class bsc : public controller {

};

class rnc : public controller {
  public:
    void set_id(const std::string&);
    void set_mnc(const std::string&);
  protected:
    int id;
};

class mobswitch {

  public:
    mobswitch();
    ~mobswitch();
    void set_name(const std::string&);
    std::string get_name() const;
    std::vector<bsc>::iterator add_bsc(const bsc&);
    std::vector<rnc>::iterator add_rnc(const rnc&);
    std::vector<bsc>::iterator find_bsc_by_name(const std::string&);
    std::vector<rnc>::iterator find_rnc_by_name(const std::string&);
    std::vector<rnc>::iterator find_rnc_by_sac(const int&);
    std::vector<bsc>::const_iterator bsc_begin() const;
    std::vector<bsc>::const_iterator bsc_end() const;
    std::vector<rnc>::const_iterator rnc_begin() const;
    std::vector<rnc>::const_iterator rnc_end() const;
    bool has_sac(const int&);
    void operator+=(const mobswitch&);
    bool operator==(const mobswitch&) const;
    size_t get_number_bscs() const;
    size_t get_number_rncs() const;

  protected:
    std::string name;
    std::vector<bsc> lista_de_bscs;
    std::vector<rnc> lista_de_rncs;
};

class mme {

};
#endif
