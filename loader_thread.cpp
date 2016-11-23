#include "loader_thread.hpp"

#include <functional>

#define N_CARACTERES_ESPERADO_LINHA 150

using namespace std;

string logparser::readline(ifstream &filehandle) {
    string resultado;
    char byte(0);
    resultado.reserve(N_CARACTERES_ESPERADO_LINHA);
    while (filehandle.good()) {
        filehandle.read(&byte, 1);
        if (byte == '\n')
            break;
        else {
            if (byte == '\r')
                continue;
            resultado.push_back(byte);
        }
    }
    return resultado;
}

logparser::parser::parser() {
    triggers.insert( {"cell_3g", regex{"SERVICE AREAS IN MSS CONCEPT \\:"}});
    triggers.insert( {"cell_3g_end",regex{"RNCs IN MSS CONCEPT LOCATED IN LOCATION AREA \\:"}} );
    triggers.insert( {"cell_2g", regex{"BTSs UNDER BSC \\:"}});
    triggers.insert( {"bscname", regex{"(?:BSC NAME )(?:\\.*)(?:\\(NAME\\)\\. \\:)(\\w+)"}});
    triggers.insert( {"rncid", regex{"(?:RNC IDENTIFICATION\\.+ RNCID \\.+ \\: )(\\d+)"}});
    triggers.insert( {"rnc_load_lac",regex{"(?:LA +NAME \\:LAC)(\\d+)(?: +LAC +\\: *)(\\d+)"}});
    triggers.insert( {"enodeb", regex{"ENB IN RADIO NETWORK"}} );
    triggers.insert( {"enodeb_end", regex{"BROADCAST PLMN\\:"}} );
    regexers.insert( {"switchname", regex{"(?:MSCi +)(\\w+)(?: +)(\\d{4}-\\d{2}-\\d{2} +\\d{2}\\:\\d{2}\\:\\d{2})"}});
    regexers.insert( {"cell_2g", regex{"(?:^ +)(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\w+)"}});
    regexers.insert( {"rncname", regex{"(?:RNC NAME\\.+ RNCNAME \\. \\: )(\\w+)"}});
    regexers.insert( {"rnc_sac", regex{"(\\d+)(?: +724 +)(\\d+)"}});
    regexers.insert( {"command_end", regex{"COMMAND +EXECUTED"}});
    regexers.insert( {"cell_3g", regex{"(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\w+)"}});
    regexers.insert( {"enodeb_id",regex{"(?:ENB IDENTIFICATION\\.+\\(ENBID\\)\\.+ \\: )(\\d+)"}});
    regexers.insert( {"inodeb_ip", regex{"(?:ENB IP ADDRESS\\.+\\(ENBIP\\)\\.+ \\: )(\\d+\\.\\d+\\.\\d+\\.\\d+)"}});
    regexers.insert( {"tac", regex{"(?:TRACKING AREA CODE\\.+\\(TAC#1 \\)\\.+ \\: )(\\d+)"}});
    regexers.insert( {"enb_mcc", regex{"(?:MOBILE COUNTRY CODE\\.+\\(MCC\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert( {"enb_mnc", regex{"(?:MOBILE NETWORK CODE\\.+\\(MNC\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert( {"enb_id", regex{"(?:ENB IDENTIFICATION\\.+\\(\\ENBID\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert( {"enb_ip",regex{"(?:ENB IP ADDRESS\\.+\\(ENBIP\\)\\.+ \\: )(\\d+\\.\\d+\\.\\d+\\.\\d+)"}} );
    regexers.insert( {"enb_s1ca", regex{"(?:S1 CONNECTION AMOUNT\\.+\\(S1CA\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert( {"enb_tac", regex{"(?:TRACKING AREA CODE\\.+\\(TAC.. *\\)\\.* \\: )(\\d+)"}} );
}
logparser::parser::~parser() {}

void logparser::parser::clean_database() {
    if (!db_interface.connection_up()) {
        string configuration_file=signal_ask_for_connect_string_file_.emit();
        statements.clear();
        db_interface.load_database_config(configuration_file);
        try {
            db_interface.connect_to_database();
        } catch (const runtime_error& erro) {
            cerr << erro.what() << endl;
            return;
        }
    }
    try {
        db_interface.execute_command("drop table if exists carrier.mobswitch cascade");
        db_interface.execute_command("drop table if exists carrier.controller cascade");
        db_interface.execute_command("drop table if exists carrier.relate_cont_switch cascade");
        db_interface.execute_command("drop table if exists carrier.cells cascade");
        db_interface.execute_command("drop table if exists carrier.relate_cells_controller cascade");
        db_interface.execute_command("drop table if exists carrier.mme cascade");
        db_interface.execute_command("drop table if exists carrier.enodeb cascade");
        db_interface.execute_command("drop table if exists carrier.relate_enodeb_mme cascade");
        db_interface.execute_command("drop sequence if exists carrier.cells_id_seq");
        db_interface.execute_command("drop sequence if exists carrier.controller_id_seq");
        db_interface.execute_command("drop sequence if exists carrier.mobswitch_id_seq");
        db_interface.execute_command("drop sequence if exists carrier.relate_cells_controller_id_seq");
        db_interface.execute_command("drop sequence if exists carrier.cont_switch_id_seq");
    } catch (const runtime_error& erro) {
        cerr << erro.what() << endl;
    }

}
void logparser::parser::slot_new_file(
    const string &filename,
    const logtype &type_of_logger) noexcept(false) {
    switch (type_of_logger) {
    case logtype::mobswitch:
        lista_de_mobswitches.push_back(filename);
        break;
    case logtype::mme:
        lista_de_mmes.push_back(filename);
        break;
    default:
        string message("Tipo de elemento de rede incorreto. Informado: ");
        message += to_string(type_of_logger) + '\n';
        throw(domain_error(message));
    }
    signal_n_of_mobswitches_.emit(to_string(lista_de_mobswitches.size()));
    signal_n_of_mmes_.emit(to_string(lista_de_mmes.size()));
}
sigc::signal<void, const string &>
logparser::parser::signal_n_of_mobswitches() {
    return signal_n_of_mobswitches_;
}

sigc::signal<void, const string &> logparser::parser::signal_n_of_mmes() {
    return signal_n_of_mmes_;
}
sigc::signal<void> logparser::parser::signal_work_finish() {
    return signal_work_finish_;
}
sigc::signal<void, const string &, const string&, const string&>
logparser::parser::signal_n_of_mobswitches_ready() {
    return signal_n_of_mobswitches_ready_;
}

sigc::signal<void, const string &> logparser::parser::signal_n_of_mmes_ready() {
    return signal_n_of_mmes_ready_;
}

sigc::signal<string> logparser::parser::signal_ask_for_connect_string_file() {
    return signal_ask_for_connect_string_file_;
}

sigc::signal<void, string> logparser::parser::signal_send_upload_node() {
    return signal_send_upload_node_;
}

void logparser::parser::slot_run() noexcept(false) {
    size_t n_bsc(0), n_rnc(0);
    vector<shared_ptr<::mobswitch>> temporary_mobswitches;
    vector<shared_ptr<::mme>> temporary_mmes;
    for (vector<string>::iterator siter = lista_de_mobswitches.begin();
            siter != lista_de_mobswitches.end(); ++siter) {
        shared_ptr<::mobswitch> work_switch = make_shared<::mobswitch>();
        temporary_mobswitches.push_back(work_switch);
        /*threads_de_execucao.insert({*siter, make_shared<boost::thread>([&]() {
            logparser::parse_switch(work_switch,
                                    cref(*siter),
                                    triggers, regexers);
        })
                                   });*/
        parse_switch(work_switch,cref(*siter));
        n_bsc+=work_switch->get_number_bscs();
        n_rnc+=work_switch->get_number_rncs();
        signal_n_of_mobswitches_ready_.emit(to_string(temporary_mobswitches.size()),to_string(n_bsc),
                                            to_string(n_rnc));
    }
    for (vector<string>::const_iterator miter = lista_de_mmes.cbegin();
            miter != lista_de_mmes.cend(); ++miter) {
        shared_ptr<::mme> work_mme = make_shared<::mme>();
        temporary_mmes.push_back(work_mme);
        parse_mme(work_mme,*miter);
        signal_n_of_mmes_ready_.emit(to_string(temporary_mmes.size()));

        // threads_de_execucao.insert({*miter,make_shared<boost::thread>([&](){parse_mme(work_mme,cref(*miter));})});
        // //TODO Tem de preparar a função para lidar com as mmes
        // parse_switch(work_switch,switch_filename);
    }
    for (map<string, shared_ptr<boost::thread>>::iterator titer =
                threads_de_execucao.begin();
            titer != threads_de_execucao.end(); ++titer) {
        titer->second->join();
    }
    for (vector<shared_ptr<::mobswitch>>::iterator tmiter=temporary_mobswitches.begin();
            tmiter!=temporary_mobswitches.end(); ++tmiter) {
        vector<shared_ptr<::mobswitch>>::iterator miter=working_switches.end();
        for (vector<shared_ptr<::mobswitch>>::iterator rmiter=working_switches.begin();
                rmiter!=working_switches.end(); ++rmiter) {
            if (*(rmiter->get()) == *(tmiter->get())) {
                miter=rmiter;
                break;
            }
        }
        if (miter == working_switches.end()) working_switches.push_back(*tmiter);
        else *(miter->get()) += *(tmiter->get());
    }
    for (vector<shared_ptr<::mme>>::const_iterator tmiter=temporary_mmes.cbegin();
            tmiter!=temporary_mmes.cend(); ++tmiter) {
        working_mmes.push_back(*tmiter); //FIXME For now I have only one file for each mme.
    }
    lista_de_mobswitches.clear();
    lista_de_mmes.clear();
    signal_work_finish_.emit();

    //boost::this_thread::sleep_for(boost::chrono::milliseconds(400));
}

void logparser::parser::parse_switch(std::shared_ptr<::mobswitch> work_switch,
                                     const string &filename) {
    // DEBUG
    cout << filename << " sendo tratado na thread "
         << boost::this_thread::get_id() << endl;
    // ENDDEBUG
    ifstream workfile;
    workfile.open(filename, ios_base::in);
    bool switch_identified(false);
    bool t_bsc(false), t_rnc(false), t_cell2g(false), t_cell3g(false);
    vector<bsc>::iterator it_work_bsc;
    vector<rnc>::iterator it_work_rnc;
    int work_lac_sac(-1);
    smatch matches;
    if (workfile.fail()) {
        cerr << "Não foi possível abrir o arquivo " << filename << "." << endl << "Abortando Arquivo." <<
             endl;
        return;
    }
    while (workfile.good()) {
        string linha(readline(workfile));
        if (linha.size() < 4)
            continue;
        // My first step shall be identify the switch. While it's not possible,
        // theres no sense in continuing
        if (!switch_identified) {
            if (regex_search(linha, matches, regexers.at("switchname"))) {
                switch_identified = true;
                work_switch->set_name(matches[1]);
            }
            continue;
        }
        // Switch is identified, now check if I should stop any triggers. There
        // should be triggers for bsc, cell_2g, rnc and cell_3g
        if (regex_search(linha, matches, regexers.at("command_end"))) {
            t_bsc = t_rnc = t_cell2g = t_cell3g = false;
            continue;
        }
        // Now we should check if we find any of the triggers
        // NOTE 1 The trigger for bsc is already its researcher. Lets start with it.
        if (regex_search(linha, matches, triggers.at("bscname"))) {
            t_bsc = true;
            bsc n_bsc;
            n_bsc.set_name(matches[1]);
            it_work_bsc = work_switch->add_bsc(n_bsc);
            continue;
        }
        if (regex_search(linha, matches, triggers.at("cell_2g"))) {
            if (!t_bsc) {
                string message("Começaram celulas 2g sem que fosse possível descobrir o SAC.\nCélula: ");
                message += matches[1];
                throw(runtime_error(message));
            }
            t_cell3g = t_rnc = false;
            t_cell2g = true;
            continue;
        }
        if (regex_search(linha, matches, triggers.at("rnc_load_lac"))) {
            t_cell2g = t_bsc = t_rnc = false;
            work_lac_sac = stoi(matches[1]);
            continue;
        }
        if (regex_search(linha, matches, triggers.at("cell_3g"))) {
            if (work_lac_sac < 0) {
                string message("Começaram celulas 3g sem que fosse possível descobrir o SAC.\nCélula: ");//FIXME Problems with false positives.
                message += matches[1];
                //throw(runtime_error(message));
            }
            t_cell2g = t_bsc = t_rnc = false;
            t_cell3g = true;
            continue;
        }
        if (regex_search(linha,matches, triggers.at("cell_3g_end"))) {
            t_cell3g=false;
            continue;
        }
        if (regex_search(linha, matches, triggers.at("rncid"))) {
            t_bsc = t_cell2g = t_cell3g = false;
            // it_work_bsc=nullptr; TODO See if something like this is really
            // necessary
            t_rnc = true;
            rnc n_rnc;
            n_rnc.set_id(matches[1]);
            it_work_rnc = work_switch->add_rnc(n_rnc);
            continue;
        }
        // From this point on we grab the available information
        if (t_cell2g) {
            if (regex_search(linha, matches, regexers.at("cell_2g"))) {
                celula n_celula;
                n_celula.set_name(matches[1]);
                n_celula.set_cgi(matches[3], matches[4], matches[2], matches[5]);
                n_celula.set_status(matches[6]);
                it_work_bsc->add_celula(n_celula);
                continue;
            }
        }
        if (t_rnc) {
            if (regex_search(linha, matches, regexers.at("rncname"))) {
                it_work_rnc->set_name(matches[1]);
                continue;
            }
            if (regex_search(linha, matches, regexers.at("rnc_sac"))) {
                it_work_rnc->add_lac_sac(matches[1]);
                it_work_rnc->set_mnc(matches[2]);
                continue;
            }
        }
        if (t_cell3g) {
            if (regex_search(linha, matches, regexers.at("cell_3g"))) {
                celula n_celula;
                n_celula.set_name(matches[1]);
                n_celula.set_mcc(724);
                n_celula.set_mnc(it_work_rnc->get_mnc());
                n_celula.set_lac_sac(work_lac_sac);
                n_celula.set_cid(stoi(matches[2]));
                n_celula.set_status(matches[3]);
                it_work_rnc->add_celula(n_celula);
                continue;
            }
        }
    }
    workfile.close();
}
void logparser::parser::parse_mme(std::shared_ptr<::mme> work_mme, const string &filename) {
    ifstream file(filename,ios_base::in);
    bool n_enb(false);
    smatch matches;
    regex_search(filename,matches,regex{"(.*?)(\\.log)"}); //Hack for my situation. Couldn't find the mme identification in file.
    work_mme->set_name(matches[1]);
    vector<enodeb>::iterator it_enb;
    while (file.good()) {
        string linha(readline(file));
        if (regex_search(linha,matches,triggers.at("enodeb_end"))) {
            n_enb=false;
            continue;
        }
        if (regex_search(linha,matches,triggers.at("enodeb"))) {
            n_enb=true;
            continue;
        }
        if (n_enb) {
            if (regex_search(linha,matches,regexers.at("enb_mcc"))) {
                enodeb n_enb;
                n_enb.set_mcc(matches[1]);
                it_enb=work_mme->add_enodeb(n_enb);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("enb_mnc"))) {
                it_enb->set_mnc(matches[1]);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("enb_id"))) {
                it_enb->set_enbid(matches[1]);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("enb_ip"))) {
                it_enb->set_ip(matches[1]);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("enb_s1ca"))) {
                it_enb->set_s1ca(matches[1]);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("enb_tac"))) {
                it_enb->set_tac(matches[1]);
                continue;
            }
        }
    }
    file.close();
}

void logparser::parser::slot_upload_data() noexcept {
    if (!db_interface.connection_up()) {
        string configuration_file=signal_ask_for_connect_string_file_.emit();
        statements.clear();
        db_interface.load_database_config(configuration_file);
        try {
            db_interface.connect_to_database();
        } catch (const runtime_error& erro) {
            cerr << erro.what() << endl;
            return;
        }
    }
    //Set Database UP
    try {
        db_interface.execute_command("create schema if not exists carrier;");
        db_interface.execute_command("create table if not exists carrier.mobswitch (id serial, name text, primary key(name))");
        db_interface.execute_command("create table if not exists carrier.controller (id serial, name text, type varchar(5), primary key(name))");
        db_interface.execute_command("create table if not exists carrier.relate_cont_switch (id serial, id_controller integer, id_mobswitch integer, unique (id_controller, id_mobswitch))");
        db_interface.execute_command("create table if not exists carrier.cells (id serial, name text, mcc integer, mnc integer, lac integer, sac_cid integer, status text, primary key (mcc,mnc,lac,sac_cid))");
        db_interface.execute_command("create table if not exists carrier.relate_cells_controller (id serial, id_cell integer, id_cont integer, unique (id_cell,id_cont))");
        db_interface.execute_command("create table if not exists carrier.mme (id serial, name text, primary key(name))");
        db_interface.execute_command("create table if not exists carrier.enodeb (id serial, ip inet, mcc integer, mnc integer, tac integer, enbid integer, s1ca integer, primary key(mcc,mnc,tac,enbid))");
        db_interface.execute_command("create table if not exists carrier.relate_enodeb_mme (id serial, id_enodeb integer, id_mme integer, primary key(id_enodeb, id_mme))");
    } catch (const runtime_error& erro) {
        cerr << erro.what() << endl;
        return;
    }
    //Set up the Statements
    try {
        if (statements.count("insert_mobswitch") == 0) {
            db_interface.prepare_statement("insert_mobswitch",
                                           "insert into carrier.mobswitch (name) values ($1) on conflict do nothing returning id",1);
            statements.insert("insert_mobswitch");
        }
        if (statements.count("insert_controller") == 0) {
            db_interface.prepare_statement("insert_controller",
                                           "insert into carrier.controller (name, type) values ($1, $2) returning id",2);
            statements.insert("insert_controller");
        }
        if (statements.count("find_inserted_controller") == 0) {
            db_interface.prepare_statement("find_inserted_controller",
                                           "select id from carrier.controller where name = $1",1);
            statements.insert("find_inserted_controller");
        }
        if (statements.count("insert_cell") == 0) {
            db_interface.prepare_statement("insert_cell",
                                           "insert into carrier.cells (name, mcc, mnc, lac, sac_cid, status) values($1, $2::integer, $3::integer, $4::integer, $5::integer, $6) on conflict do nothing returning id",
                                           6);
            statements.insert("insert_cell");
        }
        if (statements.count("find_inserted_cell") == 0) {
            db_interface.prepare_statement("find_inserted_cell",
                                           "select id from carrier.cells where mcc=$1::integer and mnc=$2::integer and lac=$3::integer and"
                                           " sac_cid=$4::integer",
                                           4);
            statements.insert("find_inserted_cell");
        }
        if (statements.count("insert_relation_controller_switch") == 0) {
            db_interface.prepare_statement("insert_relation_controller_switch",
                                           "insert into carrier.relate_cont_switch (id_controller, id_mobswitch) values ($1, $2) on conflict do nothing",
                                           2);
            statements.insert("insert_relation_controller_switch");
        }
        if (statements.count("insert_relation_cell_controller") == 0) {
            db_interface.prepare_statement("insert_relation_cell_controller",
                                           "insert into carrier.relate_cells_controller (id_cell, id_cont) values ($1, $2) on conflict do nothing",
                                           2);
            statements.insert("insert_relation_cell_controller");
        }
        if (statements.count("insert_mme") == 0) {
            db_interface.prepare_statement("insert_mme",
                                           "insert into carrier.mme (name) values ($1) on conflict do nothing returning id",1);
            statements.insert("insert_mme");
        }
        if (statements.count("insert_enodeb") == 0) {
            db_interface.prepare_statement("insert_enodeb",
                                           "insert into carrier.enodeb (ip,mcc,mnc,tac,enbid,s1ca) values ($1,$2::integer,$3::integer,$4::integer, $5::integer, $6::integer)",
                                           6);
            statements.insert("insert_enodeb");
        }
        if (statements.count("insert_relation_enodeb_mme") == 0) {
            db_interface.prepare_statement("insert_relation_enodeb_mme",
                                           "insert into carrier.relate_enodeb_mme (id_enodeb, id_mme) values ($1, $2)",2);
            statements.insert("insert_relation_enodeb_mme");
        }
        if (statements.count("find_inserted_enodeb") == 0) {
            db_interface.prepare_statement("find_inserted_enodeb",
                                           "select id from carrier.enodeb where mcc=$1 and mnc=$2 and tac=$3 and enbid=$4",4);
            statements.insert("find_inserted_enodeb");
        }
    } catch (const runtime_error& erro) {
        cerr << erro.what() << endl;
        return;
    }
    //Let the upgames begin
    //Send the Mobile Switches
    for (vector<shared_ptr<::mobswitch>>::iterator miter=working_switches.begin();
            miter!=working_switches.end(); ++miter) {
        string id_switch, id_controller, id_cell;
        pgsql::query_result work_result;
        //Firstly insert the Switch register
        signal_send_upload_node_.emit(miter->get()->get_name());
        work_result=db_interface.execute_returning_prepared_statement("insert_mobswitch", {miter->get()->get_name()});
        if (work_result.success()) {
            id_switch=work_result.get_value(0,0);
        } else {
            signal_send_upload_node_.emit("Not able to insert register of :" + miter->get()->get_name());
            return;
        }
        //Now, lets loop on bsc list
        for (vector<bsc>::const_iterator biter=miter->get()->bsc_begin(); biter!=miter->get()->bsc_end();
                ++biter) {
            //We shall insert the bsc data and its relation with the switch
            signal_send_upload_node_.emit(biter->get_name());
            try {
                work_result=db_interface.execute_returning_prepared_statement("insert_controller", {biter->get_name(),"BSC"});
                id_controller=work_result.get_value(0,0);
            } catch (const runtime_error& erro) {
                cerr << erro.what() << endl;
                work_result=db_interface.execute_returning_prepared_statement("find_inserted_controller", {biter->get_name()});
                id_controller=work_result.get_value(0,0);
            }

            db_interface.execute_prepared_statement("insert_relation_controller_switch", {id_controller,id_switch});
            //Loop, and this time for the cells
            for (vector<celula>::const_iterator citer=biter->cell_begin(); citer!=biter->cell_end(); ++citer) {
                //Insert cell data
                try {
                    work_result=db_interface.execute_returning_prepared_statement("insert_cell", {citer->get_name(),citer->get_mcc(),citer->get_mnc(),citer->get_lac_sac(),citer->get_cid(), citer->get_status()});
                    id_cell=work_result.get_value(0,0);
                } catch (const runtime_error& erro) {
                    cerr << erro.what() << endl;
                    work_result=db_interface.execute_returning_prepared_statement("find_inserted_cell", {citer->get_mcc(),citer->get_mnc(),citer->get_lac_sac(),citer->get_cid()});
                    id_cell=work_result.get_value(0,0);
                }
                db_interface.execute_prepared_statement("insert_relation_cell_controller", {id_cell,id_controller});
            }
        }
        //Now, lets loop on RNC list
        for (vector<rnc>::const_iterator riter=miter->get()->rnc_begin(); riter!=miter->get()->rnc_end();
                ++riter) {
            //We shall insert the bsc data and its relation with the switch
            signal_send_upload_node_.emit(riter->get_name());
            try {
                work_result=db_interface.execute_returning_prepared_statement("insert_controller", {riter->get_name(),"RNC"});
                id_controller=work_result.get_value(0,0);
            } catch (const runtime_error& erro) {
                cerr << erro.what() << endl;
                work_result=db_interface.execute_returning_prepared_statement("find_inserted_controller", {riter->get_name()});
                id_controller=work_result.get_value(0,0);
            }
            db_interface.execute_prepared_statement("insert_relation_controller_switch", {id_controller,id_switch});
            //Loop, and this time for the cells
            for (vector<celula>::const_iterator citer=riter->cell_begin(); citer!=riter->cell_end(); ++citer) {
                //Insert cell data
                try {
                    work_result=db_interface.execute_returning_prepared_statement("insert_cell", {citer->get_name(),citer->get_mcc(),citer->get_mnc(),citer->get_lac_sac(),citer->get_cid(),citer->get_status()});
                    id_cell=work_result.get_value(0,0);
                } catch (const runtime_error& erro) {
                    cerr << erro.what() << endl;
                    work_result=db_interface.execute_returning_prepared_statement("find_inserted_cell", {citer->get_mcc(),citer->get_mnc(),citer->get_lac_sac(),citer->get_cid()});
                    id_cell=work_result.get_value(0,0);
                }
                db_interface.execute_prepared_statement("insert_relation_cell_controller", {id_cell,id_controller});
            }
        }
    }
    //Now to the mmes
    for (vector<shared_ptr<::mme>>::const_iterator miter=working_mmes.cbegin();
            miter!=working_mmes.cend(); ++miter) {
        string id_mme, id_enodeb;
        pgsql::query_result work_result;
        work_result=db_interface.execute_returning_prepared_statement("insert_mme", {miter->get()->get_name()});
        id_mme=work_result.get_value(0,0);
        //Now we loop for the enodebs
        for (vector<enodeb>::const_iterator enbiter=miter->get()->enodeb_begin();
                enbiter!=miter->get()->enodeb_end(); ++enbiter) {
            try { //(ip,mcc,mnc,tac,enbid,s1ca)
                work_result=db_interface.execute_returning_prepared_statement("insert_enodeb", {enbiter->get_ip(),enbiter->get_mcc(),enbiter->get_mnc(),enbiter->get_tac(),enbiter->get_enbid(),enbiter->get_s1ca()});
                id_enodeb=work_result.get_value(0,0);
            } catch (const runtime_error& erro) {
                cerr << erro.what() << endl; //See if its a problem for now
                work_result=db_interface.execute_returning_prepared_statement("find_inserted_enodeb", {enbiter->get_mcc(),enbiter->get_mnc(),enbiter->get_tac(),enbiter->get_enbid()});
                id_enodeb=work_result.get_value(0,0);
            }
            db_interface.execute_prepared_statement("insert_relation_enodeb_mme", {id_enodeb,id_mme});
        }
    }
    signal_send_upload_node_.emit("Encerrado.");
}
