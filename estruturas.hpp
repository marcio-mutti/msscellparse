#ifndef ESTRUTURAS
#define ESTRUTURAS

# include <map>
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

class enodeb {
  public:
    enodeb();
    ~enodeb();
    void set_ip(const std::string&);
    void set_mcc(const std::string&);
    void set_mnc(const std::string&);
    void set_tac(const std::string&);
    void set_enbid(const std::string&);
    void set_s1ca(const std::string&);
    std::string get_ip () const;
    std::string get_mcc () const;
    std::string get_mnc () const;
    std::string get_tac () const;
    std::string get_enbid () const;
    std::string get_s1ca () const;
    bool operator==(const enodeb&) const;
  protected:
    std::string ip;
    unsigned long int mcc, mnc, tac, enbid, s1ca;
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
  public:
    mme();
    ~mme();
    void set_name(const std::string&);
    std::string get_name() const;
    std::vector<enodeb>::iterator add_enodeb(const enodeb&);
    std::vector<enodeb>::iterator find_enodeb(const enodeb&);
    std::vector<enodeb>::const_iterator enodeb_begin() const;
    std::vector<enodeb>::const_iterator enodeb_end() const;
  protected:
    std::string name;
    std::vector<enodeb> lista_de_enodebs;
};

class ss7_route {
  public:
    ss7_route();
    ~ss7_route();
    void set_name(const std::string&);
    std::string get_name() const;
    void set_dpc(const unsigned long int&);
    unsigned long int get_dpc() const;
    void add_connection(const unsigned long&);
    std::vector<unsigned long int>::const_iterator conn_begin() const;
    std::vector<unsigned long int>::const_iterator conn_end() const;
  protected:
    std::string name;
    unsigned long int dpc;
    std::vector<unsigned long int> conections;
};

class ss7_node {
  public:
    ss7_node();
    ~ss7_node();
    void set_name(const std::string&);
    std::string get_name() const;
    void set_opc(const unsigned long int&);
    unsigned long int get_opc() const;
    std::vector<ss7_route>::iterator add_route(const ss7_route&);
    void add_network(const std::string&);
    std::vector<ss7_route>::iterator add_full_route(const std::string&,const std::string&,
            const std::string&, const std::string&);
    void add_redundant_route(std::vector<ss7_route>::iterator&, const std::string&);
    std::map<std::string,std::vector<ss7_route>>::const_iterator netbegin() const;
    std::map<std::string,std::vector<ss7_route>>::const_iterator netend() const;
    bool operator==(const unsigned int&) const;
    bool operator==(const ss7_node&) const;
  protected:
    std::string name;
    unsigned long int opc;
    std::map<std::string,std::vector<ss7_route>> networks;
    std::vector<ss7_route> route_list;

};

#endif
